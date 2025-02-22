#include "Protocol.h"
#include "Utils.h"
#include "CongestionControl.h"
#include "LoadBalancer.h"
#include "TcpChunkOptimization.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdexcept>
#include <vector>

#pragma comment(lib, "ws2_32.lib")

class Protocol::ProtocolImpl
{
public:
    ProtocolImpl()
        : socket_(INVALID_SOCKET), isConnected_(false)
    {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        {
            throw std::runtime_error("WSAStartup failed");
        }
    }

    ~ProtocolImpl()
    {
        closeConnection();
        WSACleanup();
    }

    bool initializeConnection(const std::string &host, uint16_t port)
    {
        socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (socket_ == INVALID_SOCKET)
        {
            return false;
        }

        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);
        inet_pton(AF_INET, host.c_str(), &(serverAddr.sin_addr));

        if (connect(socket_, (sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
        {
            closesocket(socket_);
            socket_ = INVALID_SOCKET;
            return false;
        }

        isConnected_ = true;
        return true;
    }

    void setCongestionControlAlgorithm(CongestionControl::Algorithm algo)
    {
        congestionControl_.setAlgorithm(algo);
    }

    bool sendData(const std::string &data)
    {
        if (!isConnected_)
            return false;

        std::vector<uint8_t> dataBytes(data.begin(), data.end());
        auto chunks = tcpChunkOptimizer_.chunkData(dataBytes);
        uint32_t windowSize = congestionControl_.getCurrentWindow();

        for (const auto &chunk : chunks)
        {
            size_t totalSent = 0;
            while (totalSent < chunk.size())
            {
                int sent = send(socket_,
                                (char *)&chunk[totalSent],
                                (windowSize < (uint32_t)(chunk.size() - totalSent) ? windowSize : (uint32_t)(chunk.size() - totalSent)),
                                0);

                if (sent == SOCKET_ERROR)
                {
                    congestionControl_.updateWindow(false, true);
                    return false;
                }

                totalSent += sent;
                congestionControl_.updateWindow(true, false);
            }
        }

        return true;
    }

    std::string receiveData()
    {
        if (!isConnected_)
            return "";

        std::vector<uint8_t> receivedData;
        char buffer[4096];
        int bytesReceived;

        do
        {
            bytesReceived = recv(socket_, buffer, sizeof(buffer), 0);
            if (bytesReceived > 0)
            {
                receivedData.insert(receivedData.end(), buffer, buffer + bytesReceived);
            }
        } while (bytesReceived == sizeof(buffer));

        if (bytesReceived == SOCKET_ERROR)
        {
            return "";
        }

        return std::string(receivedData.begin(), receivedData.end());
    }

    void closeConnection()
    {
        if (socket_ != INVALID_SOCKET)
        {
            closesocket(socket_);
            socket_ = INVALID_SOCKET;
        }
        isConnected_ = false;
    }

private:
    SOCKET socket_;
    bool isConnected_;
    CongestionControl congestionControl_;
    TcpChunkOptimization tcpChunkOptimizer_;
    LoadBalancer loadBalancer_;
};

// Protocol类的公共方法实现
Protocol::Protocol() : impl(new ProtocolImpl()) {}
Protocol::~Protocol() = default;

bool Protocol::initializeConnection(const std::string &host, uint16_t port)
{
    return impl->initializeConnection(host, port);
}

void Protocol::setCongestionControlAlgorithm(CongestionControl::Algorithm algo)
{
    impl->setCongestionControlAlgorithm(algo);
}

bool Protocol::sendData(const std::string &data)
{
    return impl->sendData(data);
}

std::string Protocol::receiveData()
{
    return impl->receiveData();
}

void Protocol::closeConnection()
{
    impl->closeConnection();
}
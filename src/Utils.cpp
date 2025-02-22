#include "Utils.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdexcept>
#include <random>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

double NetworkUtils::measureLatency(const std::string& host, uint16_t port) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        throw std::runtime_error("WSAStartup failed");
    }

    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        WSACleanup();
        throw std::runtime_error("Socket creation failed");
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), &(serverAddr.sin_addr));

    auto start = std::chrono::high_resolution_clock::now();
    
    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        closesocket(sock);
        WSACleanup();
        throw std::runtime_error("Connection failed");
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    closesocket(sock);
    WSACleanup();

    return duration.count() / 1000.0; // 转换为毫秒
}

double NetworkUtils::measureBandwidth(const std::string& host, uint16_t port) {
    const size_t TEST_SIZE = 1024 * 1024; // 1MB的测试数据
    std::vector<uint8_t> testData(TEST_SIZE);
    
    // 生成随机测试数据
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    for (auto& byte : testData) {
        byte = static_cast<uint8_t>(dis(gen));
    }

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        throw std::runtime_error("WSAStartup failed");
    }

    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        WSACleanup();
        throw std::runtime_error("Socket creation failed");
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), &(serverAddr.sin_addr));

    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        closesocket(sock);
        WSACleanup();
        throw std::runtime_error("Connection failed");
    }

    auto start = std::chrono::high_resolution_clock::now();
    
    // 发送测试数据
    int totalSent = 0;
    while (totalSent < TEST_SIZE) {
        int sent = send(sock, (char*)&testData[totalSent], TEST_SIZE - totalSent, 0);
        if (sent == SOCKET_ERROR) {
            closesocket(sock);
            WSACleanup();
            throw std::runtime_error("Send failed");
        }
        totalSent += sent;
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    closesocket(sock);
    WSACleanup();

    // 计算带宽（MB/s）
    return (TEST_SIZE / 1024.0 / 1024.0) / (duration.count() / 1000000.0);
}

bool NetworkUtils::checkConnection(const std::string& host, uint16_t port) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        return false;
    }

    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        WSACleanup();
        return false;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), &(serverAddr.sin_addr));

    // 设置非阻塞模式
    u_long mode = 1;
    ioctlsocket(sock, FIONBIO, &mode);

    connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr));

    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(sock, &fdset);

    timeval tv;
    tv.tv_sec = 1;  // 1秒超时
    tv.tv_usec = 0;

    bool connected = false;
    if (select(0, nullptr, &fdset, nullptr, &tv) == 1) {
        int error;
        int len = sizeof(error);
        getsockopt(sock, SOL_SOCKET, SO_ERROR, (char*)&error, &len);
        connected = (error == 0);
    }

    closesocket(sock);
    WSACleanup();
    return connected;
}

uint32_t NetworkUtils::calculateChecksum(const std::vector<uint8_t>& data) {
    uint32_t checksum = 0;
    for (size_t i = 0; i < data.size(); i++) {
        checksum = (checksum << 8) | data[i];
        if (checksum & 0xFF000000) {
            checksum = (checksum & 0x00FFFFFF) + ((checksum >> 24) & 0xFF);
        }
    }
    return checksum;
}

bool NetworkUtils::isValidIpAddress(const std::string& ipAddress) {
    struct sockaddr_in sa;
    return inet_pton(AF_INET, ipAddress.c_str(), &(sa.sin_addr)) == 1;
}

std::chrono::system_clock::time_point NetworkUtils::getCurrentTimestamp() {
    return std::chrono::system_clock::now();
}
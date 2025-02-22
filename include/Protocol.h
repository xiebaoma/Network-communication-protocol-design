#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <string>
#include <memory>
#include "CongestionControl.h"
#include "LoadBalancer.h"
#include "TcpChunkOptimization.h"

class Protocol {
public:
    Protocol();
    ~Protocol();

    // 初始化连接
    bool initializeConnection(const std::string& host, uint16_t port);
    
    // 设置拥塞控制算法
    void setCongestionControlAlgorithm(CongestionControl::Algorithm algo);
    
    // 发送数据
    bool sendData(const std::string& data);
    
    // 接收数据
    std::string receiveData();
    
    // 关闭连接
    void closeConnection();

private:
    class ProtocolImpl;
    std::unique_ptr<ProtocolImpl> impl;
};

#endif // PROTOCOL_H
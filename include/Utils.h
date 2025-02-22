#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <chrono>

class NetworkUtils {
public:
    // 测量网络延迟
    static double measureLatency(const std::string& host, uint16_t port);
    
    // 测量网络带宽
    static double measureBandwidth(const std::string& host, uint16_t port);
    
    // 检查网络连接状态
    static bool checkConnection(const std::string& host, uint16_t port);
    
    // 计算校验和
    static uint32_t calculateChecksum(const std::vector<uint8_t>& data);
    
    // 转换地址格式
    static bool isValidIpAddress(const std::string& ipAddress);
    
    // 获取系统当前时间戳
    static std::chrono::system_clock::time_point getCurrentTimestamp();
};

#endif // UTILS_H
#ifndef TCP_CHUNK_OPTIMIZATION_H
#define TCP_CHUNK_OPTIMIZATION_H

#include <cstdint>
#include <vector>

class TcpChunkOptimization {
public:
    TcpChunkOptimization();
    
    // 设置最大分块大小
    void setMaxChunkSize(uint32_t size);
    
    // 设置最小分块大小
    void setMinChunkSize(uint32_t size);
    
    // 根据网络状况动态调整分块大小
    void adjustChunkSize(double networkQuality);
    
    // 将数据分块
    std::vector<std::vector<uint8_t>> chunkData(const std::vector<uint8_t>& data);
    
    // 合并数据块
    std::vector<uint8_t> mergeChunks(const std::vector<std::vector<uint8_t>>& chunks);
    
    // 获取当前最优分块大小
    uint32_t getCurrentOptimalChunkSize() const;

private:
    uint32_t maxChunkSize;
    uint32_t minChunkSize;
    uint32_t currentChunkSize;
    
    // 计算最优分块大小
    uint32_t calculateOptimalChunkSize(double networkQuality);
};

#endif // TCP_CHUNK_OPTIMIZATION_H
#include "TcpChunkOptimization.h"
#include <algorithm>
#include <cmath>

TcpChunkOptimization::TcpChunkOptimization()
    : maxChunkSize(64 * 1024)  // 默认最大分块大小为64KB
    , minChunkSize(1024)       // 默认最小分块大小为1KB
    , currentChunkSize(8 * 1024) // 默认当前分块大小为8KB
{
}

void TcpChunkOptimization::setMaxChunkSize(uint32_t size) {
    maxChunkSize = size;
    // 确保当前分块大小不超过最大值
    currentChunkSize = std::min(currentChunkSize, maxChunkSize);
}

void TcpChunkOptimization::setMinChunkSize(uint32_t size) {
    minChunkSize = size;
    // 确保当前分块大小不小于最小值
    currentChunkSize = std::max(currentChunkSize, minChunkSize);
}

void TcpChunkOptimization::adjustChunkSize(double networkQuality) {
    // networkQuality 范围为0-1，1表示最佳网络状况
    currentChunkSize = calculateOptimalChunkSize(networkQuality);
}

std::vector<std::vector<uint8_t>> TcpChunkOptimization::chunkData(
    const std::vector<uint8_t>& data) {
    std::vector<std::vector<uint8_t>> chunks;
    size_t remaining = data.size();
    size_t offset = 0;

    while (remaining > 0) {
        // 计算当前块的大小
        size_t chunkSize = std::min(static_cast<size_t>(currentChunkSize), remaining);
        
        // 创建新的数据块
        std::vector<uint8_t> chunk(data.begin() + offset, 
                                 data.begin() + offset + chunkSize);
        chunks.push_back(chunk);

        offset += chunkSize;
        remaining -= chunkSize;
    }

    return chunks;
}

std::vector<uint8_t> TcpChunkOptimization::mergeChunks(
    const std::vector<std::vector<uint8_t>>& chunks) {
    // 计算总大小
    size_t totalSize = 0;
    for (const auto& chunk : chunks) {
        totalSize += chunk.size();
    }

    // 预分配内存
    std::vector<uint8_t> mergedData;
    mergedData.reserve(totalSize);

    // 合并所有数据块
    for (const auto& chunk : chunks) {
        mergedData.insert(mergedData.end(), chunk.begin(), chunk.end());
    }

    return mergedData;
}

uint32_t TcpChunkOptimization::getCurrentOptimalChunkSize() const {
    return currentChunkSize;
}

uint32_t TcpChunkOptimization::calculateOptimalChunkSize(double networkQuality) {
    // 根据网络质量动态调整分块大小
    // networkQuality 范围：0-1，1表示最佳网络状况
    
    // 使用对数函数使分块大小的变化更平滑
    double qualityFactor = std::log10(networkQuality * 9 + 1);
    
    // 计算最优分块大小
    uint32_t optimalSize = static_cast<uint32_t>(
        minChunkSize + (maxChunkSize - minChunkSize) * qualityFactor
    );

    // 确保分块大小在有效范围内
    return std::clamp(optimalSize, minChunkSize, maxChunkSize);
}
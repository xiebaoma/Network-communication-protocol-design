#ifndef CONGESTION_CONTROL_H
#define CONGESTION_CONTROL_H

#include <cstdint>

class CongestionControl {
public:
    enum class Algorithm {
        SLOW_START,
        CONGESTION_AVOIDANCE,
        FAST_RETRANSMIT,
        FAST_RECOVERY
    };
    
    // 新增：状态信息结构体
    struct StateInfo {
        Algorithm currentAlgorithm;
        uint32_t cwnd;
        uint32_t ssthresh;
    };
    
    CongestionControl();
    
    void updateWindow(bool ackReceived, bool timeout);
    uint32_t getCurrentWindow() const;
    void setAlgorithm(Algorithm algo);
    
    // 新增的方法
    StateInfo getStateInfo() const;
    void reset();
    void setThreshold(uint32_t threshold);

private:
    Algorithm currentAlgorithm;
    uint32_t cwnd;        // 拥塞窗口
    uint32_t ssthresh;    // 慢启动阈值
    
    void slowStart();
    void congestionAvoidance();
    void fastRetransmit();
    void fastRecovery();
};

#endif // CONGESTION_CONTROL_H
#include "CongestionControl.h"
#include <algorithm>

CongestionControl::CongestionControl()
    : currentAlgorithm(Algorithm::SLOW_START)
    , cwnd(1)              // 初始拥塞窗口大小为1个MSS
    , ssthresh(64)         // 初始慢启动阈值
{
}

void CongestionControl::updateWindow(bool ackReceived, bool timeout) {
    if (timeout) {
        // 超时时进行慢启动
        ssthresh = cwnd / 2;
        cwnd = 1;
        currentAlgorithm = Algorithm::SLOW_START;
        return;
    }

    if (!ackReceived) {
        // 收到三个重复ACK，进行快速重传
        ssthresh = cwnd / 2;
        cwnd = ssthresh + 3;  // 快速恢复
        currentAlgorithm = Algorithm::FAST_RECOVERY;
        return;
    }

    switch (currentAlgorithm) {
        case Algorithm::SLOW_START:
            slowStart();
            break;
        case Algorithm::CONGESTION_AVOIDANCE:
            congestionAvoidance();
            break;
        case Algorithm::FAST_RECOVERY:
            fastRecovery();
            break;
        case Algorithm::FAST_RETRANSMIT:
            fastRetransmit();
            break;
    }
}

uint32_t CongestionControl::getCurrentWindow() const {
    return cwnd;
}

void CongestionControl::setAlgorithm(Algorithm algo) {
    currentAlgorithm = algo;
}

void CongestionControl::slowStart() {
    cwnd *= 2;  // 指数增长
    if (cwnd >= ssthresh) {
        currentAlgorithm = Algorithm::CONGESTION_AVOIDANCE;
    }
}

void CongestionControl::congestionAvoidance() {
    // 加性增，每个RTT增加1个MSS
    cwnd += 1;
}

void CongestionControl::fastRetransmit() {
    ssthresh = cwnd / 2;
    cwnd = ssthresh + 3;
    currentAlgorithm = Algorithm::FAST_RECOVERY;
}

void CongestionControl::fastRecovery() {
    // 收到新的ACK后，将拥塞窗口设置为阈值
    cwnd = ssthresh;
    currentAlgorithm = Algorithm::CONGESTION_AVOIDANCE;
}

// 新增：获取当前拥塞控制状态信息
CongestionControl::StateInfo CongestionControl::getStateInfo() const {
    return {
        currentAlgorithm,
        cwnd,
        ssthresh
    };
}

// 新增：重置拥塞控制状态
void CongestionControl::reset() {
    cwnd = 1;
    ssthresh = 64;
    currentAlgorithm = Algorithm::SLOW_START;
}

// 新增：设置自定义阈值
void CongestionControl::setThreshold(uint32_t threshold) {
    ssthresh = threshold;
}
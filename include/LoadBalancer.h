#ifndef LOAD_BALANCER_H
#define LOAD_BALANCER_H

#include <vector>
#include <string>

class LoadBalancer {
public:
    enum class Strategy {
        ROUND_ROBIN,
        WEIGHTED_ROUND_ROBIN,
        LEAST_CONNECTIONS,
        RANDOM
    };

    LoadBalancer();
    
    void addNode(const std::string& address, uint16_t port, uint32_t weight = 1);
    void removeNode(const std::string& address, uint16_t port);
    std::pair<std::string, uint16_t> getNextNode();
    void setStrategy(Strategy strategy);
    void updateNodeStatus(const std::string& address, uint16_t port, bool isActive);

private:
    struct Node {
        std::string address;
        uint16_t port;
        uint32_t weight;
        uint32_t currentConnections;
        bool isActive;
    };

    std::vector<Node> nodes;
    Strategy currentStrategy;
    size_t currentIndex;
    // 添加私有辅助方法的声明
    std::pair<std::string, uint16_t> getRoundRobinNode();
    std::pair<std::string, uint16_t> getWeightedRoundRobinNode();
    std::pair<std::string, uint16_t> getLeastConnectionsNode();
    std::pair<std::string, uint16_t> getRandomNode();
};

#endif // LOAD_BALANCER_H
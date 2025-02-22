#include "LoadBalancer.h"
#include "Utils.h"
#include <algorithm>
#include <random>
#include <stdexcept>

LoadBalancer::LoadBalancer()
    : currentStrategy(Strategy::ROUND_ROBIN), currentIndex(0)
{
}

void LoadBalancer::addNode(const std::string &address, uint16_t port, uint32_t weight)
{
    // 检查节点是否已存在
    auto it = std::find_if(nodes.begin(), nodes.end(),
                           [&](const Node &node)
                           {
                               return node.address == address && node.port == port;
                           });

    if (it != nodes.end())
    {
        it->weight = weight;
        it->isActive = true;
    }
    else
    {
        nodes.push_back({address, port, weight, 0, true});
    }
}

void LoadBalancer::removeNode(const std::string &address, uint16_t port)
{
    nodes.erase(
        std::remove_if(nodes.begin(), nodes.end(),
                       [&](const Node &node)
                       {
                           return node.address == address && node.port == port;
                       }),
        nodes.end());
}

std::pair<std::string, uint16_t> LoadBalancer::getNextNode()
{
    if (nodes.empty())
    {
        throw std::runtime_error("No available nodes");
    }

    switch (currentStrategy)
    {
    case Strategy::ROUND_ROBIN:
        return getRoundRobinNode();
    case Strategy::WEIGHTED_ROUND_ROBIN:
        return getWeightedRoundRobinNode();
    case Strategy::LEAST_CONNECTIONS:
        return getLeastConnectionsNode();
    case Strategy::RANDOM:
        return getRandomNode();
    default:
        throw std::runtime_error("Unknown strategy");
    }
}

void LoadBalancer::setStrategy(Strategy strategy)
{
    currentStrategy = strategy;
    currentIndex = 0; // 重置索引
}

void LoadBalancer::updateNodeStatus(const std::string &address, uint16_t port, bool isActive)
{
    auto it = std::find_if(nodes.begin(), nodes.end(),
                           [&](const Node &node)
                           {
                               return node.address == address && node.port == port;
                           });

    if (it != nodes.end())
    {
        it->isActive = isActive;
    }
}

// 私有辅助方法实现
std::pair<std::string, uint16_t> LoadBalancer::getRoundRobinNode()
{
    // 找到下一个活跃节点
    size_t startIndex = currentIndex;
    do
    {
        currentIndex = (currentIndex + 1) % nodes.size();
        if (nodes[currentIndex].isActive)
        {
            return {nodes[currentIndex].address, nodes[currentIndex].port};
        }
    } while (currentIndex != startIndex);

    throw std::runtime_error("No active nodes available");
}

std::pair<std::string, uint16_t> LoadBalancer::getWeightedRoundRobinNode()
{
    uint32_t totalWeight = 0;
    for (const auto &node : nodes)
    {
        if (node.isActive)
        {
            totalWeight += node.weight;
        }
    }

    if (totalWeight == 0)
    {
        throw std::runtime_error("No active nodes available");
    }

    uint32_t point = rand() % totalWeight;
    uint32_t accumulator = 0;

    for (const auto &node : nodes)
    {
        if (!node.isActive)
            continue;
        accumulator += node.weight;
        if (accumulator > point)
        {
            return {node.address, node.port};
        }
    }

    // 防止意外情况，返回第一个活跃节点
    for (const auto &node : nodes)
    {
        if (node.isActive)
        {
            return {node.address, node.port};
        }
    }

    throw std::runtime_error("No active nodes available");
}

std::pair<std::string, uint16_t> LoadBalancer::getLeastConnectionsNode()
{
    const Node *selectedNode = nullptr;
    uint32_t minConnections = UINT32_MAX;

    for (const auto &node : nodes)
    {
        if (!node.isActive)
            continue;
        if (node.currentConnections < minConnections)
        {
            minConnections = node.currentConnections;
            selectedNode = &node;
        }
    }

    if (!selectedNode)
    {
        throw std::runtime_error("No active nodes available");
    }

    return {selectedNode->address, selectedNode->port};
}

std::pair<std::string, uint16_t> LoadBalancer::getRandomNode()
{
    std::vector<const Node *> activeNodes;
    for (const auto &node : nodes)
    {
        if (node.isActive)
        {
            activeNodes.push_back(&node);
        }
    }

    if (activeNodes.empty())
    {
        throw std::runtime_error("No active nodes available");
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, activeNodes.size() - 1);

    const Node *selectedNode = activeNodes[dis(gen)];
    return {selectedNode->address, selectedNode->port};
}
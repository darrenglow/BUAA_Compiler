//
// Created by 安达楷 on 2023/11/24.
//
#include "GraphNode.h"

void GraphNode::addNeighborNode(GraphNode *node) {
    neighborNodes.insert(node);
}

int GraphNode::getEdgeCount() {
    return neighborNodes.size();
}

RegType GraphNode::getRegister() {
    return reg;
}

// 得到这个节点相邻的节点所分配到的寄存器
std::set<RegType> GraphNode::getConflictRegisters() {
    auto set = std::set<RegType>();
    for (auto node : neighborNodes) {
        set.insert(node->getRegister());
    }
    return set;
}

void GraphNode::setRegister(RegType reg_) {
    reg = reg_;
}
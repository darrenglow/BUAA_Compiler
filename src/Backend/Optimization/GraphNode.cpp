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
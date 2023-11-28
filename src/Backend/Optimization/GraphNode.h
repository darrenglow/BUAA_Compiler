//
// Created by 安达楷 on 2023/11/24.
//

#ifndef BUAA_COMPILER_GRAPHNODE_H
#define BUAA_COMPILER_GRAPHNODE_H

#include "../../Middle/MiddleCodeItem/Middle.h"
#include "../Register/Register.h"
class GraphNode {
public:
    ValueSymbol *symbol;
    std::set<GraphNode*> neighborNodes;
    RegType reg;

    explicit GraphNode(ValueSymbol *symbol_) : symbol(symbol_) {}
    void addNeighborNode(GraphNode *node);
    int getEdgeCount();
    RegType getRegister();
    std::set<RegType> getConflictRegisters();
    void setRegister(RegType reg_);
};

#endif //BUAA_COMPILER_GRAPHNODE_H

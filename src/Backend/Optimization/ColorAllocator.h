//
// Created by 安达楷 on 2023/11/24.
//

#ifndef BUAA_COMPILER_COLORALLOC_H
#define BUAA_COMPILER_COLORALLOC_H

#include "../../Middle/MiddleCodeItem/Middle.h"
#include "../Register/Register.h"
#include "GraphNode.h"
#include <set>
#include <unordered_map>

class ColorAllocator {
public:
    Func *func;
    std::unordered_map<ValueSymbol*, GraphNode*> symbolToNode{};
    std::unordered_map<GraphNode*, std::set<GraphNode*>> G{};
    // 分配的寄存器
    std::set<RegType> registers = Register::globalRegisters;
    // 对应
    std::unordered_map<ValueSymbol*, RegType> symbolToRegister{};
    // 溢出的
    std::set<ValueSymbol*> spillWorklist;
    std::set<ValueSymbol*> freezeWorklist;
    std::set<ValueSymbol*> simplifyWorklist;

    void run();
    void buildGraph();
    GraphNode* getGraphNode(ValueSymbol *symbol);
    void addEdge(GraphNode *node1, GraphNode *node2);
    void printGraph();
    void doAllocate();
    void detachFromGraph(GraphNode* node);
    void makeWorklist();
    explicit ColorAllocator(Func *func_) : func(func_) {}
};

struct CompareGraphNodes {
    bool operator()(GraphNode* node1, GraphNode* node2) {
        return node1->getEdgeCount() < node2->getEdgeCount();
    }
};
#endif //BUAA_COMPILER_COLORALLOC_H

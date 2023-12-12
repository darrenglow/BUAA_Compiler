//
// Created by 安达楷 on 2023/11/24.
//

#ifndef BUAA_COMPILER_COLORALLOC_H
#define BUAA_COMPILER_COLORALLOC_H

#include "../Register/Register.h"
#include "GraphNode.h"
#include "../../Middle/MiddleCodeItem/MiddleCodeItem.h"
#include <set>
#include <unordered_map>

class Func;
class ColorAllocator {
public:
    Func *func;
    std::unordered_map<ValueSymbol*, GraphNode*> symbolToNode{};
    std::unordered_map<GraphNode*, std::set<GraphNode*>> G{};
    // 分配的寄存器
    std::set<RegType> globalRegisters = std::set<RegType>(Register::globalRegisters);
    // 对应
    std::unordered_map<ValueSymbol*, RegType> symbolToRegister{};
    // 溢出的节点
    std::set<ValueSymbol*> spillSet;
    std::unordered_map<ValueSymbol*, RegType> spillToRegister{};
    // 可分配的全局寄存器
    std::unordered_map<ValueSymbol*, RegType> symbolToGlobalRegister{};
    void run();
    void buildGraph();
    GraphNode* getGraphNode(ValueSymbol *symbol);
    void addEdge(GraphNode *node1, GraphNode *node2);
    void printGraph();
    void doAllocate();
    void detachFromGraph(GraphNode* node);
    explicit ColorAllocator(Func *func_) : func(func_) {}

    bool satisfyGlobalRegister(ValueSymbol *symbol);
    bool hasGlobalRegister(ValueSymbol* symbol);
    RegType allocGlobalRegister(ValueSymbol *symbol);
    void dealOverflow(ValueSymbol *symbol, RegType regType);
    void freeOverflow(ValueSymbol *symbol);
    bool hasOverflow(ValueSymbol *symbol);
    std::set<ValueSymbol*> *getActiveGlobalSymbols(MiddleCodeItem *code);

};

struct CompareGraphNodes {
    bool operator()(GraphNode* node1, GraphNode* node2) {
        return node1->getEdgeCount() < node2->getEdgeCount();
    }
};
#endif //BUAA_COMPILER_COLORALLOC_H

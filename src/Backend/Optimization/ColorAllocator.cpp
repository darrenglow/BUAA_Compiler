//
// Created by 安达楷 on 2023/11/24.
//
#include <queue>
#include "ColorAllocator.h"
#include "../../Middle/Optimization/DataFlow.h"


void ColorAllocator::run() {
    DataFlow::getInstance()._activeAnalysis(func);
    buildGraph();
    printGraph();
    makeWorklist();
}

// 每条代码都对应着一个冲突集合。
void ColorAllocator::buildGraph() {
    auto conflictGroups = new std::vector<std::set<ValueSymbol*>>();
    for (auto block : func->basicBlocks) {
        auto blockConflictGroups = block->calcCodeActive();
        for (const auto& group : *blockConflictGroups) {
            conflictGroups->push_back(group);
        }
    }
    for (const auto& conflictGroup : *conflictGroups) {
        for (auto symbol1 : conflictGroup) {
            auto node1 = getGraphNode(symbol1);
            for (auto symbol2 : conflictGroup) {
                auto node2 = getGraphNode(symbol2);
                addEdge(node1, node2);
            }
        }
    }
}

GraphNode *ColorAllocator::getGraphNode(ValueSymbol *symbol) {
    if (symbolToNode[symbol] != nullptr) {
        return symbolToNode[symbol];
    }
    else {
        auto node = new GraphNode(symbol);
        symbolToNode[symbol] = node;
        return node;
    }
}

void ColorAllocator::addEdge(GraphNode *node1, GraphNode *node2) {
    if (node1 != node2) {
        // G中node1的点不为空，且已经有了node1-node2的边
        if (!G[node1].empty() && G[node1].count(node2) != 0) {
            return;
        }
        G[node1].insert(node2);
        node1->addNeighborNode(node2);
        node2->addNeighborNode(node1);
    }
}

void ColorAllocator::printGraph() {
    std::cout << "Function " << func->funcName << " conflict graph" << std::endl;
    for (const auto& pair : G) {
        auto symbol = pair.first;
        auto set = pair.second;
        std::cout << symbol->symbol->name << ": ";
        for (auto node : set) {
            std::cout << node->symbol->name << " ";
        }
        std::cout << "\n";
    }
}

void ColorAllocator::doAllocate() {
    // 按照点的相连边的大小排序
//    auto heap = new std::priority_queue<GraphNode*, std::vector<GraphNode*>, CompareGraphNodes>();
//    for (auto pair : symbolToNode) {
//        heap->push(pair.second);
//    }
//
//    while (!heap->empty()) {
//        if (heap->top()->getEdgeCount() >= registers.size()) {
//            auto overflow = heap->top();
//            heap->pop();
//            overflows.insert(overflow->symbol);
//            detachFromGraph(overflow);
//            continue;
//        }
//        while (!heap->empty()) {
//            auto node = heap->top();
//            heap->pop();
//            // 获取可以分配的寄存器
//            auto availRegisters = new std::set<RegType>(registers);
//            // 去除冲突的变量被分配到的寄存器
//            auto conflictRegisters = node->getConflictRegisters();
//            for (auto reg : conflictRegisters){
//                availRegisters->erase(reg);
//            }
//            for (auto reg : *availRegisters) {
//                node->setRegister(reg);
//                symbolToRegister[node->symbol] = reg;
//                break;
//            }
//        }
//    }
}

void ColorAllocator::detachFromGraph(GraphNode *node) {
    for (auto neighborNode : node->neighborNodes) {
        neighborNode->neighborNodes.erase(node);
    }
    node->neighborNodes.clear();
}

void ColorAllocator::makeWorklist() {

}
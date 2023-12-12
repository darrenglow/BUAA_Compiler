//
// Created by 安达楷 on 2023/11/24.
//
#include <queue>
#include "ColorAllocator.h"
#include "../../Middle/Optimization/DataFlow.h"


void ColorAllocator::run() {
    DataFlow::getInstance()._activeAnalysis(func);
    std::cout << "0";
    buildGraph();
    std::cout << "1";
    printGraph();
    std::cout << "2";
    doAllocate();
    std::cout << "3";
}

// 每条代码都对应着一个冲突集合。
void ColorAllocator::buildGraph() {
    // 函数参数冲突
    auto params = func->funcSymbol->funcFParamSymbols;
    for (auto symbol1 : params) {
        auto node1 = getGraphNode(symbol1);
        for (auto symbol2 : params) {
            auto node2 = getGraphNode(symbol2);
            addEdge(node1, node2);
        }
    }

    auto conflictGroups = new std::vector<std::set<ValueSymbol*>>();
    for (auto block : func->basicBlocks) {
        auto blockConflictGroups = block->calcCodeActive();
        for (const auto& group : *blockConflictGroups) {
            conflictGroups->push_back(group);
        }
    }

    int pos = 0;
    for (const auto& conflictGroup : *conflictGroups) {
        for (auto symbol1 : conflictGroup) {
            if (symbol1->valueType == ValueType::TEMP || symbol1->name.rfind("ArraY_*$+|!123___", 0) == 0) {
                continue;
            }
            auto node1 = getGraphNode(symbol1);
            for (auto symbol2 : conflictGroup) {
                if (symbol2->valueType == ValueType::TEMP || symbol2->name.rfind("ArraY_*$+|!123___", 0) == 0) {
                    continue;
                }
                auto node2 = getGraphNode(symbol2);
                addEdge(node1, node2);
            }
        }
    }
    // 除去TEMP的变量
//    std::set<GraphNode*> tempSet;
//    for (auto pair : G) {
//        auto node = pair.first;
//        if (node->symbol->valueType == ValueType::TEMP) {
//            tempSet.insert(node);
//        }
//        if (node->symbol->name.rfind("ArraY_*$+|!123___", 0) == 0) {
//            tempSet.insert(node);
//        }
//    }
//    for (auto node : tempSet) {
//        detachFromGraph(node);
//    }
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
//     按照点的相连边的大小排序
    auto heap = new std::priority_queue<GraphNode*, std::vector<GraphNode*>, CompareGraphNodes>();
    for (auto pair : G) {
        auto node = pair.first;
        heap->push(node);
    }

    while (!heap->empty()) {
        // 如果有大于K个寄存器的节点，就将其移出
        if (heap->top()->getEdgeCount() >= globalRegisters.size()) {
            auto overflow = heap->top();
            heap->pop();
            spillSet.insert(overflow->symbol);
            // 在图中移去这个点
            detachFromGraph(overflow);
            continue;
        }
        while (!heap->empty()) {
            auto node = heap->top();
            heap->pop();
            // 获取可以分配的寄存器
            auto availRegisters = new std::set<RegType>(globalRegisters);
            // 去除冲突的变量被分配到的寄存器
            auto conflictRegisters = node->getConflictRegisters();
            for (auto reg : conflictRegisters){
                availRegisters->erase(reg);
            }
            // 剩下的availRegisters中分配一个。
            for (auto reg : *availRegisters) {
                node->setRegister(reg);
                symbolToRegister[node->symbol] = reg;
                break;
            }
        }
#ifdef DEBUG
        std::cout << "[ColorAllocator-doAllocate]FUNC: " << func->funcName << ":\n";
        for (auto pair : symbolToRegister) {
            std::cout << pair.first->name << " " << Register::type2str[pair.second] << "\n";
        }
        std::cout << "spill symbol\n";
        for (auto symbol : spillSet) {
            std::cout << symbol->name << "\n";
        }
#endif
    }
}

void ColorAllocator::detachFromGraph(GraphNode *node) {
    for (auto neighborNode : node->neighborNodes) {
        neighborNode->neighborNodes.erase(node);
        G[neighborNode].erase(node);
    }
    node->neighborNodes.clear();
    G.erase(node);
}

// 根据之前的寄存器图得到的symbolToRegister得到symbolToGlobalRegister此时寄存器的分配：图中分配的、溢出节点、未在图中的
// 由RegisterAlloc调用。
RegType ColorAllocator::allocGlobalRegister(ValueSymbol *symbol) {
    if (symbolToRegister.count(symbol) != 0 ) {
        auto reg = symbolToRegister[symbol];
        symbolToGlobalRegister[symbol] = reg;
        return reg;
    }
    else {
        // TODO: 这里可能有点问题
        std::cout << "Error in allocGlobalRegister " << symbol->name << std::endl;
        auto reg = *globalRegisters.begin();
        symbolToGlobalRegister[symbol] = reg;
        return reg;
    }
}


// 是局部变量且不为溢出的
// !!!排除临时变量和数组成员
bool ColorAllocator::satisfyGlobalRegister(ValueSymbol *symbol) {
    // !!! 排除数组成员和数组指针！！！
    if (symbol != nullptr && (symbol->name.rfind("ArraY_*$+|!123___", 0) == 0 || symbol->name.rfind("PointeR_*$+|!123___", 0) == 0)) {
        return false;
    }

    // 如果是参数数组的话，也是要分配全局寄存器的
//    if (symbol != nullptr && (symbol->name.rfind("ArraY_*$+|!123___", 0) == 0)) {
//        return false;
//    }
    if (symbol != nullptr && (symbol->isLocal || symbol->valueType==FUNCFPARAM) && symbol->valueType != TEMP && spillSet.count(symbol) == 0) {
        return true;
    }
    return false;
}

bool ColorAllocator::hasGlobalRegister(ValueSymbol *symbol) {
    if (symbolToGlobalRegister.count(symbol) != 0) {
        return true;
    }
    return false;
}

void ColorAllocator::dealOverflow(ValueSymbol *symbol, RegType regType) {
    this->spillToRegister[symbol] = regType;
}

void ColorAllocator::freeOverflow(ValueSymbol *symbol) {
    this->spillToRegister.erase(symbol);
}

bool ColorAllocator::hasOverflow(ValueSymbol *symbol) {
    if (this->spillToRegister.count(symbol) != 0) {
        return true;
    }
    return false;
}


// 得到这行代码后的所有代码的use
std::set<ValueSymbol *> *ColorAllocator::getActiveGlobalSymbols(MiddleCodeItem *code) {
    auto *symbols = new std::set<ValueSymbol*>;
//    std::cout << "[ColorAllocator-getActiveGlobalSymbols]:\n";
    std::set<ValueSymbol*> activeSymbols = std::set<ValueSymbol*>();
//    auto activeSymbols = *code->activeSymbols;

//    auto it = func->middleCodeItems.find(code);
//    for (; it != func->middleCodeItems.end(); it ++ ) {
//        auto x = *it;
//        auto uses = x->getUse();
//        for (auto use : *uses) {
//            activeSymbols.insert(use);
//        }
//    }

    // !!! 还要加上活跃变量分析的结果
    for (auto symbol : *code->activeSymbols) {
        activeSymbols.insert(symbol);
    }

#ifdef DEBUG
    if (code->codeType == MiddleCodeItem::MiddleFuncCall) {
        std::cout << "[getActiveGlobalSymbols]" << "\n";
        std::cout << *code << " " << code->index << std::endl;
        for (auto symbol : activeSymbols) {
            std::cout << symbol->name << "\n";
        }
        for (auto pair : spillToRegister) {
            auto s = pair.first;
            std::cout << "<spill>" << s->name << std::endl;
        }
    }
#endif



    for (auto symbol : activeSymbols) {
        if (symbolToGlobalRegister.count(symbol) != 0) {
            symbols->insert(symbol);
//            std::cout << dynamic_cast<ValueSymbol*>(symbol)->name << "\n";
        }
        // ！！！这里还要考虑symbolToSpillRegister
        if (spillToRegister.count(symbol) != 0) {
            symbols->insert(symbol);
//            std::cout << "<spill> " << dynamic_cast<ValueSymbol*>(symbol)->name << "\n";
        }
    }
    return symbols;
}
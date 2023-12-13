//
// Created by 安达楷 on 2023/11/19.
//

#ifndef BUAA_COMPILER_DATAFLOW_H
#define BUAA_COMPILER_DATAFLOW_H

#include <queue>
#include <unordered_map>

#include "../MiddleCodeItem/Middle.h"
#include "DefinitionSet.h"
#include "../../Backend/Optimization/ColorAllocator.h"
#include "../../Backend/MipsGenerator.h"

class DataFlow{
public:
    static DataFlow& getInstance() {
        static DataFlow instance;
        return instance;
    }
    std::vector<Func*> funcs;
    BasicBlock *root{};
    std::queue<BasicBlock*> q;  // 每个func时，bfs遍历所有的block
    std::unordered_map<BasicBlock*, int> hasVisited{};
    std::unordered_map<ValueSymbol*, DefinitionSet*> symbolDefs;
    std::unordered_map<int, MiddleCodeItem*> index2code;

    explicit DataFlow(std::vector<Func*> &funcs_) : funcs(funcs_) {

    }

    static void mergeJump(std::vector<BasicBlock*> &basicBlocks);
    // 建图
    void buildGraph();

    BasicBlock *getTargetFromCode(MiddleCodeItem *middleCodeItem);

    // 到达定义分析
    void reachDefinition();

    void _reachDefinition(Func *func);

    void _initSymbolDefs(Func *func);

    void _calcKill(Func *func);

    void _calcKillSetPerCode(MiddleCodeItem *middleCodeItem, BasicBlock *basicBlock);

    void _calcGen(Func *func);

    void _calcDefFlow(Func *func);

    // 活跃变量分析
    void activeAnalysis();

    void _activeAnalysis(Func *func);

    void _calcDefAndUse(Func *func);

    static void _calcActiveFlow(Func *func);

    // 死代码删除
    void deleteDeadCode();

    bool _deleteDeadCode(Func *func);

    // 基本块内常量传播
    void inBroadcast();

    void _inBroadcast(Func *func);

    void deleteUselessJump();

    void deleteUselessCall();

private:
    DataFlow()=default;
    ~DataFlow()=default;
};
#endif //BUAA_COMPILER_DATAFLOW_H

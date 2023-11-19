//
// Created by 安达楷 on 2023/11/19.
//

#ifndef BUAA_COMPILER_DATAFLOW_H
#define BUAA_COMPILER_DATAFLOW_H

#include <queue>
#include <unordered_map>

#include "../MiddleCodeItem.h"
class DataFlow{
public:
    std::vector<Func*> funcs;
    BasicBlock *root{};
    std::queue<BasicBlock*> q;  // 每个func时，bfs遍历所有的block
    std::unordered_map<BasicBlock*, int> hasVisited{};
    DataFlow()=default;
    explicit DataFlow(std::vector<Func*> &funcs_) : funcs(funcs_) {}

    static void mergeJump(std::vector<BasicBlock*> &basicBlocks);
    // 建图
    void buildGraph();

    BasicBlock *getTargetFromCode(MiddleCodeItem *middleCodeItem);

};
#endif //BUAA_COMPILER_DATAFLOW_H

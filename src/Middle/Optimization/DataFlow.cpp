//
// Created by 安达楷 on 2023/11/19.
//
#include "DataFlow.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include "../../Optimization/Config.h"

void DataFlow::mergeJump(std::vector<BasicBlock *> &basicBlocks) {
    // 有些block中存在两条连着的jump，只要有一个jump后面的语句都可以去掉了
    for (auto &basicBlock : basicBlocks) {
        for (int i = 0; i < basicBlock->middleCodeItems.size(); i++) {
            auto code = basicBlock->middleCodeItems[i];
            if (dynamic_cast<MiddleJump *>(code)) {
                if (dynamic_cast<MiddleJump *>(code)->type == MiddleJump::JUMP) {
                    // 删除后面的指令
                    for (int j = i + 1; j < basicBlock->middleCodeItems.size(); j++) {
                        basicBlock->middleCodeItems.erase(basicBlock->middleCodeItems.begin() + j);
                    }
                    break;
                }
            }
        }
    }
    // 删除基本块为空的
    for (int i = 0; i < basicBlocks.size(); i++) {
        if (basicBlocks[i]->middleCodeItems.size() == 0) {
            // 将跳转到这个空基本块的其他基本块的target设置为i+1
            for (int j = 0; j < basicBlocks.size(); j++) {
                if (!basicBlocks[j]->middleCodeItems.empty()) {
                    auto lastCode = basicBlocks[j]->middleCodeItems.back();
                    if (dynamic_cast<MiddleJump *>(lastCode)) {
                        auto target = dynamic_cast<MiddleJump *>(lastCode)->target;
                        if (target == basicBlocks[i]) {
                            dynamic_cast<MiddleJump *>(lastCode)->target = basicBlocks[i + 1];
                        }
                    }
                }
            }
            basicBlocks.erase(basicBlocks.begin() + i);
        }
    }

    // 跳转合并
    for (auto &basicBlock : basicBlocks) {
        auto lastCodeItem = basicBlock->middleCodeItems.back();
        if (dynamic_cast<MiddleJump *>(lastCodeItem)) {
            // 得到跳转到的基本块
            auto target = dynamic_cast<MiddleJump *>(lastCodeItem)->target;
//             如果跳转到的基本块的只有一条语句且为jump
            while (target->middleCodeItems.size() == 1) {
                auto code = target->middleCodeItems[0];
                if (dynamic_cast<MiddleJump *>(code) == nullptr) {
                    break;
                }
                auto finalTarget = dynamic_cast<MiddleJump *>(code)->target;
                target = finalTarget;
            }
            dynamic_cast<MiddleJump *>(lastCodeItem)->target = target;

            if (basicBlock->middleCodeItems.size() >= 2) {
                auto code2 = basicBlock->middleCodeItems[basicBlock->middleCodeItems.size() - 2];
                if (dynamic_cast<MiddleJump *>(code2) &&
                    dynamic_cast<MiddleJump *>(code2)->type == MiddleJump::JUMP_EQZ) {
                    dynamic_cast<MiddleJump *>(code2)->anotherTarget = target;
                }
            }
        }

        if (basicBlock->middleCodeItems.size() >= 2) {
            lastCodeItem = basicBlock->middleCodeItems[basicBlock->middleCodeItems.size() - 2];
            if (dynamic_cast<MiddleJump *>(lastCodeItem)) {
                // 得到跳转到的基本块
                auto target = dynamic_cast<MiddleJump *>(lastCodeItem)->target;

//             如果跳转到的基本块的只有一条语句且为jump
                while (target->middleCodeItems.size() == 1) {
                    auto code = target->middleCodeItems[0];
                    if (dynamic_cast<MiddleJump *>(code) == nullptr) {
                        break;
                    }
                    auto finalTarget = dynamic_cast<MiddleJump *>(code)->target;
                    target = finalTarget;
                }
                dynamic_cast<MiddleJump *>(lastCodeItem)->target = target;
            }
        }
    }

    // 删除block中jump_nez t1和jump t1连着的情况
    for (auto &basicBlock : basicBlocks) {
        for (int i = 0; i < basicBlock->middleCodeItems.size(); i++) {
            auto code = basicBlock->middleCodeItems[i];
            if (dynamic_cast<MiddleJump *>(code)) {
                if (dynamic_cast<MiddleJump *>(code)->type == MiddleJump::JUMP_EQZ) {
                    auto code2 = basicBlock->middleCodeItems[i + 1];
                    if (dynamic_cast<MiddleJump *>(code2)->type == MiddleJump::JUMP) {
                        std::cout << "ahahaahahaahahah@@@@@@@@@@@@@@@" << std::endl;
                        std::cout << dynamic_cast<MiddleJump *>(code)->target->label->label << std::endl;
                        std::cout << dynamic_cast<MiddleJump *>(code2)->target->label->label << std::endl;
                        if (dynamic_cast<MiddleJump *>(code)->target == dynamic_cast<MiddleJump *>(code2)->target) {
                            basicBlock->middleCodeItems.erase(basicBlock->middleCodeItems.begin() + i);
                        }
                    }
                }
            }
        }
    }
}

void DataFlow::buildGraph() {
    for (auto func : funcs) {
        std::cout << "\n" << func->funcName << ": \n";

        auto& basicBlocks = func->basicBlocks;
        while (!q.empty()) q.pop();

        // 先得到根block
        root = basicBlocks[0];
        func->root = root;
        q.push(root);
        hasVisited[root] = true;
        while (!q.empty()) {
            auto t = q.front();
            auto target1 = getTargetFromCode(t->middleCodeItems[t->middleCodeItems.size() - 1]);
            if (target1 != nullptr) {
                t->setNext(target1);
                if (!hasVisited[target1]) {
                    q.push(target1);
                    hasVisited[target1] = true;
                }
            }
            if (t->middleCodeItems.size() >= 2) {
                auto target2 = getTargetFromCode(t->middleCodeItems[t->middleCodeItems.size() - 2]);
                if (target2 != nullptr) {
                    t->setNext(target2);
                    if (!hasVisited[target2]){
                        q.push(target2);
                        hasVisited[target2] = true;
                    }
                }
            }
            q.pop();
        }
#ifdef UNTHROUGH_BASICBLOCK
        // 结束后清除无用基本块，此时已经有了前驱和后继，删除不会经过的基本块
        for (auto it = basicBlocks.begin(); it != basicBlocks.end();) {
            auto block = (*it);
            std::cout << block->basicBlockID << std::endl;
            if (block != root && block->prevs.empty()) {
                for (auto follow : block->follows) {
                    auto it2 = std::find(follow->prevs.begin(), follow->prevs.end(), block);
                    if (it2 != follow->prevs.end()) {
                        follow->prevs.erase(it2);
                    }
                }
                it = basicBlocks.erase(it);
            }
            else {
                it ++ ;
            }
        }
#endif
    }
}

BasicBlock *DataFlow::getTargetFromCode(MiddleCodeItem* middleCodeItem) {
    if (dynamic_cast<MiddleJump*>(middleCodeItem) == nullptr) {
        return nullptr;
    }
    auto target = dynamic_cast<MiddleJump*>(middleCodeItem)->target;
    return target;
}

//void DataFlow::printGraph(BasicBlock *x) {
//    while (!q.empty()) q.pop();
//    q.push(x);
//    while (!q.empty()) {
//        auto t = q.front();
//        std::cout << t->basicBlockID << " -> ";
//        for (auto next : t->follows) {
//            std:: cout << next->basicBlockID << " ";
//            q.push(next);
//        }
//        std::cout << std::endl;
//        q.pop();
//    }
//}
//
//void DataFlow::printAllGraph() {
//    for (auto func : funcs) {
//        std::cout << "\n" << func->funcName << ": \n";
//        printGraph(func->root);
//    }
//}
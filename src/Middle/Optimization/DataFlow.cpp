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


void DataFlow::reachDefinition() {
    for (auto func : funcs) {
        symbolDefs.clear();
        index2code.clear();

        _reachDefinition(func);
    }
}

void DataFlow::_reachDefinition(Func *func) {
    std::cout << func->funcName << " Start Reach Definition Analysis" << std::endl;
    _initSymbolDefs(func);

#ifdef DEBUG_REACH_DEFINITION
    // 打印symbolDefs
    for (auto pair : symbolDefs) {
        auto symbol = pair.first;
        auto defSet = pair.second;
        std::cout << symbol->printMiddleCode() << ": " << std::endl;
        for (int i = 0; i < 1000; i ++) {
            if (defSet->setIndex[i] > 0){
                std::cout << "--" << i << ": " << *index2code[i] << "\n";
            }
        }
    }
#endif
    _calcKill(func);
    _calcGen(func);

#ifdef DEBUG_REACH_DEFINITION
    for (auto block : func->basicBlocks) {
        std::cout << "Block[" << block->basicBlockID << "]: " << std::endl;
        std::cout << "KillSet: " << std::endl;
        for (auto code : block->killSet->defs) {
            std::cout << "--" << code->index << ": " << *code << std::endl;
        }
        std::cout << "GenSet: " << std::endl;
        for (auto code : block->genSet->defs) {
            std::cout << "--" << code->index << ": " << *code << std::endl;
        }
    }
#endif
}

// 遍历函数的所有中间代码，将<symbol, defs>对应起来
void DataFlow::_initSymbolDefs(Func *func) {
    int index = 0;
    for (auto& block : func->basicBlocks) {
        for (auto &code : block->middleCodeItems) {
            code->setIndex(index);

            if (dynamic_cast<MiddleDef*>(code) && dynamic_cast<MiddleDef*>(code)->type == MiddleDef::DEF_VAR) {
                auto symbol = dynamic_cast<MiddleDef*>(code)->valueSymbol;
                // 如果symbolDefs中没有symbol，就new一个definitionSet
                auto it = symbolDefs.find(dynamic_cast<ValueSymbol*>(symbol));
                if (it == symbolDefs.end()) {
                    symbolDefs[dynamic_cast<ValueSymbol*>(symbol)] = new DefinitionSet();
                }
                symbolDefs[symbol]->add(code, index);
                block->inDefSet->add(code, index);
            }
            else if (dynamic_cast<MiddleUnaryOp*>(code)) {
                auto symbol = dynamic_cast<MiddleUnaryOp*>(code)->valueSymbol;
                if (dynamic_cast<ValueSymbol*>(symbol)) {
                    // 如果symbolDefs中没有symbol，就new一个definitionSet
                    auto it = symbolDefs.find(dynamic_cast<ValueSymbol*>(symbol));
                    if (it == symbolDefs.end()) {
                        symbolDefs[dynamic_cast<ValueSymbol*>(symbol)] = new DefinitionSet();
                    }
                    symbolDefs[dynamic_cast<ValueSymbol*>(symbol)]->add(code, index);
                    block->inDefSet->add(code, index);
                }
                else {
                    std::cout << "ERROR in [_initSymbolDefs-MiddleUnaryOp] in Block " << block->basicBlockID << std::endl;
                }
            }
            else if (dynamic_cast<MiddleOffset*>(code)) {
                auto symbol = dynamic_cast<MiddleOffset*>(code)->ret;
                if (dynamic_cast<ValueSymbol*>(symbol)) {
                    // 如果symbolDefs中没有symbol，就new一个definitionSet
                    auto it = symbolDefs.find(dynamic_cast<ValueSymbol*>(symbol));
                    if (it == symbolDefs.end()) {
                        symbolDefs[dynamic_cast<ValueSymbol*>(symbol)] = new DefinitionSet();
                    }
                    symbolDefs[dynamic_cast<ValueSymbol*>(symbol)]->add(code, index);
                    block->inDefSet->add(code, index);
                }
                else {
                    std::cout << "ERROR in [_initSymbolDefs-MiddleOffset] in Block " << block->basicBlockID << std::endl;
                }
            }
            else if(dynamic_cast<MiddleMemoryOp*>(code)) {
                auto memoryCode = dynamic_cast<MiddleMemoryOp*>(code);
                auto symbol = memoryCode->type==MiddleMemoryOp::LOAD ? memoryCode->sym1 : memoryCode->sym2;
                if (dynamic_cast<ValueSymbol*>(symbol)) {
                    // 如果symbolDefs中没有symbol，就new一个definitionSet
                    auto it = symbolDefs.find(dynamic_cast<ValueSymbol*>(symbol));
                    if (it == symbolDefs.end()) {
                        symbolDefs[dynamic_cast<ValueSymbol*>(symbol)] = new DefinitionSet();
                    }
                    symbolDefs[dynamic_cast<ValueSymbol*>(symbol)]->add(code, index);
                    block->inDefSet->add(code, index);
                }
                else {
                    std::cout << "ERROR in [_initSymbolDefs-MiddleMemoryOp] in Block " << block->basicBlockID << std::endl;
                }
            }
            else if (dynamic_cast<MiddleBinaryOp*>(code)) {
                auto symbol = dynamic_cast<MiddleBinaryOp*>(code)->target;
                if (dynamic_cast<ValueSymbol*>(symbol)) {
                    // 如果symbolDefs中没有symbol，就new一个definitionSet
                    auto it = symbolDefs.find(dynamic_cast<ValueSymbol*>(symbol));
                    if (it == symbolDefs.end()) {
                        symbolDefs[dynamic_cast<ValueSymbol*>(symbol)] = new DefinitionSet();
                    }
                    symbolDefs[dynamic_cast<ValueSymbol*>(symbol)]->add(code, index);
                    block->inDefSet->add(code, index);
                }
                else {
                    std::cout << "ERROR in [_initSymbolDefs-MiddleBinaryOp] in Block " << block->basicBlockID << std::endl;
                }
            }
            else if (dynamic_cast<MiddleIO*>(code)) {
                if (dynamic_cast<MiddleIO*>(code)->type == MiddleIO::GETINT) {
                    auto symbol = dynamic_cast<MiddleIO*>(code)->target;
                    // 如果symbolDefs中没有symbol，就new一个definitionSet
                    if (dynamic_cast<ValueSymbol*>(symbol)) {
                        auto it = symbolDefs.find(dynamic_cast<ValueSymbol*>(symbol));
                        if (it == symbolDefs.end()) {
                            symbolDefs[dynamic_cast<ValueSymbol*>(symbol)] = new DefinitionSet();
                        }
                        symbolDefs[dynamic_cast<ValueSymbol*>(symbol)]->add(code, index);
                        block->inDefSet->add(code, index);
                    }
                    else {
                        std::cout << "ERROR in [_initSymbolDefs-MiddleIOOp] in Block " << block->basicBlockID << std::endl;
                    }
                }
            }
            index2code[index] = code;
            index ++ ;
        }
    }
}

// block的killSet就是所有代码的killSet的并集
void DataFlow::_calcKill(Func *func) {
    for (auto &block : func->basicBlocks) {
        for (auto code : block->inDefSet->defs) {
            _calcKillSetPerCode(code, block);
            // code->killSetIndex中存的是std::vector，其killSet
            // 所有code的killSet的并集就是block的killSet
            for (auto index : code->killSetIndex) {
                block->killSet->add(index2code[index], index);
            }
#ifdef DEBUG_REACH_DEFINITION
            std::cout << *code << ":" << std::endl;
            for (auto index : code->killSetIndex) {
                std::cout << "--" << index << ": " << *index2code[index] << std::endl;
            }
#endif
        }
    }
}

void DataFlow::_calcKillSetPerCode(MiddleCodeItem *middleCodeItem, BasicBlock *basicBlock) {
    auto symbol = middleCodeItem->getLeftIntermediate();
    auto totalDefSet = symbolDefs[dynamic_cast<ValueSymbol*>(symbol)];
    auto diffSet = totalDefSet->eraseNew(middleCodeItem);
    // 此时middleCodeItem中的killSet就是diffSet中不为0的下标
    for (int i = 0; i < 1000; i ++ ) {
        if (diffSet->setIndex[i] > 0)
            middleCodeItem->killSetIndex.push_back(i);
    }
}

void DataFlow::_calcGen(Func *func) {
    for (auto &block : func->basicBlocks) {
        auto tmpKillSet = new DefinitionSet();
        for (auto rit = block->inDefSet->defs.rbegin(); rit != block->inDefSet->defs.rend(); ++rit) {
            auto tmpGenset = new DefinitionSet();
            auto code = *rit;
            std::cout << "############ " << *code << "############" << std::endl;
            // gen[dn] + gen[dn-1] - kill[dn] + gen[dn-2] - kill[dn-1] - kill[dn]
            tmpGenset->add(code, code->index);
            tmpGenset->sub(tmpKillSet);
            for (auto index : code->killSetIndex) {
                tmpKillSet->add(index2code[index], index);
            }
            block->genSet->plus(tmpGenset);
        }
    }
}

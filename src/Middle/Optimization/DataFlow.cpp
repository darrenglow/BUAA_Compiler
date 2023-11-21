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
            auto nowTarget = target;
            while (target->middleCodeItems.size() == 1) {
                auto code = target->middleCodeItems[0];
                if (dynamic_cast<MiddleJump *>(code) == nullptr) {
                    break;
                }
                auto finalTarget = dynamic_cast<MiddleJump *>(code)->target;
                target = finalTarget;
                // 如果是环，就跳出。
                if (target == nowTarget) {
                    break;
                }
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
            if (t->middleCodeItems.empty()) {
                q.pop();
                continue;
            }
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
    _calcKill(func);
    _calcGen(func);
    _calcDefFlow(func);

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
        std::cout << "InFlow: " << std::endl;
        for (auto code : block->inDefFlow->defs) {
            std::cout << "--" << code->index << ": " << *code << std::endl;
        }
        std::cout << "OutFlow: " << std::endl;
        for (auto code : block->outDefFlow->defs) {
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
        }
    }
}

void DataFlow::_calcKillSetPerCode(MiddleCodeItem *middleCodeItem, BasicBlock *basicBlock) {
    auto symbol = middleCodeItem->getLeftIntermediate();
    if (symbol == nullptr) {
        return;
    }
    auto totalDefSet = symbolDefs[dynamic_cast<ValueSymbol*>(symbol)];
    auto diffSet = totalDefSet->eraseNew(middleCodeItem);
    // 此时middleCodeItem中的killSet就是diffSet中不为0的下标
    for (int i = 0; i < 100000; i ++ ) {
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

void DataFlow::_calcDefFlow(Func *func) {
    bool flag = true;
    while (flag) {
        flag = false;
        for (auto &block : func->basicBlocks) {
            for (auto &prev : block->prevs) {
                block->inDefFlow->plus(prev->outDefFlow);
            }
            auto prevOutFlow = new DefinitionSet(block->outDefFlow);

            block->outDefFlow->plus(block->genSet);
            auto tmpDefSet = new DefinitionSet();
            tmpDefSet = tmpDefSet->plus(block->inDefFlow);
            tmpDefSet = tmpDefSet->sub(block->killSet);
            block->outDefFlow->plus(tmpDefSet);

            if (!block->outDefFlow->equalTo(prevOutFlow)) {
                flag = true;
            }
        }
    }
}

void DataFlow::positiveAnalysis() {
    for (auto func : funcs) {
        _positiveAnalysis(func);
    }
}

void DataFlow::_positiveAnalysis(Func *func) {
    _calcDefAndUse(func);
    _calcPositiveFlow(func);
#ifdef DEBUG_POSITIVE_ANALYSIS
    std::cout << "Start Func " << func->funcName << ": " << "\n";
    for (auto block : func->basicBlocks) {
        std::cout << "Block [" << block->basicBlockID << "]" << ":\n";
        std::cout << "UseSet: ";
        for (auto valueSymbol : block->useSet->symbols) {
            std::cout << valueSymbol->name << " ";
        }
        std::cout << "\n";
        std::cout << "DefSet: ";
        for (auto valueSymbol : block->defSet->symbols) {
            std::cout << valueSymbol->name << " ";
        }
        std::cout << "\n";
        std::cout << "inPositiveFlow: ";
        for (auto valueSymbol : block->inPosFlow->symbols) {
            std::cout << valueSymbol->name << " ";
        }
        std::cout << "\n";
        std::cout << "outPostiveFlow: ";
        for (auto valueSymbol : block->outPosFlow->symbols) {
            std::cout << valueSymbol->name << " ";
        }
        std::cout << "\n";
    }
#endif
}

void DataFlow::_calcDefAndUse(Func *func) {
    for (auto block : func->basicBlocks) {
        for (auto it = block->middleCodeItems.begin(); it != block->middleCodeItems.end();) {
            auto code = *it;
            auto leftSymbol = code->getLeftIntermediate();

            if (dynamic_cast<ValueSymbol*>(leftSymbol) && dynamic_cast<ValueSymbol*>(leftSymbol)->valueType != ARRAY && isLegalPrefix(
                    dynamic_cast<ValueSymbol*>(leftSymbol)->name)) {
                auto name = dynamic_cast<ValueSymbol*>(leftSymbol)->name;
                if (!block->useSet->contain(dynamic_cast<ValueSymbol*>(leftSymbol))) {
                    block->defSet->add(dynamic_cast<ValueSymbol*>(leftSymbol));
                }
            }

            auto rightSymbol1 = code->getRightIntermediate1();
            if (dynamic_cast<ValueSymbol*>(rightSymbol1) && dynamic_cast<ValueSymbol*>(rightSymbol1)->valueType != ARRAY && isLegalPrefix(
                    dynamic_cast<ValueSymbol*>(rightSymbol1)->name)) {
                if (!block->defSet->contain(dynamic_cast<ValueSymbol*>(rightSymbol1))) {
                    block->useSet->add(dynamic_cast<ValueSymbol*>(rightSymbol1));
                }
            }
            auto rightSymbol2 = code->getRightIntermediate2();
            if (dynamic_cast<ValueSymbol*>(rightSymbol2) && dynamic_cast<ValueSymbol*>(rightSymbol2)->valueType != ARRAY && isLegalPrefix(
                    dynamic_cast<ValueSymbol*>(rightSymbol2)->name)) {
                if (!block->defSet->contain(dynamic_cast<ValueSymbol*>(rightSymbol2))) {
                    block->useSet->add(dynamic_cast<ValueSymbol*>(rightSymbol2));
                }
            }
            it ++ ;
        }
    }
}

void DataFlow::_calcPositiveFlow(Func *func) {
    bool flag = true;
    while (flag) {
        flag = false;
        for (auto it = func->basicBlocks.rbegin(); it != func->basicBlocks.rend();) {
            auto block = *it;
            for (auto &follow : block->follows) {
                block->outPosFlow->plus(follow->inPosFlow);
            }
            auto prevInFlow = new PositiveSet(block->inPosFlow);

            block->inPosFlow->plus(block->useSet);
            auto tmpPosSet = new PositiveSet();
            tmpPosSet->plus(block->outPosFlow);
            tmpPosSet->sub(block->defSet);
            block->inPosFlow->plus(tmpPosSet);

            if (!block->inPosFlow->equalTo(prevInFlow)) {
                flag = true;
            }
            it ++ ;
        }
    }
}

void DataFlow::deleteDeadCode() {
    for (auto func : funcs) {
        _deleteDeadCode(func);
    }
}

void DataFlow::_deleteDeadCode(Func *func) {
    for (auto block : func->basicBlocks) {
        auto s = new PositiveSet(block->outPosFlow);
        for (auto it = block->middleCodeItems.rbegin(); it != block->middleCodeItems.rend();) {
            auto code = *it;
            // 如果有左值def
            auto symbol = code->getLeftIntermediate();
            if (dynamic_cast<ValueSymbol*>(symbol)) {
                auto leftValueSymbol = dynamic_cast<ValueSymbol*>(symbol);
                // 如果def在set中
                if (s->contain(leftValueSymbol)) {
                    s->remove(leftValueSymbol);

                    auto rightValueSymbol1 = dynamic_cast<ValueSymbol*>(code->getRightIntermediate1());
                    if (rightValueSymbol1 && rightValueSymbol1->valueType != ARRAY &&
                            isLegalPrefix(rightValueSymbol1->name)) {
                        s->add(rightValueSymbol1);
                    }
                    auto rightValueSymbol2 = dynamic_cast<ValueSymbol*>(code->getRightIntermediate2());
                    if (rightValueSymbol2 && rightValueSymbol2->valueType != ARRAY &&
                            isLegalPrefix(rightValueSymbol2->name)) {
                        s->add(rightValueSymbol2);
                    }
                }
                // 如果是全局变量的话需要加入use
                else if (!leftValueSymbol->isLocal) {
                    auto rightValueSymbol1 = dynamic_cast<ValueSymbol*>(code->getRightIntermediate1());
                    if (rightValueSymbol1 && rightValueSymbol1->valueType != ARRAY &&
                            isLegalPrefix(rightValueSymbol1->name)) {
                        s->add(rightValueSymbol1);
                    }

                    auto rightValueSymbol2 = dynamic_cast<ValueSymbol*>(code->getRightIntermediate2());
                    if (rightValueSymbol2 && rightValueSymbol2->valueType != ARRAY &&
                            isLegalPrefix(rightValueSymbol2->name)) {
                        s->add(rightValueSymbol2);
                    }
                }
                // 如果是数组成员或是指针的话，也需要加入use
                else if (leftValueSymbol->name.rfind("ArraY_*|!123___", 0) == 0 || leftValueSymbol->name.rfind("PointeR_*|!123___", 0) == 0) {
                    auto rightValueSymbol1 = dynamic_cast<ValueSymbol*>(code->getRightIntermediate1());
                    if (rightValueSymbol1 && rightValueSymbol1->valueType != ARRAY &&
                            isLegalPrefix(rightValueSymbol1->name)) {
                        s->add(rightValueSymbol1);
                    }

                    auto rightValueSymbol2 = dynamic_cast<ValueSymbol*>(code->getRightIntermediate2());
                    if (rightValueSymbol2 && rightValueSymbol2->valueType != ARRAY &&
                            isLegalPrefix(rightValueSymbol2->name)) {
                        s->add(rightValueSymbol2);
                    }
                }
                // 如果不在set中，删除这个code
                else {
                    if (dynamic_cast<MiddleIO*>(code) && dynamic_cast<MiddleIO*>(code)->type == MiddleIO::GETINT) {
                        dynamic_cast<MiddleIO*>(code)->target = nullptr;
                        it ++ ;
                        continue;
                    }
                    // !!! 草，本来还想优化到让部分数组不store，但是有问题，比如a[i],不知道i的值。所以对于数组的定义，还是得全部加载。
//                    if (dynamic_cast<MiddleMemoryOp*>(code) && dynamic_cast<MiddleMemoryOp*>(code)->type == MiddleMemoryOp::STORE) {
//                        it ++ ;
//                        continue;
//                    }
//                    if (dynamic_cast<MiddleOffset*>(code)) {
//                        auto beforeIt = it - 1;
//                        auto nextCode = *beforeIt;
//                        if (dynamic_cast<MiddleMemoryOp*>(nextCode) && dynamic_cast<MiddleMemoryOp*>(nextCode)->type == MiddleMemoryOp::STORE) {
//                            it ++ ;
//                            continue;
//                        }
//                    }
                    block->middleCodeItems.erase(std::next(it).base());
                    // !!! 草，这个erase我以为会增加it的位置，调试才发现，他甚至还往回跑了！！！气死我了。。。这块的代码有点屎。
                    it ++ ;
                    continue;
                }
            }
            // 如果没有左值def
            else if (symbol == nullptr) {
                auto rightValueSymbol1 = dynamic_cast<ValueSymbol*>(code->getRightIntermediate1());
                if (rightValueSymbol1 && isLegalPrefix(rightValueSymbol1->name) && rightValueSymbol1->valueType != ARRAY) {
                    std::cout << "no left value, right value is : " << rightValueSymbol1->name << std::endl;
                    s->add(rightValueSymbol1);
                }
                auto rightValueSymbol2 = dynamic_cast<ValueSymbol*>(code->getRightIntermediate2());
                if (rightValueSymbol2 && isLegalPrefix(rightValueSymbol2->name) && rightValueSymbol2->valueType != ARRAY) {
                    std::cout << "no left value, right value is : " << rightValueSymbol2->name << std::endl;
                    s->add(rightValueSymbol2);
                }
            }
#ifdef DEBUG_DEADCODE
            std::cout << "Now code is : " << *(*it) << std::endl;
            std::cout << "{ ";
            for (auto valueSymbol : s->symbols) {
                std::cout << valueSymbol->name << " ";
            }
            std::cout << " }\n";
#endif
            it ++ ;
        }
    }
}

bool DataFlow::isLegalPrefix(std::string &name) {
    return name.rfind("ArraY_*|!123___", 0) != 0 && name.rfind("PointeR_*|!123___", 0) != 0;
}

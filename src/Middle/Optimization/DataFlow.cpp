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
        for (auto it = basicBlock->middleCodeItems.begin(); it != basicBlock->middleCodeItems.end();) {
            auto code = *it;
            if (dynamic_cast<MiddleJump *>(code)) {
                if (dynamic_cast<MiddleJump *>(code)->type == MiddleJump::JUMP) {
                    // 删除后面的指令
                    auto nextIt = std::next(it);
                    basicBlock->middleCodeItems.erase(nextIt, basicBlock->middleCodeItems.end());
                    break;
                }
            }
            ++it;
        }
    }
    // 删除基本块为空的
    for (int i = 0; i < basicBlocks.size(); i++) {
        if (basicBlocks[i]->middleCodeItems.empty()) {
            // 将跳转到这个空基本块的其他基本块的target设置为i+1
            for (auto & basicBlock : basicBlocks) {
                if (!basicBlock->middleCodeItems.empty()) {
                    auto lastCode = *basicBlock->middleCodeItems.rbegin();
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
        auto lastCodeItem = *basicBlock->middleCodeItems.rbegin();
        if (dynamic_cast<MiddleJump *>(lastCodeItem)) {
            // 得到跳转到的基本块
            auto target = dynamic_cast<MiddleJump *>(lastCodeItem)->target;
//             如果跳转到的基本块的只有一条语句且为jump
            auto nowTarget = target;
            while (target->middleCodeItems.size() == 1) {
                auto code = *target->middleCodeItems.begin();
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
                auto it = basicBlock->middleCodeItems.rbegin();
                ++ it;
                auto code2 = *it;
                if (dynamic_cast<MiddleJump *>(code2) != nullptr&&
                    dynamic_cast<MiddleJump *>(code2)->type == MiddleJump::JUMP_EQZ) {
                    dynamic_cast<MiddleJump *>(code2)->anotherTarget = target;
                }
            }
        }

        if (basicBlock->middleCodeItems.size() >= 2) {
            auto it = basicBlock->middleCodeItems.rbegin();
            ++ it;
            lastCodeItem = *it;
//            lastCodeItem = basicBlock->middleCodeItems[basicBlock->middleCodeItems.size() - 2];
            if (dynamic_cast<MiddleJump *>(lastCodeItem)) {
                // 得到跳转到的基本块
                auto target = dynamic_cast<MiddleJump *>(lastCodeItem)->target;

//             如果跳转到的基本块的只有一条语句且为jump
                while (target->middleCodeItems.size() == 1) {
                    auto code = *target->middleCodeItems.begin();
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
        for (auto it = basicBlock->middleCodeItems.begin(); it != basicBlock->middleCodeItems.end();) {
            auto code = *it;
            if (dynamic_cast<MiddleJump *>(code)) {
                if (dynamic_cast<MiddleJump *>(code)->type == MiddleJump::JUMP_EQZ) {
                    it ++ ;
                    if (it == basicBlock->middleCodeItems.end())
                        break;
                    auto code2 = *it;
                    it -- ;
                    if (dynamic_cast<MiddleJump *>(code2)->type == MiddleJump::JUMP) {
                        if (dynamic_cast<MiddleJump *>(code)->target == dynamic_cast<MiddleJump *>(code2)->target) {
                            it = basicBlock->middleCodeItems.erase(it);
                            continue;
                        }
                    }
                }
            }
            it ++ ;
        }
    }

    // 删除后下标需要重排
    for (auto bb : basicBlocks) {
        bb->resetIndex();
    }
}

void DataFlow::buildGraph() {
    // init
    hasVisited.clear();
    for (auto func : funcs) {
        for (auto block : func->basicBlocks) {
            block->prevs.clear();
            block->follows.clear();
        }
    }
    // 开始建图
    for (auto func : funcs) {
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
            std::cout << t->basicBlockID << ": #########" << std::endl;
            auto it = t->middleCodeItems.rbegin();
            auto target1 = getTargetFromCode(*it);
            if (target1 != nullptr) {
                t->setNext(target1);
                std::cout << target1->basicBlockID << std::endl;
                if (!hasVisited[target1]) {
                    q.push(target1);
                    hasVisited[target1] = true;
                }
            }
            if (t->middleCodeItems.size() >= 2) {
                auto it2 = t->middleCodeItems.rbegin();
                it2 ++ ;
                auto target2 = getTargetFromCode(*it2);
//                auto target2 = getTargetFromCode(t->middleCodeItems[t->middleCodeItems.size() - 2]);
                if (target2 != nullptr) {
                    std::cout << target2->basicBlockID << std::endl;
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
    for (auto& block : func->basicBlocks) {
        for (auto &code : block->middleCodeItems) {

            if (dynamic_cast<MiddleDef*>(code) && dynamic_cast<MiddleDef*>(code)->type == MiddleDef::DEF_VAR) {
                auto symbol = dynamic_cast<MiddleDef*>(code)->valueSymbol;
                // 如果symbolDefs中没有symbol，就new一个definitionSet
                auto it = symbolDefs.find(dynamic_cast<ValueSymbol*>(symbol));
                if (it == symbolDefs.end()) {
                    symbolDefs[dynamic_cast<ValueSymbol*>(symbol)] = new DefinitionSet();
                }
                symbolDefs[dynamic_cast<ValueSymbol*>(symbol)]->add(code, code->index);
                block->inDefSet->add(code, code->index);
            }
            else if (dynamic_cast<MiddleUnaryOp*>(code)) {
                auto symbol = dynamic_cast<MiddleUnaryOp*>(code)->valueSymbol;
                if (dynamic_cast<ValueSymbol*>(symbol)) {
                    // 如果symbolDefs中没有symbol，就new一个definitionSet
                    auto it = symbolDefs.find(dynamic_cast<ValueSymbol*>(symbol));
                    if (it == symbolDefs.end()) {
                        symbolDefs[dynamic_cast<ValueSymbol*>(symbol)] = new DefinitionSet();
                    }
                    symbolDefs[dynamic_cast<ValueSymbol*>(symbol)]->add(code, code->index);
                    block->inDefSet->add(code, code->index);
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
                    symbolDefs[dynamic_cast<ValueSymbol*>(symbol)]->add(code, code->index);
                    block->inDefSet->add(code, code->index);
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
                    symbolDefs[dynamic_cast<ValueSymbol*>(symbol)]->add(code, code->index);
                    block->inDefSet->add(code, code->index);
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
                    symbolDefs[dynamic_cast<ValueSymbol*>(symbol)]->add(code, code->index);
                    block->inDefSet->add(code, code->index);
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
                        symbolDefs[dynamic_cast<ValueSymbol*>(symbol)]->add(code, code->index);
                        block->inDefSet->add(code, code->index);
                    }
                    else {
                        std::cout << "ERROR in [_initSymbolDefs-MiddleIOOp] in Block " << block->basicBlockID << std::endl;
                    }
                }
            }
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
    auto symbol = middleCodeItem->getGen();
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


// 活跃变量分析
void DataFlow::activeAnalysis() {
    for (auto func : funcs) {
        _activeAnalysis(func);
    }
}

void DataFlow::_activeAnalysis(Func *func) {
    _calcDefAndUse(func);
    _calcActiveFlow(func);
#ifdef DEBUG_POSITIVE_ANALYSIS
    std::cout << "Start Func " << func->funcName << ": " << "\n";
    for (auto block : func->basicBlocks) {
        std::cout << "Block [" << block->basicBlockID << "]" << ":\n";
        std::cout << "UseSet: ";
        for (auto valueSymbol : *block->useSet) {
            std::cout << valueSymbol->name << " ";
        }
        std::cout << "\n";
        std::cout << "DefSet: ";
        for (auto valueSymbol : *block->defSet) {
            std::cout << valueSymbol->name << " ";
        }
        std::cout << "\n";
        std::cout << "inPositiveFlow: ";
        for (auto valueSymbol : *block->inPosFlow) {
            std::cout << valueSymbol->name << " ";
        }
        std::cout << "\n";
        std::cout << "outPostiveFlow: ";
        for (auto valueSymbol : *block->outPosFlow) {
            std::cout << valueSymbol->name << " ";
        }
        std::cout << "\n";
    }
#endif
}

void DataFlow::_calcDefAndUse(Func *func) {
    for (auto block : func->basicBlocks) {
        block->calcDefAndUse();
    }
}

void DataFlow::_calcActiveFlow(Func *func) {
    bool flag = true;
    while (flag) {
        flag = false;
        for (auto it = func->basicBlocks.rbegin(); it != func->basicBlocks.rend(); it ++ ) {
            auto block = *it;
            auto out = new std::set<ValueSymbol*>();
            for (auto &follow : block->follows) {
                out->insert(follow->inPosFlow->begin(), follow->inPosFlow->end());
            }

            auto in = new std::set<ValueSymbol*>();
            in->insert(out->begin(), out->end());
            for (auto symbol : *block->defSet) {
                in->erase(symbol);
            }
            for (auto symbol : *block->useSet) {
                in->insert(symbol);
            }
            if (*block->inPosFlow != *in || *block->outPosFlow != *out) {
                block->inPosFlow = in;
                block->outPosFlow = out;
                flag = true;
            }
        }
    }
}

// 死代码删除
void DataFlow::deleteDeadCode() {
    for (auto func : funcs) {
        bool change = _deleteDeadCode(func);
        while (change) {
            mergeJump(func->basicBlocks);
            func->resetCode();
            buildGraph();
            _activeAnalysis(func);
            change = _deleteDeadCode(func);
        }
    }
}

bool DataFlow::_deleteDeadCode(Func *func) {
    auto codeSet = new std::set<MiddleCodeItem*>();
    for (auto block : func->basicBlocks) {
        for (auto code : block->middleCodeItems) {
            codeSet->insert(code);
        }
    }

    auto nowCodeSet = new std::set<MiddleCodeItem*>();
    for (auto block : func->basicBlocks) {
        block->deleteDeadCode();
        for (auto code : block->middleCodeItems) {
            nowCodeSet->insert(code);
        }
    }

    // 如果不等,说明发生了删除
    return *codeSet != *nowCodeSet;
}


// ------------------基本块内常量传播-------------------------------
// 块内的传播的话，_inBlockcast(func)得到的就会是全部常量完成的，然后进行死代码删除。
void DataFlow::inBroadcast() {
    for (auto func : funcs) {
        _activeAnalysis(func);
        _deleteDeadCode(func);
        _inBroadcast(func);
        _deleteDeadCode(func);
    }
}

// 得到的就是当前基本块的常量传播结果了
void DataFlow::_inBroadcast(Func *func) {
    for (auto block : func->basicBlocks) {
        block->inBroadcast();
    }
}

void DataFlow::deleteUselessJump() {
    for (auto func : funcs) {
        for (int i = 0; i < func->basicBlocks.size(); i ++ ) {
            auto block = func->basicBlocks[i];
            auto nextBlock = func->basicBlocks[i + 1];

            auto it = block->middleCodeItems.rbegin();
            auto code = *it;
            if (dynamic_cast<MiddleJump*>(code) != nullptr && dynamic_cast<MiddleJump*>(code)->target == nextBlock) {
                if (nextBlock->prevs.size() == 1) {
                    block->middleCodeItems.erase(*it);
                }
            }
        }
    }
}

void DataFlow::deleteUselessCall() {
    for (auto func : funcs) {
        for (auto block : func->basicBlocks) {
            auto toEraseSet = new std::set<MiddleCodeItem*>();
            for (auto code : block->middleCodeItems) {
                if (dynamic_cast<MiddleFuncCall*>(code) != nullptr) {
                    auto funcName = dynamic_cast<MiddleFuncCall*>(code)->funcName;
                    // 得到调用的函数
                    Func *callFunc = nullptr;
                    for (auto f : funcs) {
                        if (f->funcName == funcName) {
                            callFunc = f;
                        }
                    }
                    if (callFunc == nullptr) {
                        continue;
                    }
                    bool notHasReturn = true;
                    bool notChangeGlobal = true;
                    bool notIO = true;
                    bool notCall = true;
                    bool notAddress = true;

                    if (callFunc->hasReturn) {
                        notHasReturn = false;
                    }
                    for (auto x : callFunc->middleCodeItems) {
                        auto def = x->getDef();
                        if (def != nullptr && !def->isLocal) {
                            notChangeGlobal = false;
                        }
                    }
                    for (auto x : callFunc->middleCodeItems) {
                        if (dynamic_cast<MiddleIO*>(x) != nullptr) {
                            notIO = false;
                        }
                        if (dynamic_cast<MiddleFuncCall*>(x) != nullptr) {
                            notCall = false;
                        }
                    }
                    for (auto param : dynamic_cast<MiddleFuncCall*>(code)->funcRParams) {
                        if (dynamic_cast<ValueSymbol*>(param) && dynamic_cast<ValueSymbol*>(param)->name.rfind("ArraY_*$+|!123___", 0) == 0) {
                            notAddress = false;
                        }
                    }
                    if (notHasReturn && notChangeGlobal && notIO && notCall && notAddress) {
                        toEraseSet->insert(code);
                    }
                }
            }
            for (auto x : *toEraseSet) {
                block->middleCodeItems.erase(x);
            }
        }
        func->resetCode();
    }
}
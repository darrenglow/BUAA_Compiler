//
// Created by 安达楷 on 2023/11/19.
//
#include "Middle.h"
#include "BasicBlock.h"
#include <set>
#include "../../Optimization/Config.h"
#include <unordered_map>

// BasicBlock
int BasicBlock::blockID = 0;


std::ostream & BasicBlock::output(std::ostream &os) const {
    os << "### " << type2str[type] << " [" << std::dec << basicBlockID << "] BEGIN" << std::endl;
    os << *label << std::endl;
    for (auto middleCodeItem : middleCodeItems) {
        os << *middleCodeItem << std::endl;
    }
    os << "### " << type2str[type] << " [" << std::dec << basicBlockID << "] END" << std::endl;
    return os;
}
Label * BasicBlock::getLabel() {
    return label;
}

void BasicBlock::setNext(BasicBlock *next) {
    follows.push_back(next);
    next->prevs.push_back(this);
}

void BasicBlock::calcDefAndUse() {
    this->defSet->clear();
    this->useSet->clear();
    for (auto code : this->middleCodeItems) {
        auto uses = code->getUse();
        if (uses != nullptr) {
            for (auto use : *uses) {
                if (use != nullptr) {
//                    if (!this->defSet->count(use)) {
//                        useSet->insert(use);
//                    }
                    useSet->insert(use);
                }
            }
        }
        auto def = code->getDef();
        if (def != nullptr) {
            auto name = def->name;
//            if (!this->useSet->count(def)) {
//                defSet->insert(def);
//            }
            defSet->insert(def);
        }
    }
}

void BasicBlock::add(int *codeIndex, MiddleCodeItem *middleCodeItem) {
    middleCodeItem->setIndex(*codeIndex);
    *codeIndex = *codeIndex + 1;
    middleCodeItems.insert(middleCodeItem);
}

void BasicBlock::deleteDeadCode() {
    auto s = new std::set<ValueSymbol*>(outPosFlow->begin(), outPosFlow->end());
    for (auto it = middleCodeItems.rbegin(); it != middleCodeItems.rend();) {
        auto code = *it;

#ifdef DEBUG_DEADCODE
        std::cout << "Now code is : " << *(*it) << std::endl;
            std::cout << "{ ";
            for (auto valueSymbol : *s) {
                std::cout << valueSymbol->name << " ";
            }
            std::cout << " }\n";
#endif
        // 如果有左值def
        auto def = code->getDef();
        if (def != nullptr) {
            // 如果def在set中
            if (s->count(def)) {

                s->erase(def);
                auto uses = code->getUse();
                for (auto use : *uses) {
                    s->insert(use);
                }
                it ++ ;
                continue;
            }
                // 如果是全局变量的话需要加入use
            else if (!def->isLocal) {
                auto uses = code->getUse();
                for (auto use : *uses) {
                    s->insert(use);
                }
                it ++ ;
                continue;
            }
                // 如果不在set中，删除这个code
            else {
                if (dynamic_cast<class MiddleIO*>(code) != nullptr && dynamic_cast<class MiddleIO*>(code)->type == MiddleIO::GETINT) {
                    auto target = dynamic_cast<class MiddleIO*>(code)->target;
                    if (dynamic_cast<ValueSymbol*>(target) != nullptr && dynamic_cast<ValueSymbol*>(target)->name.rfind("ArraY_*$+|!123___", 0) == 0) {
                        it ++ ;
                        continue;
                    }
                    dynamic_cast<class MiddleIO*>(code)->target = nullptr;
                    it ++ ;
                    continue;
                }
                // 如果碰到OFFSET, STORE的组合，不能删，且田家庵OFFSET的use
                if (code->codeType == MiddleOffset) {
                    auto uses = code->getUse();
                    for (auto use : *uses) {
                        s->insert(use);
                    }
                    it ++ ;
                    continue;
                }

                middleCodeItems.erase(code);
                // !!! 草，这个erase我以为会增加it的位置，调试才发现，他甚至还往回跑了！！！气死我了。。。这块的代码有点屎。
                continue;
            }
        }
        // 如果没有左值def
        else {
            auto uses = code->getUse();
            for (auto use : *uses) {
                s->insert(use);
            }
            it ++ ;
            continue;
        }
    }
    resetIndex();
}

void BasicBlock::inBroadcast() {
    bool change = true;
    std::vector<MiddleCodeItem*> defs;
    // 在迭代时记录，循环结束后更新。
    auto symbolToValueList = new std::vector<std::pair<std::pair<MiddleCodeItem*, ValueSymbol*>, int>>();
    while (change) {
        symbolToValueList->clear();
        for (auto code: middleCodeItems) {
            auto uses = code->getUse();
            for (auto symbol: *uses) {
                // 如果是全局变量的话，就跳过，不能直接赋值，可能中途会改变
                if (!symbol->isLocal) {
                    continue;
                }
                // 接着倒着遍历defs，看是否有use的symbol的对应def
                MiddleCodeItem *p = nullptr;
                bool flag = false;
                for (auto it = defs.rbegin(); it != defs.rend(); it ++ ) {
                    p = *it;
                    if (p->getDef() != nullptr && p->getDef() == symbol) {
                        flag = true;
                        break;
                    }
                }
                if (flag) {
                    // 如果这个def代码的值是常数赋值
                    if ((p->codeType == MiddleCodeItem::MiddleDef) &&
                        p->_getSrc1() != nullptr && dynamic_cast<Immediate *>(p->_getSrc1()) != nullptr) {
                        code->reset(symbol, p->_getSrc1());
                        // 特判一下code的类型，如果是pushparam的话，那就需要加入symboltoValue中。
                        if (dynamic_cast<class PushParam*>(code) != nullptr) {
                            symbolToValueList->push_back({{code, symbol}, dynamic_cast<Immediate*>(p->_getSrc1())->value});
                        }
                        change = true;
                    }
                    // 如果这个def的代码定义的是变量的话, int a = b；需要从这个代码向后看，看b是否被重新定义了。
                    else if (p->codeType == MiddleCodeItem::MiddleDef && p->_getSrc1() != nullptr
                            && dynamic_cast<ValueSymbol*>(p->_getSrc1()) != nullptr && dynamic_cast<ValueSymbol*>(p->_getSrc1())->isLocal && symbol != p->_getSrc1()) {
                        bool flag2 = true;
                        auto it = middleCodeItems.find(p);
                        it ++ ;
                        for (; it != middleCodeItems.end(); it ++ ) {
                            auto q = *it;
                            if (q->getDef() != nullptr && q->getDef() == p->_getSrc1()) {
                                flag2 = false;
                                break;
                            }
                        }
                        if (flag2) {
                            code->reset(symbol, p->_getSrc1());
                            // 特判一下code的类型，如果是pushparam的话，需要改变后面的funcCall
                            if (dynamic_cast<class PushParam*>(code) != nullptr) {
                                auto it2 = middleCodeItems.find(code);
                                auto funcCall = *it2;
                                while (dynamic_cast<class MiddleFuncCall*>(funcCall) == nullptr) {
                                    it2 ++ ;
                                    funcCall = *it2;
                                }
                                auto &rParams = dynamic_cast<class MiddleFuncCall*>(funcCall)->funcRParams;
                                auto it3 = rParams.begin();
                                for (it3 = rParams.begin(); it3 != rParams.end(); it3 ++ ) {
                                    if (dynamic_cast<ValueSymbol*>(*it3) == symbol) {
                                        break;
                                    }
                                }
                                it3 = rParams.erase(it3);
                                rParams.insert(it3, p->_getSrc1());
                            }
                        }
                    }
                }
            }
            auto def = code->getDef();
            // 处理unaryOp
            if (code->codeType == MiddleCodeItem::MiddleUnaryOp && dynamic_cast<Immediate*>(code->_getSrc1()) != nullptr) {
                int src1 = dynamic_cast<Immediate*>(code->_getSrc1())->value;
                int res = 0;
                auto op = dynamic_cast<class MiddleUnaryOp*>(code)->type;
                switch (op) {
                    case MiddleUnaryOp::NOT: res = !src1; break;
                    case MiddleUnaryOp::ASSIGN: res = src1; break;
                    case MiddleUnaryOp::POSITIVE: res = src1; break;
                    case MiddleUnaryOp::NEGATIVE: res = -src1; break;
                    default: break;
                }
                symbolToValueList->push_back({{code, def}, res});
            }
            if (dynamic_cast<Immediate *>(code->_getSrc1()) != nullptr &&
                dynamic_cast<Immediate *>(code->_getSrc2()) != nullptr) {
                int src1 = dynamic_cast<Immediate *>(code->_getSrc1())->value;
                int src2 = dynamic_cast<Immediate *>(code->_getSrc2())->value;
                int res = 0;
                if (code->codeType == MiddleCodeItem::MiddleBinaryOp) {
                    auto op = dynamic_cast<class MiddleBinaryOp *>(code)->type;
                    switch (op) {
                        case MiddleBinaryOp::ADD:
                            res = src1 + src2;
                            break;
                        case MiddleBinaryOp::SUB:
                            res = src1 - src2;
                            break;
                        case MiddleBinaryOp::MUL:
                            res = src1 * src2;
                            break;
                        case MiddleBinaryOp::DIV:
                            res = src1 / src2;
                            break;
                        case MiddleBinaryOp::MOD:
                            res = src1 % src2;
                            break;
                        case MiddleBinaryOp::EQ:
                            res = src1 == src2 ? 1 : 0;
                            break;
                        case MiddleBinaryOp::NE:
                            res = src1 != src2 ? 1 : 0;
                            break;
                        case MiddleBinaryOp::LT:
                            res = src1 < src2 ? 1 : 0;
                            break;
                        case MiddleBinaryOp::LE:
                            res = src1 <= src2 ? 1 : 0;
                            break;
                        case MiddleBinaryOp::GT:
                            res = src1 > src2 ? 1 : 0;
                            break;
                        case MiddleBinaryOp::GE:
                            res = src1 >= src2 ? 1 : 0;
                            break;
                        default:
                            break;
                    }
                    symbolToValueList->push_back({{code, def}, res});
                }
            }
            if (def != nullptr) {
                defs.push_back(code);
            }
        }
        if (symbolToValueList->empty()) {
            change = false;
        }
        for (auto pair: *symbolToValueList) {
            auto code = pair.first.first;
            auto symbol = pair.first.second;
            auto newCode = new class MiddleDef(MiddleDef::DEF_VAR, pair.first.second, new Immediate(pair.second));
            newCode->index = code->index;
            middleCodeItems.erase(code);
            middleCodeItems.insert(newCode);
            // 如果是pushparam的话，函数调用实际的值并没有发生替换
            if (dynamic_cast<class PushParam*>(code) != nullptr) {
                auto it = middleCodeItems.find(code);
                // 找到funcCall
                while (dynamic_cast<class MiddleFuncCall*>(*it) == nullptr) {
                    it ++ ;
                }
                auto &rParams = dynamic_cast<class MiddleFuncCall*>(*it)->funcRParams;
                bool insertFlag = false;
                auto it2 = rParams.begin();
                for (it2 = rParams.begin(); it2 != rParams.end(); it2 ++ ) {
                    if (dynamic_cast<ValueSymbol*>(*it2) == symbol) {
                        insertFlag = true;
                        break;
                    }
                }
                if (insertFlag) {
                    it2 = rParams.erase(it2);
                    rParams.insert(it2, new Immediate(pair.second));
                }
            }
        }
    }
}


// 为了建立冲突图
std::vector<std::set<ValueSymbol*>> *BasicBlock::calcCodeActive() {
    auto groups = new std::vector<std::set<ValueSymbol*>>();

    auto out = std::set<ValueSymbol*>();
    auto in = std::set<ValueSymbol*>();

    for (auto it = middleCodeItems.rbegin(); it != middleCodeItems.rend(); it ++ ) {
        // 每个code的冲突集合
        auto codeConflictGroup = new std::set<ValueSymbol*>();
        auto code = *it;
        // 最后一个代码，和该block的out是一样的
        if (it == middleCodeItems.rbegin()) {
            for (auto symbol : *this->outPosFlow) {
                if (symbol->isLocal) {
                    out.insert(symbol);
                }
            }
        }
        // 如果不是最后一行代码，那就是其下一行代码的in
        else {
            for (auto symbol : in) {
                // 建立冲突图时只考虑局部变量
                if (symbol->isLocal) {
                    out.insert(symbol);
                }
            }
        }
        // 计算该行代码的in
        for (auto symbol : out) {
            if (symbol->isLocal) {
                in.insert(symbol);
            }
        }
        in.erase(code->getDef());
        for (auto use : *code->getUse()) {
            if (use->isLocal) {
                in.insert(use);
            }
        }
        // 将out填入conflictGroup中
        for (auto symbol : out) {
            codeConflictGroup->insert(symbol);
        }
        // !!!建立冲突图时还应该包括该行代码的def和use
        // 如果是funcCall的话，还有其参数
        if (code->getDef() != nullptr) {
            codeConflictGroup->insert(code->getDef());
        }
        for (auto symbol : *code->getUse()) {
            codeConflictGroup->insert(symbol);
        }
        if (dynamic_cast<class MiddleFuncCall*>(code) != nullptr) {
            auto params = dynamic_cast<class MiddleFuncCall*>(code)->funcRParams;
            for (auto symbol : params) {
                if (dynamic_cast<ValueSymbol*>(symbol) != nullptr) {
                    codeConflictGroup->insert(dynamic_cast<ValueSymbol*>(symbol));
                }
            }
        }


        // code的activeSymbols设置为在函数中，该行代码之后的所有use
        // !!! 还要加上活跃变量分析的结果，因为如果是for(a)的话，代码后并不会出现a
        code->activeSymbols = codeConflictGroup;
        groups->push_back(*codeConflictGroup);

#ifdef DEBUG
        std::cout << "#######[calcCodeActive] " << std::endl;
        std::cout << *(*it) << std::endl;
        for (auto symbol : *codeConflictGroup) {
            std::cout << symbol->name << " " << symbol << "\n";
        }
#endif
    }
    return groups;
}

void BasicBlock::resetIndex() {
    int i = 0;
    for (auto code : middleCodeItems) {
        code->setIndex(i);
        i ++ ;
    }
}
//
// Created by 安达楷 on 2023/11/19.
//
#include "Middle.h"
#include "BasicBlock.h"
#include <set>
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
        auto def = code->getDef();
        if (def != nullptr) {
            auto name = def->name;
            if (!this->useSet->count(def)) {
                this->defSet->insert(def);
            }
        }
        auto uses = code->getUse();
        if (uses != nullptr) {
            for (auto use : *uses) {
                if (use != nullptr) {
                    if (!this->defSet->count(use)) {
                        this->useSet->insert(use);
                    }
                }
            }
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
                    dynamic_cast<class MiddleIO*>(code)->target = nullptr;
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
}


void BasicBlock::inBroadcast() {
    if (this->basicBlockID == 28) {
        int a = 1;
    }
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
                    if (p->codeType == MiddleCodeItem::MiddleDef &&
                        p->_getSrc1() != nullptr && dynamic_cast<Immediate *>(p->_getSrc1()) != nullptr) {
                        code->reset(symbol, p->_getSrc1());
                        // 特判一下code的类型，如果是pushparam的话，那就需要加入symboltoValue中。
                        if (dynamic_cast<class PushParam*>(code) != nullptr) {
                            symbolToValueList->push_back({{code, symbol}, dynamic_cast<Immediate*>(p->_getSrc1())->value});
                        }
                        change = true;

                    }
                }
            }
            auto def = code->getDef();
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
                it2 = rParams.erase(it2);
                rParams.insert(it2, new Immediate(pair.second));
            }
        }
    }
}
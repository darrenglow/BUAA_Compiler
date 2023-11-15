//
// Created by 安达楷 on 2023/11/14.
//

#include "NoChangeValue.h"

void NoChangeValue::add(ValueSymbol *valueSymbol, int value) {
    std::cout << valueSymbol->name << std::endl;
    if (noChangeValues.count(valueSymbol) == 0)
        noChangeValues.insert({valueSymbol, value});
}

void NoChangeValue::remove(ValueSymbol *valueSymbol) {
    if (valueSymbol == nullptr) {
        return;
    }
    if (!noChangeValues.count(valueSymbol)) {
        return;
    }
    noChangeValues.erase(valueSymbol);
}

void NoChangeValue::replace(std::vector<Func *> &funcs) {
    for (auto x : noChangeValues) {
        std::cout << x.first->name << " ";
    }
    std::cout << std::endl;
    for (auto func : funcs) {
        auto block = func->block;
        for (auto middleCode : block->middleCodeItems) {
            if (dynamic_cast<MiddleFuncCall*>(middleCode)) {
                auto middleFuncCall = dynamic_cast<MiddleFuncCall*>(middleCode);
                for (int i = 0; i < middleFuncCall->funcRParams.size(); i ++ ) {
                    if (exist(dynamic_cast<ValueSymbol*>(middleFuncCall->funcRParams[i]))) {
                        middleFuncCall->funcRParams[i] = new Immediate(noChangeValues[dynamic_cast<ValueSymbol*>(middleFuncCall->funcRParams[i])]);
                    }
                }
            }
            else if (dynamic_cast<MiddleReturn*>(middleCode)) {
                auto valueSymbol = dynamic_cast<MiddleReturn*>(middleCode)->target;
                if (exist(dynamic_cast<ValueSymbol*>(valueSymbol))) {
                    dynamic_cast<MiddleReturn*>(middleCode)->target = new Immediate(noChangeValues[dynamic_cast<ValueSymbol*>(valueSymbol)]);
                }
            }
            else if (dynamic_cast<MiddleDef*>(middleCode)) {
                auto valueSymbol = dynamic_cast<MiddleDef*>(middleCode)->srcValueSymbol;
                if (exist(dynamic_cast<ValueSymbol*>(valueSymbol))) {
                    dynamic_cast<MiddleDef*>(middleCode)->srcValueSymbol = new Immediate(noChangeValues[dynamic_cast<ValueSymbol*>(valueSymbol)]);
                }
            }
            else if (dynamic_cast<MiddleBinaryOp*>(middleCode)) {
                auto symbol1 = dynamic_cast<MiddleBinaryOp*>(middleCode)->src1;
                auto symbol2 = dynamic_cast<MiddleBinaryOp*>(middleCode)->src2;
                auto valueSymbol1 = dynamic_cast<ValueSymbol*>(symbol1);
                auto valueSymbol2 = dynamic_cast<ValueSymbol*>(symbol2);
                if (exist(valueSymbol1)) {
                    dynamic_cast<MiddleBinaryOp*>(middleCode)->src1 = new Immediate(noChangeValues[valueSymbol1]);
                }
                if (exist(valueSymbol2)) {
                    dynamic_cast<MiddleBinaryOp*>(middleCode)->src2 = new Immediate(noChangeValues[valueSymbol2]);
                }
            }
            else if (dynamic_cast<MiddleOffset*>(middleCode)) {
                auto sym = dynamic_cast<MiddleOffset*>(middleCode)->offset;
                auto valueSymbol = dynamic_cast<ValueSymbol*>(sym);
                if (exist(valueSymbol)) {
                    dynamic_cast<MiddleOffset*>(middleCode)->offset = new Immediate(noChangeValues[valueSymbol]);
                }
            }
            else if (dynamic_cast<MiddleMemoryOp*>(middleCode)) {
                auto sym1 = dynamic_cast<MiddleMemoryOp*>(middleCode)->sym1;
                auto sym2 = dynamic_cast<MiddleMemoryOp*>(middleCode)->sym2;
                auto val1 = dynamic_cast<ValueSymbol*>(sym1);
                auto val2 = dynamic_cast<ValueSymbol*>(sym2);
                if (exist(val1)) {
                    dynamic_cast<MiddleMemoryOp*>(middleCode)->sym1 = new Immediate(noChangeValues[val1]);
                }
                if (exist(val2)) {
                    dynamic_cast<MiddleMemoryOp*>(middleCode)->sym2 = new Immediate(noChangeValues[val2]);
                }
            }
            else if (dynamic_cast<MiddleUnaryOp*>(middleCode)) {
                auto sym = dynamic_cast<MiddleUnaryOp*>(middleCode)->srcValueSymbol;
                auto val = dynamic_cast<ValueSymbol*>(sym);
                if (exist(val)) {
                    dynamic_cast<MiddleUnaryOp*>(middleCode)->srcValueSymbol = new Immediate(noChangeValues[val]);
                }
            }
            else if (dynamic_cast<MiddleJump*>(middleCode)) {
                auto sym = dynamic_cast<MiddleJump*>(middleCode)->src;
                auto val = dynamic_cast<ValueSymbol*>(sym);
                if (exist(val)) {
                    dynamic_cast<MiddleJump*>(middleCode)->src = new Immediate(noChangeValues[val]);
                }
            }
            else if (dynamic_cast<MiddleIO*>(middleCode)) {
                auto sym = dynamic_cast<MiddleIO*>(middleCode)->target;
                auto val = dynamic_cast<ValueSymbol*>(sym);
                if (exist(val)) {
                    dynamic_cast<MiddleIO*>(middleCode)->target = new Immediate(noChangeValues[val]);
                }
            }
        }
    }
}

bool NoChangeValue::exist(ValueSymbol *valueSymbol) {
    if (valueSymbol == nullptr) {return false;}
    return noChangeValues.count(valueSymbol) > 0;
}
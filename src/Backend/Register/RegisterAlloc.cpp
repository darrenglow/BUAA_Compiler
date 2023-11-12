//
// Created by 安达楷 on 2023/11/6.
//

#include "RegisterAlloc.h"
#include "../MipsGenerator.h"
#include "Register.h"

RegType RegisterAlloc::allocRegister(Symbol *symbol, bool fromMemory) {
    auto existReg = findRegister(symbol);
    if (existReg != RegType::none) {
        return existReg;
    }
    auto reg = availableRegs[ptr];
    ptr ++ ;
    if (ptr == availableRegs.size()) {
        ptr = 0;
    }

    if (fromMemory) {
        if (dynamic_cast<ValueSymbol*>(symbol)) {
            auto valueSymbol = dynamic_cast<ValueSymbol*>(symbol);
            // reg中存pointer指向的地址
            if (valueSymbol->valueType == ValueType::POINTER) {
                auto offset = valueSymbol->getAddress();   // 偏移的地址

                if (valueSymbol->isLocal) {
                    MipsGenerator::getInstance().add(new RRI(RRI::addiu, reg, $sp, -offset));
                }
                else {
                    std::cout << offset << "####" << std::endl;
                    MipsGenerator::getInstance().add(new RRI(RRI::addiu, reg, $gp, offset));
                }
            }
            else {
                if (valueSymbol->isLocal) {
                    MipsGenerator::getInstance().add(new M(M::lw, reg, -valueSymbol->getAddress(), $sp));
                }
                else {
                    MipsGenerator::getInstance().add(new M(M::lw, reg, valueSymbol->getAddress(), $gp));
                }
            }

        }
        else if (dynamic_cast<NumSymbol*>(symbol)) {
            auto numSymbol = dynamic_cast<NumSymbol*>(symbol);
            MipsGenerator::getInstance().add(new RI(RI::li, reg, numSymbol->value));
        }
    }

    registerMap[reg] = symbol;
    return reg;
}

RegType RegisterAlloc::findRegister(Symbol *symbol) {
    for (const auto &pair : registerMap) {
        if (pair.second == symbol) {
            return pair.first;
        }
    }
    return RegType::none;
}

void RegisterAlloc::clearRegister() {
    for (auto pair : registerMap) {
        // 如果寄存器对应有符号，并且符号是valueSymbol的话
        // TODO: 如果是地址的话，不需要写回
        if (pair.second != nullptr && dynamic_cast<ValueSymbol*>(pair.second)) {
            pushBackToMem(pair.first, dynamic_cast<ValueSymbol*>(pair.second));
        }
    }
}

void RegisterAlloc::pushBackToMem(RegType reg, ValueSymbol *valueSymbol) {
    MipsGenerator::getInstance().add(new Notation("# save to value " + valueSymbol->name));
    MipsGenerator::getInstance().add(new M(M::sw, reg, -valueSymbol->getAddress(), $sp));
}


//
// Created by 安达楷 on 2023/11/6.
//

#include "RegisterAlloc.h"
#include "../MipsGenerator.h"
#include "Register.h"
#include "../../Util/Debug.h"

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

    // 如果不为空，将原来的写回内存中
    // TODO: 地址不写回。
    if (registerMap[reg] != nullptr) {
        if (dynamic_cast<ValueSymbol*>(registerMap[reg])) {
            MipsGenerator::getInstance().add(new Notation("# Register[" + Register::type2str[reg] + "] " + dynamic_cast<ValueSymbol*>(registerMap[reg])->name + " to " + symbol->printMiddleCode()));
            pushBackToMem(reg, dynamic_cast<ValueSymbol*>(registerMap[reg]));
        }
    }

    if (fromMemory) {
        if (dynamic_cast<ValueSymbol*>(symbol)) {
            auto valueSymbol = dynamic_cast<ValueSymbol*>(symbol);
            // reg中存pointer指向的地址
            if (valueSymbol->valueType == ValueType::POINTER) {
                auto offset = valueSymbol->getPAddress();   // 偏移地址，或绝对地址

                if (valueSymbol->isAbsoluteAddress) {
                    if (dynamic_cast<Immediate*>(offset)) {
                        MipsGenerator::getInstance().add(new RI(RI::li, reg, dynamic_cast<Immediate*>(offset)->value));
                    }
                    else {
                        // TODO: 或许可以直接放到reg上
                        auto offReg = allocRegister(dynamic_cast<ValueSymbol*>(offset));
                        MipsGenerator::getInstance().add(new RR(RR::move, reg, offReg));
                    }
                }
                else if (dynamic_cast<Immediate*>(offset)) {
                    if (valueSymbol->isLocal) {
                        MipsGenerator::getInstance().add(new RRI(RRI::addiu, reg, $sp, -dynamic_cast<Immediate*>(offset)->value));
                        // !!!如果是参数数组的话，需要把地址值加载到目标寄存器中
                        if (valueSymbol->isFParam) {
                            MipsGenerator::getInstance().add(new M(M::lw, reg, 0, reg));
                        }
                    }
                    else {
                        MipsGenerator::getInstance().add(new RRI(RRI::addiu, reg, $gp, dynamic_cast<Immediate*>(offset)->value));
                    }
                }
                else {
                    auto offReg = allocRegister(dynamic_cast<ValueSymbol*>(offset));
                    if (valueSymbol->isLocal) {
                        MipsGenerator::getInstance().add(new RRR(RRR::subu, reg, $sp, offReg));
                    }
                    else {
                        MipsGenerator::getInstance().add(new RRR(RRR::addu, reg, $gp, offReg));
                    }
                }
            }
            // 如果是参数数组，需要先根据其在栈中的偏移得到其参数的地址，然后将其值到寄存器中。
//            else if (valueSymbol->valueType == ValueType::FUNCFPARAM) {
//                auto offset = valueSymbol->getPAddress();
//                if (dynamic_cast<Immediate*>(offset)) {
//                    MipsGenerator::getInstance().add(new M(M::lw, reg, -dynamic_cast<Immediate*>(offset)->value, $sp));
//                }
//                else {
//                    auto offReg = allocRegister(dynamic_cast<ValueSymbol*>(offset));
//                    // TODO: 这里随便设了个寄存器，不知道会不会有问题。
//                    auto tempReg = allocRegister(new NumSymbol(1));
//                    MipsGenerator::getInstance().add(new RRR(RRR::subu, tempReg, $sp, offReg));
//                    MipsGenerator::getInstance().add(new M(M::lw, reg, 0, tempReg));
//                }
//            }
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
    // 只是将变量映射到寄存器上
    else {
        MipsGenerator::getInstance().add(new Notation("### " + symbol->name + " -> " + Register::type2str[reg]));
    }
    registerMap[reg] = symbol;
    return reg;
}


RegType RegisterAlloc::allocRegisterAvoid(RegType reg, Symbol *symbol, bool fromMemory) {
    auto toAllocReg = availableRegs[ptr];
    if (toAllocReg == reg) {
        ptr ++ ;
        if (ptr == availableRegs.size()) {
            ptr = 0;
        }
    }
    return allocRegister(symbol, fromMemory);
}


RegType RegisterAlloc::findRegister(Symbol *symbol) {
    for (const auto &pair : registerMap) {
        if (pair.second == symbol) {
            return pair.first;
        }
    }
    return RegType::none;
}

void RegisterAlloc::saveRegisters() {
    for (auto pair : registerMap) {
        // 如果寄存器对应有符号，并且符号是valueSymbol的话
        // TODO: 如果是地址的话，不需要写回
        if (pair.second != nullptr && dynamic_cast<ValueSymbol*>(pair.second)) {
            auto valueSymbol = dynamic_cast<ValueSymbol*>(pair.second);
            pushBackToMem(pair.first, valueSymbol);
            registerMap[pair.first] = nullptr;
        }
    }
}



void RegisterAlloc::pushBackToMem(RegType reg, ValueSymbol *valueSymbol) {
    // TODO: 这个貌似有问题，参数可以写回内存的
    // ！！！如果参数是普通变量，那么不写回内存中。错错错！可以写回的。
    if (valueSymbol->valueType == ValueType::FUNCFPARAM) {
        return;
    }
    if (valueSymbol->getDim() > 0) {
        return;
    }
    MipsGenerator::getInstance().add(new Notation("# save to value " + valueSymbol->name));
    if (valueSymbol->isLocal) {
        MipsGenerator::getInstance().add(new M(M::sw, reg, -valueSymbol->getAddress(), $sp));
    }
    else {
        MipsGenerator::getInstance().add(new M(M::sw, reg, valueSymbol->getAddress(), $gp));
    }
}

void RegisterAlloc::clearAllRegister() {
    for (auto pair : registerMap) {
        registerMap[pair.first] = nullptr;
    }
}

void RegisterAlloc::clearRegister(RegType reg) {
    if (registerMap[reg] == nullptr) {
        return;
    }
    else if (dynamic_cast<ValueSymbol*>(registerMap[reg])){
        pushBackToMem(reg, dynamic_cast<ValueSymbol*>(registerMap[reg]));
    }
    registerMap[reg] = nullptr;
}

void RegisterAlloc::forceSymbolToRegister(ValueSymbol *valueSymbol, RegType reg) {
    clearRegister(reg);
    registerMap[reg] = valueSymbol;
}
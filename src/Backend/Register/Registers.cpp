//
// Created by 安达楷 on 2023/12/1.
//
#include "Registers.h"

bool Registers::hasSymbol(Symbol *symbol) {
    if (symbol2reg.count(symbol) != 0) {
        return true;
    }
    return false;
}

bool Registers::hasRegister(RegType reg) {
    if (reg2symbol.count(reg) != 0) {
        return true;
    }
    return false;
}
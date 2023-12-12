//
// Created by 安达楷 on 2023/12/1.
//

#ifndef BUAA_COMPILER_REGISTERS_H
#define BUAA_COMPILER_REGISTERS_H

#include "Register.h"
#include <unordered_map>
#include "../../Middle/MiddleCodeItem/Middle.h"
class Registers {
    std::unordered_map<Symbol*, RegType> symbol2reg{};
    std::unordered_map<RegType, Symbol*> reg2symbol{};
    bool hasRegister(RegType reg);
    bool hasSymbol(Symbol* symbol);
    bool freeRegister(RegType reg);
};

#endif //BUAA_COMPILER_REGISTERS_H

//
// Created by 安达楷 on 2023/11/19.
//

#include "Middle.h"
// Def
std::ostream & MiddleDef::output(std::ostream &os) const {
    os << type2str[type] << " ";
    if (isInit && type == DEF_VAR && srcValueSymbol != nullptr) {
        os << srcValueSymbol->printMiddleCode() << " ";
    }
    os << valueSymbol->name << "[0x" << std::hex << valueSymbol->getAddress() << "]";
    return os;
}

Intermediate * MiddleDef::getLeftIntermediate() {
    return valueSymbol;
}
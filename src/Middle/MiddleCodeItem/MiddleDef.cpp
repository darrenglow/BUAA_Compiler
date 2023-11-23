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
    os << dynamic_cast<ValueSymbol*>(valueSymbol)->name << "[0x" << std::hex << dynamic_cast<ValueSymbol*>(valueSymbol)->getAddress() << "]";
    return os;
}

Intermediate * MiddleDef::_getRet() {
    if (type == MiddleDef::END_ARRAY || type == MiddleDef::DEF_ARRAY) {
        return nullptr;
    }
    return valueSymbol;
}

Intermediate * MiddleDef::_getSrc1() {
    return srcValueSymbol;
}

Intermediate **MiddleDef::getPointerToRet() {
    return &valueSymbol;
}

Intermediate **MiddleDef::getPointerToSrc1() {
    return &srcValueSymbol;
}
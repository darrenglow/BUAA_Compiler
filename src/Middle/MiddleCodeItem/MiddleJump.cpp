//
// Created by 安达楷 on 2023/11/19.
//

#include "Middle.h"

// MiddleJump
std::ostream & MiddleJump::output(std::ostream &os) const {
    os << type2str[type] << " ";
    if (src != nullptr)
        os << src->printMiddleCode() << " ";
    os << target->label->label;
    if (anotherTarget != nullptr) {
        os << " (" << anotherTarget->label->label << ")";
    }
    return os;
}

Intermediate * MiddleJump::_getSrc1() {
    if (type == MiddleJump::JUMP_EQZ)
        return src;
    return nullptr;
}


Intermediate **MiddleJump::getPointerToSrc1() {
    if (type == MiddleJump::JUMP_EQZ)
        return &src;
    return nullptr;
}
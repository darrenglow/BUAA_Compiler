//
// Created by 安达楷 on 2023/11/19.
//
#include "Middle.h"
// BinaryOp
std::ostream & MiddleBinaryOp::output(std::ostream &os) const {
    os << type2str[type] << " ";
    os << src1->printMiddleCode() << " ";
    os << src2->printMiddleCode() << " ";
    os << target->printMiddleCode();
    return os;
}

Intermediate * MiddleBinaryOp::getLeftIntermediate() {
    return target;
}
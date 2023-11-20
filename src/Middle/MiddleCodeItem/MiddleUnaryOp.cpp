//
// Created by 安达楷 on 2023/11/19.
//

#include "Middle.h"
// UnaryOp
std::ostream & MiddleUnaryOp::output(std::ostream &os) const {
    os << type2str[type] << " ";
    os << srcValueSymbol->printMiddleCode() << " ";
    os << valueSymbol->printMiddleCode();

    return os;
}

Intermediate * MiddleUnaryOp::getLeftIntermediate() {
    return valueSymbol;
}

Intermediate * MiddleUnaryOp::getRightIntermediate1() {
    return srcValueSymbol;
}
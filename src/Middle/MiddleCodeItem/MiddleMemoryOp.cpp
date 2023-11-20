//
// Created by 安达楷 on 2023/11/19.
//

#include "Middle.h"
// MemoryOp
std::ostream & MiddleMemoryOp::output(std::ostream &os) const {
    os << type2str[type] << " ";
    os << sym1->printMiddleCode() << " ";
    os << sym2->printMiddleCode();
    return os;
}
Intermediate * MiddleMemoryOp::getLeftIntermediate() {
    if (type == MiddleMemoryOp::STORE) {
        return sym2;
    }
    else {
        return sym1;
    }
}
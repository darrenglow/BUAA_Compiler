//
// Created by 安达楷 on 2023/11/19.
//

#include "Middle.h"
// Offset
std::ostream & MiddleOffset::output(std::ostream &os) const {
    // 如果是立即数的话，就是offset * 4
    os << "OFFSET " << offset->printMiddleCode() << " " << src->printMiddleCode() << " " << ret->printMiddleCode();
    return os;
}

Intermediate * MiddleOffset::getLeftIntermediate() {
    return ret;
//    return nullptr;
}


Intermediate * MiddleOffset::getRightIntermediate1() {
    return offset;
//    return nullptr;
}

Intermediate * MiddleOffset::getRightIntermediate2() {
    return src;
//    return nullptr;
}
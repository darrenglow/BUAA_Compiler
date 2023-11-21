//
// Created by 安达楷 on 2023/11/19.
//

#include "Middle.h"

// MiddleIO
std::ostream & MiddleIO::output(std::ostream &os) const {
    os << type2str[type];
    if (target != nullptr){
        os << " " << target->printMiddleCode();
    }
    return os;
}

Intermediate * MiddleIO::getLeftIntermediate() {
    if (type == MiddleIO::GETINT){
        return target;
    }
    return nullptr;
}

Intermediate * MiddleIO::getRightIntermediate1() {
    if (type == MiddleIO::PRINT_INT) {
        return target;
    }
    return nullptr;
}
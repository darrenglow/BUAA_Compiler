//
// Created by 安达楷 on 2023/11/19.
//

#include "Middle.h"

// MiddleIO
std::ostream & MiddleIO::output(std::ostream &os) const {
    os << type2str[type] << " " << target->printMiddleCode();
    return os;
}

Intermediate * MiddleIO::getLeftIntermediate() {
    return target;
}
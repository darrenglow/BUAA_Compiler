//
// Created by 安达楷 on 2023/11/19.
//

#include "Middle.h"
// return
std::ostream & MiddleReturn::output(std::ostream &os) const {
    os << "return ";
    if (target != nullptr) {
        os << target->printMiddleCode();
    }
    return os;
}


Intermediate * MiddleReturn::_getSrc1() {
    return target;
}
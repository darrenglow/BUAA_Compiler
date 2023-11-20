//
// Created by 安达楷 on 2023/11/19.
//

#include "Middle.h"
// MiddleFuncCall
std::ostream & MiddleFuncCall::output(std::ostream &os) const {
    os << "CALL " << funcName << "( ";
    for (auto x : funcRParams) {
        os << x->printMiddleCode() << " ";
    }
    os << ")" << std::endl;
    return os;
}
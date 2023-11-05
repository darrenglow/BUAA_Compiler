//
// Created by 安达楷 on 2023/11/2.
//

#include <string>
#include "Immediate.h"

std::string Immediate::printMiddleCode() {
    if (type == VALUE)
        return std::to_string(value);
    else
        return str;
}
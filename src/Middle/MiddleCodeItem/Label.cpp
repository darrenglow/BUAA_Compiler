//
// Created by 安达楷 on 2023/11/19.
//
#include "Middle.h"

int Label::count = 1;
std::ostream & Label::output(std::ostream &os) const {
    os << label << ":";
    return os;
}
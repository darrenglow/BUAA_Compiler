//
// Created by 安达楷 on 2023/10/24.
//

#include "MiddleCodeItem.h"
#include "../Visitor/Visitor.h"
#include <iostream>

std::ostream& operator<<(std::ostream& os, const MiddleCodeItem& obj) {
    return obj.output(os);
}

Intermediate * MiddleCodeItem::getLeftIntermediate() { return nullptr; }

void MiddleCodeItem::setIndex(int x) {
    index = x;
}

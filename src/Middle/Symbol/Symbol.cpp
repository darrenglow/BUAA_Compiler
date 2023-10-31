//
// Created by 安达楷 on 2023/10/31.
//
#include "Symbol.h"

#include <utility>

int ValueSymbol::getDim() const {
    return dims.size();
}

void ValueSymbol::setBlockLevel(int level) {
    blockLevel = level;
}

void ValueSymbol::setInitValues(std::vector<int> x) {
    initValues = std::move(x);
}

int ValueSymbol::getFlattenDim() {
    int base = 1;
    for (auto x : dims) {
        base *= x;
    }
    return base;
}
//
// Created by 安达楷 on 2023/10/31.
//
#include "Symbol.h"

#include <utility>

std::string Symbol::printMiddleCode() {
    return name;
}

int ValueSymbol::getDim() const {
    return dims.size();
}

void ValueSymbol::setBlockLevel(int level) {
    blockLevel = level;
}

void ValueSymbol::setInitValues(std::vector<int> x) {
    initValues = std::move(x);
}

int ValueSymbol::getFlattenDim() const {
    int base = 1;
    for (auto x : dims) {
        base *= x;
    }
    return base;
}

void ValueSymbol::setAddress(int addr) {
    this->address = addr;
}

void ValueSymbol::setLocal(bool local) {
    this->isLocal = local;
}

int ValueSymbol::getSize() const {
    if (valueType == ARRAY) {
        return getFlattenDim() * 4;
    }
    else {
        return 4;
    }
}

int ValueSymbol::getAddress() {
    return address;
}

void ValueSymbol::setToPointer() {
    valueType = ValueType::POINTER;
}

int FuncSymbol::getDim() const {
    return 0;
}

int FuncSymbol::getSize() const {
    return 0;
}

bool FuncSymbol::isConst() const {
    return false;
}

int NumSymbol::getSize() const {
    return 4;
}

bool NumSymbol::isConst() const {
    return true;
}

int NumSymbol::getDim() const {
    return 0;
}
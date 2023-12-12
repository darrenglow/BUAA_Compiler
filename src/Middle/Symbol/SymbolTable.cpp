//
// Created by 安达楷 on 2023/10/4.
//

#include "SymbolTable.h"

bool SymbolTable::hasParent() {
    return parent != nullptr;
}

void SymbolTable::add(Symbol *symbol) {
    symbols[symbol->name] = symbol;
    size += symbol->getSize();
}

// 是否包括name变量
bool SymbolTable::contain(const std::string &name, bool up) {
    return getSymbol(name, up) != nullptr;
}

// 得到name标识符的symbol
Symbol * SymbolTable::getSymbol(const std::string &name, bool up) {
    auto it = symbols.find(name);
    if (it != symbols.end()) {
        return it->second;
    }
    else {
        if (hasParent() && up) {
            return parent->getSymbol(name, true);
        }
        return nullptr;
    }
}

int SymbolTable::getSize() {
    return size;
}

Symbol * SymbolTable::getSymbol(const std::string &name, int offset, bool up) {
    auto realName = "ArraY_*$+|!123___" + name + "_" + std::to_string(offset);
    auto it = this->getSymbol(realName, up);
    if (it != nullptr) {
        return it;
    }
    return nullptr;
}

Symbol * SymbolTable::getSymbol(const std::string &name, Intermediate* offset, bool up) {
    auto realName = "ArraY_*$+|!123___" + name + "_" + offset->printMiddleCode();
    auto it = this->getSymbol(realName, up);
    if (it != nullptr) {
        return it;
    }
    return nullptr;
}

Symbol * SymbolTable::getSymbol(const std::string &name, std::string offset, bool up) {
    auto realName = "ArraY_*$+|!123___" + name + "_" + offset;
    auto it = this->getSymbol(realName, up);
    if (it != nullptr) {
        return it;
    }
    return nullptr;
}

// int a[2]; func(a);    则为 PointeR*|!123___a_1_0_0
// int a[2][2]; func(a);    PointeR*|!123___a_2_0_0
// int a[2][2]; func(a[1]);     PointeR*|!123___a_2_1_1
// 主要目的就是保证在符号表中的名字能够唯一
Symbol * SymbolTable::getSymbol(const std::string &name, int realDim, int useDim, int offset1, bool up) {
    auto realName = "PointeR_*$+|!123___" + name + "_" + std::to_string(realDim) + "_" + std::to_string(useDim) + "_" + std::to_string(offset1);
    auto it = this->getSymbol(realName, up);
    if (it != nullptr) {
        return it;
    }
    return nullptr;
}

Symbol * SymbolTable::getSymbol(const std::string &name, int realDim, int useDim, Intermediate* offset1, bool up) {
    auto realName = "PointeR_*$+|!123___" + name + "_" + std::to_string(realDim) + "_" + std::to_string(useDim) + "_" + offset1->printMiddleCode();
    auto it = this->getSymbol(realName, up);
    if (it != nullptr) {
        return it;
    }
    return nullptr;
}
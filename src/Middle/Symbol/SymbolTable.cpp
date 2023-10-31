//
// Created by 安达楷 on 2023/10/4.
//

#include "SymbolTable.h"

bool SymbolTable::hasParent() {
    return parent != nullptr;
}

void SymbolTable::add(Symbol *symbol) {
    symbols[symbol->name] = symbol;
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
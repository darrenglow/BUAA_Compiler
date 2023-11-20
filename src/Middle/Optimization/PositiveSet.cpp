//
// Created by 安达楷 on 2023/11/20.
//

#include "PositiveSet.h"

void PositiveSet::add(ValueSymbol *symbol) {
    symbols.insert(symbol);
}

void PositiveSet::remove(ValueSymbol *symbol) {
    symbols.erase(symbol);
}

bool PositiveSet::contain(ValueSymbol *symbol) {
    return symbols.count(symbol) > 0;
}

void PositiveSet::plus(PositiveSet *posSet) {
    for (auto x : posSet->symbols) {
        this->symbols.insert(x);
    }
}

void PositiveSet::sub(PositiveSet *posSet) {
    for (auto x : posSet->symbols) {
        this->symbols.erase(x);
    }
}

bool PositiveSet::equalTo(PositiveSet *posSet) {
    return this->symbols == posSet->symbols;
}
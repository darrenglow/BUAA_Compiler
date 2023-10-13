//
// Created by 安达楷 on 2023/10/4.
//

#ifndef BUAA_COMPILER_SYMBOLTABLE_H
#define BUAA_COMPILER_SYMBOLTABLE_H

#include <map>
#include "Symbol.h"

class SymbolTable{
public:
    std::map<std::string, Symbol*> symbols;
    SymbolTable* parent = nullptr;
    SymbolTable()=default;
    explicit SymbolTable(SymbolTable* parent_) : parent(parent_) {}

    bool hasParent();

    void add(Symbol* symbol);

    bool contain(const std::string &name, bool up);

    Symbol* getSymbol(const std::string &name, bool up);
};

#endif //BUAA_COMPILER_SYMBOLTABLE_H

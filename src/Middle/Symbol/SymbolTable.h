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
    int size = 0;

    SymbolTable* parent = nullptr;
    SymbolTable()=default;
    explicit SymbolTable(SymbolTable* parent_) : parent(parent_) {}

    bool hasParent();

    void add(Symbol* symbol);

    bool contain(const std::string &name, bool up);

    Symbol* getSymbol(const std::string &name, bool up);

    // 针对数组变量来特殊处理的
    Symbol* getSymbol(const std::string &name, int offset, bool up);
    Symbol * getSymbol(const std::string &name, Intermediate* offset, bool up);

    // 针对指针变量特殊处理
    Symbol *getSymbol(const std::string &name, int realDim, int useDim, int offset1, bool up);
    Symbol * getSymbol(const std::string &name, int realDim, int useDim, Intermediate* offset1, bool up);
    int getSize();
};

#endif //BUAA_COMPILER_SYMBOLTABLE_H

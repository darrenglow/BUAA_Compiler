//
// Created by 安达楷 on 2023/10/4.
//

#ifndef BUAA_COMPILER_SYMBOL_H
#define BUAA_COMPILER_SYMBOL_H

#include <string>
#include <vector>
#include <iostream>
enum BasicType{
    INT,
    VOID
};
enum ValueType{
    SINGLE,
    ARRAY,
};

class Symbol{
public:
    std::string name;
    BasicType basicType;    // 默认都是int
    Symbol(std::string name_, BasicType basicType_=INT)
        :name(name_), basicType(basicType_) {}
    virtual ~Symbol() {}
};

class FuncFParamSymbol : public Symbol  {
public:
    std::vector<int> dims;
    ValueType valueType;
    FuncFParamSymbol(std::string &name_)
            : Symbol(name_), valueType(SINGLE) {};
    FuncFParamSymbol(std::string &name_, std::vector<int> dims_)
        : Symbol(name_), dims(dims_), valueType(ARRAY) {};

};

class FuncSymbol : public Symbol {
public:
    int num;
    std::vector<FuncFParamSymbol*> funcFParamSymbols;
    FuncSymbol(std::string &name, BasicType basicType_, int num_)
        : Symbol(name, basicType_), num(num_) {};
    void addFuncFParamSymbol(FuncFParamSymbol* funcFParamSymbol) {
        funcFParamSymbols.push_back(funcFParamSymbol);
    }
};

class ValueSymbol : public Symbol {
public:
    ValueType valueType;
    std::vector<int> dims;   //具体维度
    std::vector<int> initValues; //展开后的一维值
    int initValue;
    bool isConst;
    //形如int a; int a=10; const int a = 10;
    ValueSymbol(std::string &name_, int initValue_=0, bool isConst_=false)
    : Symbol(name_), valueType(SINGLE), initValue(initValue_), isConst(isConst_) {};
    //形如int a[10];
    ValueSymbol(std::string &name_, std::vector<int> dims_, bool isConst_=false)
    : Symbol(name_), valueType(ARRAY), dims(dims_), isConst(isConst_) {};
    //形如int a[10] = {...}; a[10][10]={...};
    ValueSymbol(std::string &name_, std::vector<int> dims_, std::vector<int> initValues_, bool isConst_=false)
    : Symbol(name_), valueType(ARRAY), dims(dims_), initValues(initValues_), isConst(isConst_) {};
};

#endif //BUAA_COMPILER_SYMBOL_H

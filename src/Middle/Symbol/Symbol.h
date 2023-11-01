//
// Created by 安达楷 on 2023/10/4.
//

#ifndef BUAA_COMPILER_SYMBOL_H
#define BUAA_COMPILER_SYMBOL_H

#include <string>
#include <vector>
#include <iostream>
#include "../Intermediate/Intermediate.h"

enum BasicType{
    INT,
    VOID
};
enum ValueType{
    SINGLE,
    ARRAY,
    POINTER
};

class Symbol : public Intermediate{
public:
    std::string name;
    BasicType basicType;    // 默认都是int
    explicit Symbol(std::string &name_, BasicType basicType_=INT)
        :name(name_), basicType(basicType_) {}
    virtual ~Symbol() = default;
    virtual bool isConst() const = 0;
    virtual int getDim() const = 0;
};

class FuncFParamSymbol : public Symbol  {
public:
    std::vector<int> dims;
    ValueType valueType;
    FuncFParamSymbol(std::string &name_)
            : Symbol(name_), valueType(SINGLE) {};
    FuncFParamSymbol(std::string &name_, std::vector<int> &dims_)
        : Symbol(name_), dims(dims_), valueType(ARRAY) {};
    bool isConst() const {
        return false;
    }
    int getDim() const {
        return dims.size();
    }
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
    bool isConst() const {
        return false;
    }
    int getDim() const {
        return -100;
    }
};

class ValueSymbol : public Symbol {
public:
    ValueType valueType;
    std::vector<int> dims;   //具体维度
    std::vector<int> initValues; //展开后的一维值
    int initValue;
    bool isConstValue;
    int blockLevel;         //是在第几个嵌套括号中，全局变量就是0

    //形如int a; int a=10; const int a = 10;
    ValueSymbol(std::string name_, int initValue_=0, bool isConst_=false)
    : Symbol(name_), valueType(SINGLE), initValue(initValue_), isConstValue(isConst_) {};
    //形如int a[10];
    ValueSymbol(std::string name_, std::vector<int> &dims_, bool isConst_=false)
    : Symbol(name_), valueType(ARRAY), dims(dims_), isConstValue(isConst_), initValues({0}) {};
    //形如int a[10] = {...}; a[10][10]={...};
    ValueSymbol(std::string name_, std::vector<int> &dims_, std::vector<int> &initValues_, bool isConst_=false)
    : Symbol(name_), valueType(ARRAY), dims(dims_), initValues(initValues_), isConstValue(isConst_) {};
    bool isConst() const {
        return isConstValue;
    }
    void setBlockLevel(int level);
    void setInitValues(std::vector<int> x);
    int getFlattenDim();
    int getDim() const override;
};

#endif //BUAA_COMPILER_SYMBOL_H

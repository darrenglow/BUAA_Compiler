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
    POINTER,
    TEMP
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
    virtual int getSize() const = 0;
    std::string printMiddleCode();
};

class FuncFParamSymbol : public Symbol  {
public:
    std::vector<int> dims;
    ValueType valueType;
    FuncFParamSymbol(std::string &name_)
            : Symbol(name_), valueType(SINGLE) {};
    FuncFParamSymbol(std::string &name_, std::vector<int> &dims_)
        : Symbol(name_), dims(dims_), valueType(ARRAY) {};
    bool isConst() const;
    int getDim() const;
    int getSize() const;
};

class ValueSymbol;
class FuncSymbol : public Symbol {
public:
    int num;
    std::vector<ValueSymbol*> funcFParamSymbols;

    FuncSymbol(std::string &name, BasicType basicType_, int num_)
        : Symbol(name, basicType_), num(num_) {};
    void addFuncFParamSymbol(ValueSymbol* funcFParamSymbol) {
        funcFParamSymbols.push_back(funcFParamSymbol);
    }
    bool isConst() const;
    int getDim() const;
    int getSize() const;
};

class ValueSymbol : public Symbol {
public:
    ValueType valueType;
    std::vector<int> dims;   //具体维度
    std::vector<int> initValues; //展开后的一维值
    int initValue;
    bool isConstValue;
    int blockLevel;         //是在第几个嵌套括号中，全局变量就是0
    int address;
    bool isLocal=true;
    // 主要是用于临时变量的初始化
    ValueSymbol(std::string name_, ValueType valueType_, int initValue_=0, bool isConst_=false)
        : Symbol(name_), valueType(valueType_), initValue(initValue_), isConstValue(isConst_) {};
    //形如int a; int a=10; const int a = 10;
    ValueSymbol(std::string name_, int initValue_=0, bool isConst_=false)
    : Symbol(name_), valueType(SINGLE), initValue(initValue_), isConstValue(isConst_) {};
    //形如int a[10];
    ValueSymbol(std::string name_, std::vector<int> &dims_, bool isConst_=false)
    : Symbol(name_), valueType(ARRAY), dims(dims_), isConstValue(isConst_), initValues({0}) {};
    //形如int a[10] = {...}; a[10][10]={...};
    ValueSymbol(std::string name_, std::vector<int> &dims_, std::vector<int> &initValues_, bool isConst_=false)
    : Symbol(name_), valueType(ARRAY), dims(dims_), initValues(initValues_), isConstValue(isConst_) {};
    // 作为函数的参数
    ValueSymbol(ValueType valueType_, std::string name_, std::vector<int> &dims_, bool isConst_=false)
    : Symbol(name_), valueType(valueType_), dims(dims_), isConstValue(isConst_) {};

    bool isConst() const {
        return isConstValue;
    }
    void setBlockLevel(int level);
    void setInitValues(std::vector<int> x);
    int getFlattenDim() const;
    int getDim() const override;
    void setAddress(int addr);
    void setLocal(bool local);
    int getSize() const override;
    int getAddress();
};

// 生成mips时，发现会给常数分配一个寄存器，所以新建这个类
class NumSymbol : public Symbol {
public:
    int value;
    bool isConst() const;
    int getDim() const;
    int getSize() const;
    explicit NumSymbol(int value_, std::string name="", BasicType basicType=BasicType::INT) :
        Symbol(name, basicType), value(value_) {}
};

#endif //BUAA_COMPILER_SYMBOL_H

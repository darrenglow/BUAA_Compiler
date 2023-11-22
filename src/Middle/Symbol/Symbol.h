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
    POINTER,    // 函数的实参如果是指针的话，就设置为POINTER
    FUNCFPARAM, // 函数的形参，数组
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
    bool isFParam=false;    // 针对指针，判断其是否是函数的参数
    Intermediate *pAddress{}; // 针对传参时的指针，如果是二维数组的话a[2][2]，可能传入的是a[x]，这时其地址就要动态计算
    bool isAbsoluteAddress=false;  //仅针对func(int a[][2])调用func2(a[1])的特殊情况。
    bool isArrayElement=false;   // 仅针对a[2] = getint()这样的情况。
    // 主要用于指针，记录其偏移地址，和是否是全局的
    ValueSymbol(ValueType valueType_, std::string &name_, Intermediate *pAddress_, bool isLocal_, bool isConst_=true)
        : Symbol(name_), valueType(valueType_), isLocal(isLocal_), pAddress(pAddress_) {}
    ValueSymbol(bool isAbsolute_, ValueType valueType_, std::string &name_, Intermediate *pAddress_, bool isLocal_, bool isConst_=true)
            : Symbol(name_), valueType(valueType_), isLocal(isLocal_), pAddress(pAddress_), isAbsoluteAddress(isAbsolute_) {}
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
    Intermediate* getPAddress();
    void setFParam();
    void setArrayElement();
};

// 生成mips时，发现会给常数分配一个寄存器，所以新建这个类
class NumSymbol : public Symbol {
public:
    int value;
    bool isConst() const;
    int getDim() const;
    int getSize() const;
    explicit NumSymbol(int value_, std::string name="number", BasicType basicType=BasicType::INT) :
        Symbol(name, basicType), value(value_) {}
};

#endif //BUAA_COMPILER_SYMBOL_H

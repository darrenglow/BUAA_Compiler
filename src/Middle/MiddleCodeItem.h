//
// Created by 安达楷 on 2023/10/24.
//

#ifndef BUAA_COMPILER_MIDDLECODEITEM_H
#define BUAA_COMPILER_MIDDLECODEITEM_H

#define OVERRIDE_OUTPUT std::ostream& output(std::ostream &os) const override

#include <string>
#include <iostream>
#include <vector>
#include "Symbol/Symbol.h"
#include "Intermediate/Immediate.h"

enum MiddleCodeType {
    FUNC_BEGIN,
    FUNC_END,

    DEF_VAL,        // int a = 1;       DEF_VAL         1       int         a
    DEF_ARR_BEGIN,  // int a[10]={...}  DEF_ARR_BEGIN           int         a
                    //                  SET_VAL         1       int         a[0]
    DEF_ARR_END,    //                  DEF_ARR_END             int         a
    SET_VAL,        // a = 1;           SET_VAL         1       int         a
                    // a[0] = 1;        SET_VAL         1       int         a[0]

};

class MiddleCodeItem {
public:
    MiddleCodeItem()=default;
    virtual std::ostream& output(std::ostream &os) const = 0;
    friend std::ostream& operator<<(std::ostream& os, const MiddleCodeItem& obj);
};

class Label : public MiddleCodeItem {
public:
    static int count;
    std::string label;
    explicit Label() : label("LABEL_" + std::to_string(count)) {
        count ++ ;
    }
    OVERRIDE_OUTPUT;
};


class BasicBlock : public MiddleCodeItem {
public:
    enum Type{
        FUNC,
        BLOCK,
        LOOP,
        BRANCH
    };
    Type type;
    Label *label;
    std::string type2str[5] = {"BLOCK_FUNC", "BLOCK_BLOCK", "BLOCK_LOOP", "BLOCK_BRANCH"};
    std::vector<MiddleCodeItem*> middleCodeItems;

    explicit BasicBlock(Type type_) : type(type_), label(new Label()) {}

    void add(MiddleCodeItem *middleCodeItem);
    OVERRIDE_OUTPUT;
};

class Func : public MiddleCodeItem {
public:
    std::string funcName;
    BasicBlock *block{};

    explicit Func(std::string &funcName_) :
        funcName(funcName_) {}

    void setFuncBlock(BasicBlock* block);

    OVERRIDE_OUTPUT;
};

// int a = 1;
// int a[10];
class MiddleDef : public MiddleCodeItem {
public:
    enum Type{
        DEF_VAR,
        DEF_ARRAY,
        END_ARRAY
    };
    std::string type2str[4] = {"DEF_VAR", "DEF_ARRAY", "END_ARRAY"};
    Type type;
    bool isInit;

    ValueSymbol *valueSymbol;
    Intermediate *srcValueSymbol;   // src可能是立即数，可能是变量名

    MiddleDef(Type type_, ValueSymbol *valueSymbol_) : type(type_), valueSymbol(valueSymbol_) {}
    MiddleDef(Type type_, ValueSymbol *valueSymbol_, Intermediate *srcValueSymbol_) : type(type_), srcValueSymbol(srcValueSymbol_), valueSymbol(valueSymbol_), isInit(true) {}
    OVERRIDE_OUTPUT;
};

// a = 1;
// ASSIGN 1 a
// POSITIVE a T1
class MiddleUnaryOp : public MiddleCodeItem {
public:
    enum Type{
        ASSIGN,
        POSITIVE,
        NEGATIVE,
        NOT,
        ERROR,
    };
    Type type;
    Intermediate *valueSymbol;
    Intermediate *srcValueSymbol;   // srcValueSymbol可能是值，可能是变量

    std::string type2str[8] = {"ASSIGN", "POSITIVE", "NEGATIVE", "NOT", "ERROR"};
    // op 1 a
    // op a a
    MiddleUnaryOp(Type type_, Intermediate *valueSymbol_, Intermediate *srcValueSymbol_) :
        type(type_), valueSymbol(valueSymbol_), srcValueSymbol(srcValueSymbol_) {}

    OVERRIDE_OUTPUT;
};



class MiddleOffset : public MiddleCodeItem {
public:
    int offset;
    ValueSymbol *src;
    ValueSymbol *ret;
    MiddleOffset(ValueSymbol *src_, int offset_, ValueSymbol *ret_) :
        src(src_), offset(offset_), ret(ret_) {}
    OVERRIDE_OUTPUT;
};

class MiddleMemoryOp : public MiddleCodeItem {
public:
    enum Type{
        STORE,
        LOAD
    };
    std::string type2str[2] = {"STORE", "LOAD"};
    Type type;
    Intermediate *sym1;
    Intermediate *sym2;
    // STORE sym1/value sym2
    MiddleMemoryOp(Type type_, Intermediate *sym1_, ValueSymbol *sym2_) : type(type_), sym1(sym1_), sym2(sym2_) {}
    OVERRIDE_OUTPUT;
};

class MiddleBinaryOp : public MiddleCodeItem {
public:
    enum Type{
        ADD,
        SUB,
        MUL,
        DIV,
        MOD,
        ERROR,
    };
    std::string type2str[10] = {"ADD", "SUB", "MUL", "DIV", "MOD", "ERROR"};
    Type type;

    Intermediate *src1;
    Intermediate *src2;
    Intermediate *target;

    MiddleBinaryOp(Type type_, Intermediate *src1_, Intermediate *src2_, Intermediate *target_)
        : type(type_), src1(src1_), src2(src2_), target(target_) {}
    OVERRIDE_OUTPUT;
};

#endif //BUAA_COMPILER_MIDDLECODEITEM_H

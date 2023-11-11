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
#include "Symbol/SymbolTable.h"

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
    Label() : label("LABEL_" + std::to_string(count)) {
        count ++ ;
    }
    explicit Label(std::string &label_) : label(label_) {}
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
    Label *label;   // FUNC, LOOP, BRANCH
    std::string type2str[4] = {"BLOCK_FUNC", "BLOCK_BLOCK", "BLOCK_LOOP", "BLOCK_BRANCH"};
    std::vector<MiddleCodeItem*> middleCodeItems;
    std::vector<ValueSymbol*> tempSymbols;  // 临时变量

    explicit BasicBlock(Type type_) : type(type_), label(new Label()) {}

    void add(MiddleCodeItem *middleCodeItem);
    Label * getLabel();
    OVERRIDE_OUTPUT;
};

class Func : public MiddleCodeItem {
public:
    enum Type{
        DEF_FUNC,
    };

    std::string funcName;
    BasicBlock *block{};
    SymbolTable * funcSymbolTable = new SymbolTable();
    Label *label;
    Type type;
    bool hasReturn;
    explicit Func(Type type_, std::string &funcName_) : type(type_), funcName(funcName_) {
        std::string tmp = "Func_" + funcName;
        label = new Label(tmp);
    }

    void setFuncBlock(BasicBlock* block);
    int getSize();
    OVERRIDE_OUTPUT;
};

class MiddleFuncCall : public MiddleCodeItem {
public:
    std::string funcName;
    std::vector<Intermediate*> funcRParams;
    ValueSymbol *ret;
    MiddleFuncCall(std::string &funcName_) : funcName(funcName_) {}
    OVERRIDE_OUTPUT;
};

class MiddleFuncParam : public MiddleCodeItem {
public:
    enum Type{
        PARAM,
        PUSH_PARAM,
    };
    std::string type2str[3] = {"PARAM", "PUSH_PARAM"};
    Type type;
    Intermediate * target;
    MiddleFuncParam(Type type_, Intermediate * target_)
        : type(type_), target(target_) {}

    OVERRIDE_OUTPUT;
};

class MiddleReturn : public MiddleCodeItem {
public:
    Intermediate * target{};
    MiddleReturn() {}
    explicit MiddleReturn(Intermediate* target_) : target(target_) {}
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
    std::string type2str[3] = {"DEF_VAR", "DEF_ARRAY", "END_ARRAY"};
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

    std::string type2str[5] = {"ASSIGN", "POSITIVE", "NEGATIVE", "NOT", "ERROR"};
    // op 1 a
    // op a a
    MiddleUnaryOp(Type type_, Intermediate *valueSymbol_, Intermediate *srcValueSymbol_) :
        type(type_), valueSymbol(valueSymbol_), srcValueSymbol(srcValueSymbol_) {}

    OVERRIDE_OUTPUT;
};

class MiddleOffset : public MiddleCodeItem {
public:
    Intermediate *offset;
    Intermediate *src;
    Intermediate *ret;
    MiddleOffset(Intermediate *src_, Intermediate *offset_, Intermediate *ret_) :
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
    MiddleMemoryOp(Type type_, Intermediate *sym1_, Intermediate *sym2_) : type(type_), sym1(sym1_), sym2(sym2_) {}
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
        EQ,
        NE,
        GT,
        GE,
        LT,
        LE,
        ERROR,
    };
    std::string type2str[12] = {"ADD", "SUB", "MUL", "DIV", "MOD", "EQ", "NE", "GT", "GE", "LT", "LE", "ERROR"};
    Type type;

    Intermediate *src1;
    Intermediate *src2;
    Intermediate *target;
    MiddleBinaryOp(Type type_, Intermediate *src1_, Intermediate *src2_, Intermediate *target_)
        : type(type_), src1(src1_), src2(src2_), target(target_) {}
    OVERRIDE_OUTPUT;
};

// JUMP_EQZ a label
class MiddleJump : public MiddleCodeItem {
public:
    enum Type {
        JUMP,
        JUMP_EQZ,
        JUMP_NEZ
    };
    std::string type2str[3] = {"JUMP", "JUMP_EQZ", "JUMP_NEZ"};
    Type type;
    Intermediate *src;
    Label *label;
    MiddleJump(Type type_, Label *label_)
        : type(type_), label(label_), src(nullptr){}
    MiddleJump(Type type_, Intermediate *src_, Label *label_)
        : type(type_), src(src_), label(label_) {}

    OVERRIDE_OUTPUT;
};

class MiddleIO : public MiddleCodeItem {
public:
    enum Type {
        GETINT,
        PRINT_INT,
        PRINT_STR
    };
    std::string type2str[3] = {"GETINT", "PRINT_INT", "PRINT_STR"};
    Type type;
    Intermediate * target;
    MiddleIO(Type type_, Intermediate *target_) : type(type_), target(target_) {}
    OVERRIDE_OUTPUT;
};

#endif //BUAA_COMPILER_MIDDLECODEITEM_H

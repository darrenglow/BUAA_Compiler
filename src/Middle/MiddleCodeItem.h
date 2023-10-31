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

// int a = 1;   int a;
// ASSIGN 1 a
class MiddleUnaryOp : public MiddleCodeItem {
public:
    enum Type{
        ASSIGN,
    };
    Type type;
    ValueSymbol *valueSymbol;
    int value;
    std::string type2str[2] = {"ASSIGN"};
    MiddleUnaryOp(Type type_, ValueSymbol *valueSymbol_, int value_=0) :
        type(type_), valueSymbol(valueSymbol_), value(value_) {}

    OVERRIDE_OUTPUT;
};

#endif //BUAA_COMPILER_MIDDLECODEITEM_H

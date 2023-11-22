//
// Created by 安达楷 on 2023/10/24.
//

#ifndef BUAA_COMPILER_MIDDLECODEITEM_H
#define BUAA_COMPILER_MIDDLECODEITEM_H

#define OVERRIDE_OUTPUT std::ostream& output(std::ostream &os) const override

#include <string>
#include <iostream>
#include <vector>
#include "../Symbol/Symbol.h"
#include "../Intermediate/Immediate.h"
#include "../Symbol/SymbolTable.h"
#include <set>

class MiddleCodeItem {
public:
    enum Type {
        BasicBlock, Func, Label, MiddleBinaryOp, MiddleDef, MiddleFuncCall, MiddleIO,
        MiddleJump, MiddleMemoryOp, MiddleOffset, MiddleReturn, MiddleUnaryOp, PushParam
    };
    Type codeType {};

    MiddleCodeItem()=default;
    explicit MiddleCodeItem(Type codeType_) : codeType(codeType_) {}

    std::vector<int> killSetIndex;
    int index{};

    void setIndex(int x);

    virtual Intermediate* _getSrc1();
    virtual Intermediate* _getSrc2();
    virtual Intermediate* _getRet();

    ValueSymbol *getDef();
    std::set<ValueSymbol*> *getUse();
    ValueSymbol *getGen();
    bool isNotArrayOrPointerElement(const std::string& name);
    virtual std::ostream& output(std::ostream &os) const = 0;
    friend std::ostream& operator<<(std::ostream& os, const MiddleCodeItem& obj);
};

struct CompareMiddleCodeItem {
    bool operator()(const MiddleCodeItem* a, const MiddleCodeItem* b) const {
        return a->index < b->index;
    }
};
#endif //BUAA_COMPILER_MIDDLECODEITEM_H

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

    virtual Intermediate** getPointerToSrc1();
    virtual Intermediate** getPointerToSrc2();
    virtual Intermediate** getPointerToRet();

    void reset(Intermediate *prev, Intermediate *now);
    void setCodeType(Type codeType_);
    void setSrc1(Intermediate* src1_);
    void setSrc2(Intermediate* src2_);
    void setRet(Intermediate* ret_);
    void clearSrc1();
    void clearSrc2();
};

struct CompareMiddleCodeItem {
    bool operator()(const MiddleCodeItem* a, const MiddleCodeItem* b) const {
        return a->index < b->index;
    }
};
#endif //BUAA_COMPILER_MIDDLECODEITEM_H

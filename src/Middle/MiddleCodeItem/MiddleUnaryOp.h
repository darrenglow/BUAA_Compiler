//
// Created by 安达楷 on 2023/11/19.
//

#ifndef BUAA_COMPILER_MIDDLEUNARYOP_H
#define BUAA_COMPILER_MIDDLEUNARYOP_H
#include "MiddleCodeItem.h"
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

    Intermediate * getLeftIntermediate() override;
    Intermediate * getRightIntermediate1() override;
    OVERRIDE_OUTPUT;
};
#endif //BUAA_COMPILER_MIDDLEUNARYOP_H

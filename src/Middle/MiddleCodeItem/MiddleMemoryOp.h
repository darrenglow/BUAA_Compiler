//
// Created by 安达楷 on 2023/11/19.
//

#ifndef BUAA_COMPILER_MIDDLEMEMORYOP_H
#define BUAA_COMPILER_MIDDLEMEMORYOP_H
#include "MiddleCodeItem.h"
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
    Intermediate * getLeftIntermediate() override;
    Intermediate * getRightIntermediate1() override;
    OVERRIDE_OUTPUT;
};
#endif //BUAA_COMPILER_MIDDLEMEMORYOP_H

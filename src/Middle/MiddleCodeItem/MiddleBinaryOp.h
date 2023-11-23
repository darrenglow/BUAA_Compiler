//
// Created by 安达楷 on 2023/11/19.
//

#ifndef BUAA_COMPILER_MIDDLEBINARYOP_H
#define BUAA_COMPILER_MIDDLEBINARYOP_H
#include "MiddleCodeItem.h"
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

    Intermediate *src1{};
    Intermediate *src2{};
    Intermediate *target{};
    MiddleBinaryOp(Type type_, Intermediate *src1_, Intermediate *src2_, Intermediate *target_)
            : MiddleCodeItem(MiddleCodeItem::MiddleBinaryOp), type(type_), src1(src1_), src2(src2_), target(target_) {}
    Intermediate * _getRet() override;
    Intermediate * _getSrc1() override;
    Intermediate * _getSrc2() override;

    Intermediate** getPointerToSrc1() override;
    Intermediate** getPointerToSrc2() override;
    Intermediate** getPointerToRet() override;

    OVERRIDE_OUTPUT;
};
#endif //BUAA_COMPILER_MIDDLEBINARYOP_H

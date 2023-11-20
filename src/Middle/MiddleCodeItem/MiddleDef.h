//
// Created by 安达楷 on 2023/11/19.
//

#ifndef BUAA_COMPILER_MIDDLEDEF_H
#define BUAA_COMPILER_MIDDLEDEF_H
#include "MiddleCodeItem.h"

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
    bool isInit{};

    ValueSymbol *valueSymbol;
    Intermediate *srcValueSymbol{};   // src可能是立即数，可能是变量名

    MiddleDef(Type type_, ValueSymbol *valueSymbol_) : type(type_), valueSymbol(valueSymbol_) {}
    MiddleDef(Type type_, ValueSymbol *valueSymbol_, Intermediate *srcValueSymbol_) : type(type_), srcValueSymbol(srcValueSymbol_), valueSymbol(valueSymbol_), isInit(true) {}

    Intermediate * getLeftIntermediate() override;
    Intermediate * getRightIntermediate1() override;
    OVERRIDE_OUTPUT;
};
#endif //BUAA_COMPILER_MIDDLEDEF_H

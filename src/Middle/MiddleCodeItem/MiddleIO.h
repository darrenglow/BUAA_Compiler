//
// Created by 安达楷 on 2023/11/19.
//

#ifndef BUAA_COMPILER_MIDDLEIO_H
#define BUAA_COMPILER_MIDDLEIO_H
#include "MiddleCodeItem.h"


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
    MiddleIO(Type type_, Intermediate *target_) : MiddleCodeItem(MiddleCodeItem::MiddleIO), type(type_), target(target_) {}
    Intermediate * _getRet() override;
    Intermediate * _getSrc1() override;
    Intermediate** getPointerToSrc1() override;
    Intermediate** getPointerToRet() override;
    OVERRIDE_OUTPUT;
};

#endif //BUAA_COMPILER_MIDDLEIO_H

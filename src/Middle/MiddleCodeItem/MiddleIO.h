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
    MiddleIO(Type type_, Intermediate *target_) : type(type_), target(target_) {}
    Intermediate * getLeftIntermediate() override;
    OVERRIDE_OUTPUT;
};

#endif //BUAA_COMPILER_MIDDLEIO_H

//
// Created by 安达楷 on 2023/11/1.
//

#ifndef BUAA_COMPILER_IMMEDIATE_H
#define BUAA_COMPILER_IMMEDIATE_H

#include "Intermediate.h"

class Immediate : public Intermediate{
public:
    enum Type {
        VALUE,
        STR
    };
    Type type;
    int value;
    std::string str;
    Immediate(int value_) : value(value_), type(VALUE) {}
    Immediate(std::string &str_) : str(str_), type(STR) {}
    std::string printMiddleCode();
};

#endif //BUAA_COMPILER_IMMEDIATE_H

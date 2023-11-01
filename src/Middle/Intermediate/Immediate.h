//
// Created by 安达楷 on 2023/11/1.
//

#ifndef BUAA_COMPILER_IMMEDIATE_H
#define BUAA_COMPILER_IMMEDIATE_H

#include "Intermediate.h"

class Immediate : public Intermediate{
public:
    explicit Immediate(int value_) : value(value_) {}
    int value;
};

#endif //BUAA_COMPILER_IMMEDIATE_H

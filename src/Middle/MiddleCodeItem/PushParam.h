//
// Created by 安达楷 on 2023/11/20.
//

#ifndef BUAA_COMPILER_PUSHPARAM_H
#define BUAA_COMPILER_PUSHPARAM_H

#include "MiddleCodeItem.h"
class PushParam : public MiddleCodeItem {
public:
    Intermediate *param;
    explicit PushParam(Intermediate *param_) : MiddleCodeItem(MiddleCodeItem::PushParam), param(param_) {};
    OVERRIDE_OUTPUT;
    Intermediate * _getSrc1() override;
};

#endif //BUAA_COMPILER_PUSHPARAM_H

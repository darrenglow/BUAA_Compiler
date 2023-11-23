//
// Created by 安达楷 on 2023/11/19.
//

#ifndef BUAA_COMPILER_MIDDLERETURN_H
#define BUAA_COMPILER_MIDDLERETURN_H
#include "MiddleCodeItem.h"

class MiddleReturn : public MiddleCodeItem {
public:
    Intermediate * target{};
    MiddleReturn(): MiddleCodeItem(MiddleCodeItem::MiddleReturn) {} ;
    explicit MiddleReturn(Intermediate* target_) : MiddleCodeItem(MiddleCodeItem::MiddleReturn), target(target_) {}
    Intermediate * _getSrc1() override;
    Intermediate** getPointerToSrc1() override;
    OVERRIDE_OUTPUT;
};
#endif //BUAA_COMPILER_MIDDLERETURN_H

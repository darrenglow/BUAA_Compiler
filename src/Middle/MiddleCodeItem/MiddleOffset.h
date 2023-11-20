//
// Created by 安达楷 on 2023/11/19.
//

#ifndef BUAA_COMPILER_MIDDLEOFFSET_H
#define BUAA_COMPILER_MIDDLEOFFSET_H
#include "MiddleCodeItem.h"

class MiddleOffset : public MiddleCodeItem {
public:
    Intermediate *offset;
    Intermediate *src;
    Intermediate *ret;
    MiddleOffset(Intermediate *src_, Intermediate *offset_, Intermediate *ret_) :
            src(src_), offset(offset_), ret(ret_) {}
    Intermediate * getLeftIntermediate() override;
    OVERRIDE_OUTPUT;
};
#endif //BUAA_COMPILER_MIDDLEOFFSET_H

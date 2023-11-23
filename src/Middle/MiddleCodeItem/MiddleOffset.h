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
            MiddleCodeItem(MiddleCodeItem::MiddleOffset), src(src_), offset(offset_), ret(ret_) {}
    Intermediate * _getRet() override;
    Intermediate * _getSrc1() override;
    Intermediate * _getSrc2() override;
    Intermediate** getPointerToSrc1() override;
    Intermediate** getPointerToRet() override;
    Intermediate** getPointerToSrc2() override;
    OVERRIDE_OUTPUT;
};
#endif //BUAA_COMPILER_MIDDLEOFFSET_H

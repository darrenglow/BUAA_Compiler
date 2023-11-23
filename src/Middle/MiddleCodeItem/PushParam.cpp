//
// Created by 安达楷 on 2023/11/20.
//

#include "PushParam.h"
std::ostream & PushParam::output(std::ostream &os) const {
    os << "PUSH_PARAM " << param->printMiddleCode();
    return os;
}

Intermediate * PushParam::_getSrc1() {
    // 如果PushParam的是数组元素或者是指针，不纳入数据流的考虑范围内
//    if (dynamic_cast<ValueSymbol *>(param) &&
//        (dynamic_cast<ValueSymbol *>(param)->name.rfind("ArraY_*|!123___", 0) == 0 ||
//         dynamic_cast<ValueSymbol *>(param)->name.rfind("PointeR_*|!123___", 0) == 0))
//        return nullptr;
    return param;
}

Intermediate **PushParam::getPointerToSrc1() {
    return &param;
}
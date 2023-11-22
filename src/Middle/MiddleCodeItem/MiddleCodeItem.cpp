//
// Created by 安达楷 on 2023/10/24.
//

#include "MiddleCodeItem.h"
#include "../Visitor/Visitor.h"
#include <iostream>
#include <set>

std::ostream& operator<<(std::ostream& os, const MiddleCodeItem& obj) {
    return obj.output(os);
}

Intermediate * MiddleCodeItem::_getRet() {
    return nullptr;
}

Intermediate * MiddleCodeItem::_getSrc1() {
    return nullptr;
}
Intermediate * MiddleCodeItem::_getSrc2() {
    return nullptr;
}

void MiddleCodeItem::setIndex(int x) {
    index = x;
}

ValueSymbol * MiddleCodeItem::getDef() {
    // 定义的话，只考虑二元计算、一元计算、定义、输入，LOAD中的变量
    if (this == nullptr) return nullptr;
    if (this->codeType == MiddleBinaryOp || this->codeType == MiddleUnaryOp
        || (this->codeType == MiddleCodeItem::MiddleDef && dynamic_cast<class MiddleDef*>(this)->type == MiddleDef::DEF_VAR)
        || (this->codeType == MiddleIO && dynamic_cast<class MiddleIO*>(this)->type == MiddleIO::GETINT)
        || (this->codeType == MiddleMemoryOp && dynamic_cast<class MiddleMemoryOp*>(this)->type == MiddleMemoryOp::LOAD)) {
        auto ret = this->_getRet();
        if (dynamic_cast<ValueSymbol*>(ret) && dynamic_cast<ValueSymbol*>(ret)->getDim() == 0 &&
                isNotArrayOrPointerElement(dynamic_cast<ValueSymbol*>(ret)->name))
            return dynamic_cast<ValueSymbol*>(ret);
    }
    return nullptr;
}
// OFFSET i a T0
//
std::set<ValueSymbol *>* MiddleCodeItem::getUse() {
    // 使用的话，只考虑二元计算，一元计算、输出，函数调用中的参数，数组中的变量，memoryOp的操作数
    auto set = new std::set<ValueSymbol*>();

    if (this->codeType == MiddleBinaryOp || this->codeType == MiddleUnaryOp
        || (this->codeType == MiddleDef && dynamic_cast<class MiddleDef*>(this)->type == MiddleDef::DEF_VAR)
        || (this->codeType == MiddleIO && dynamic_cast<class MiddleIO*>(this)->type == MiddleIO::PRINT_INT)
        || (this->codeType == MiddleJump && dynamic_cast<class MiddleJump*>(this)->type == MiddleJump::JUMP_EQZ)
        || this->codeType == PushParam
        || this->codeType == MiddleOffset
        || (this->codeType == MiddleMemoryOp && dynamic_cast<class MiddleMemoryOp*>(this)->type == MiddleMemoryOp::STORE)
        || (this->codeType == MiddleReturn)) {
        auto src1 = _getSrc1();
        if (dynamic_cast<ValueSymbol*>(src1) != nullptr && dynamic_cast<ValueSymbol*>(src1)->getDim() == 0
            && isNotArrayOrPointerElement(dynamic_cast<ValueSymbol*>(src1)->name)) {
            set->insert(dynamic_cast<ValueSymbol*>(src1));
        }
        auto src2 = _getSrc2();
        if (dynamic_cast<ValueSymbol*>(src2) != nullptr && dynamic_cast<ValueSymbol*>(src2)->getDim() == 0
            && isNotArrayOrPointerElement(dynamic_cast<ValueSymbol*>(src2)->name)) {
            set->insert(dynamic_cast<ValueSymbol*>(src2));
        }
    }
    return set;
}

ValueSymbol * MiddleCodeItem::getGen() {
    // 定义的话，只考虑二元计算、一元计算、定义、输入
    if (this->codeType == MiddleBinaryOp || this->codeType == MiddleUnaryOp
        || (this->codeType == MiddleDef && dynamic_cast<class MiddleDef*>(this)->type == MiddleDef::DEF_VAR)
        || (this->codeType == MiddleIO && dynamic_cast<class MiddleIO*>(this)->type == MiddleIO::GETINT)) {
        auto ret = this->_getRet();
        if (dynamic_cast<ValueSymbol*>(ret) && dynamic_cast<ValueSymbol*>(ret)->getDim() == 0)
            return dynamic_cast<ValueSymbol*>(ret);
    }
    return nullptr;
}

bool MiddleCodeItem::isNotArrayOrPointerElement(const std::string& name) {
    return name.rfind("ArraY_*$+|!123___", 0) != 0 && name.rfind("PointeR_*$+|!123___", 0) != 0;
}
//
// Created by 安达楷 on 2023/10/24.
//

#include "MiddleCodeItem.h"
#include "Visitor/Visitor.h"
#include <iostream>

std::ostream& operator<<(std::ostream& os, const MiddleCodeItem& obj) {
    return obj.output(os);
}

int Label::count = 1;
std::ostream & Label::output(std::ostream &os) const {
    os << label << ":" << std::endl;
    return os;
}

// BasicBlock
void BasicBlock::add(MiddleCodeItem *middleCodeItem) {
    middleCodeItems.push_back(middleCodeItem);
}
std::ostream & BasicBlock::output(std::ostream &os) const {
    os << "### " << type2str[type] << " BEGIN" << std::endl;
    os << *label;
    for (auto middleCodeItem : middleCodeItems) {
        os << *middleCodeItem;
    }
    os << "### " << type2str[type] << " END" << std::endl;
    return os;
}
Label * BasicBlock::getLabel() {
    return label;
}

// Func
std::ostream & Func::output(std::ostream &os) const {
    if (type == DEF_FUNC) {
        os << "###### BEGIN_" << funcName << " ######" << std::endl;
        os << *block;
        os << "###### END_" << funcName << " ######" << std::endl;
    }
    else if (type == CALL) {
        os << "CALL " << funcName << std::endl;
    }
    return os;
}
void Func::setFuncBlock(BasicBlock *basicBlock) {
    block = basicBlock;
}

// FuncCall
std::ostream & MiddleFuncCall::output(std::ostream &os) const {
    os << type2str[type]  << " " << target->printMiddleCode() << std::endl;
    return os;
}

// Def
std::ostream & MiddleDef::output(std::ostream &os) const {
    os << type2str[type] << " ";
    if (isInit) {
        os << srcValueSymbol->printMiddleCode() << " ";
    }
    os << valueSymbol->name << std::endl;
    return os;
}


// UnaryOp
std::ostream & MiddleUnaryOp::output(std::ostream &os) const {
    os << type2str[type] << " ";
    os << srcValueSymbol->printMiddleCode() << " ";
    os << valueSymbol->printMiddleCode() << std::endl;

    return os;
}

// Offset
std::ostream & MiddleOffset::output(std::ostream &os) const {
    // 如果是立即数的话，就是offset * 4
    os << "OFFSET " << offset->printMiddleCode() << " " << src->printMiddleCode() << " " << ret->printMiddleCode() << std::endl;
    return os;
}

// MemoryOp
std::ostream & MiddleMemoryOp::output(std::ostream &os) const {
    os << type2str[type] << " ";
    os << sym1->printMiddleCode() << " ";
    os << sym2->printMiddleCode() << std::endl;
    return os;
}

// BinaryOp
std::ostream & MiddleBinaryOp::output(std::ostream &os) const {
    os << type2str[type] << " ";
    os << src1->printMiddleCode() << " ";
    os << src2->printMiddleCode() << " ";
    os << target->printMiddleCode() << std::endl;
    return os;
}

// MiddleJump
std::ostream & MiddleJump::output(std::ostream &os) const {
    os << type2str[type] << " ";
    if (src != nullptr)
        os << src->printMiddleCode() << " ";
    os << label->label << std::endl;
    return os;
}

// MiddleIO
std::ostream & MiddleIO::output(std::ostream &os) const {
    os << type2str[type] << " " << target->printMiddleCode() << std::endl;
    return os;
}
//
// Created by 安达楷 on 2023/10/24.
//

#include "MiddleCodeItem.h"
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

// Func
std::ostream & Func::output(std::ostream &os) const {
    os << "###### BEGIN_" << funcName << " ######" << std::endl;
    os << *block;
    os << "###### END_" << funcName << " ######" << std::endl;
    return os;
}
void Func::setFuncBlock(BasicBlock *basicBlock) {
    block = basicBlock;
}

// Def
std::ostream & MiddleDef::output(std::ostream &os) const {
    os << type2str[type] << " ";
    if (isInit) {
        os << " " << srcValueSymbol->printMiddleCode() << " ";
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
    os << "OFFSET " << offset << " " << src->name << " " << ret->name << std::endl;
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
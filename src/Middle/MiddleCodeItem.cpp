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


std::ostream & MiddleUnaryOp::output(std::ostream &os) const {
    os << type2str[type] << " " << value << " " << valueSymbol->name << std::endl;
    return os;
}
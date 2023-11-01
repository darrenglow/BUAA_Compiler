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
        if (dynamic_cast<ValueSymbol*>(srcValueSymbol) != nullptr) {
            os << dynamic_cast<ValueSymbol*>(srcValueSymbol)->name << " ";
        }
        else if (dynamic_cast<Immediate*>(srcValueSymbol) != nullptr)
            os << dynamic_cast<Immediate*>(srcValueSymbol)->value << " ";
        else {
            std::cout << "[MiddleDef output] ERROR" << std::endl;
        }
    }
    os << valueSymbol->name << std::endl;
    return os;
}


// UnaryOp
std::ostream & MiddleUnaryOp::output(std::ostream &os) const {
    os << type2str[type] << " ";

    if (dynamic_cast<ValueSymbol*>(srcValueSymbol) != nullptr) {
        os << dynamic_cast<ValueSymbol*>(srcValueSymbol)->name << " ";
    }
    else if (dynamic_cast<Immediate*>(srcValueSymbol) != nullptr)
        os << dynamic_cast<Immediate*>(srcValueSymbol)->value << " ";
    else {
        std::cout << "[MiddleDef output] ERROR" << std::endl;
    }

    if (dynamic_cast<ValueSymbol*>(valueSymbol) != nullptr) {
        os << dynamic_cast<ValueSymbol*>(valueSymbol)->name << " " << std::endl;
    }
    else {
        std::cout << "[MiddleUnaryOp output] ERROR" << std::endl;
    }
    return os;
}

// Offset
std::ostream & MiddleOffset::output(std::ostream &os) const {
    os << "OFFSET " << offset << " " << src->name << " " << ret->name << std::endl;
    return os;
}

// MemoryOp
std::ostream & MiddleMemoryOp::output(std::ostream &os) const {
    if (sym1 == nullptr)
        os << type2str[type] << " " << value << " " << sym2->name << std::endl;
    else
        os << type2str[type] << " " << sym1->name << " " << sym2->name << std::endl;
    return os;
}

// BinaryOp
std::ostream & MiddleBinaryOp::output(std::ostream &os) const {
    os << type2str[type] << " ";
    if (dynamic_cast<Immediate*>(src1) != nullptr) {
        os << dynamic_cast<Immediate*>(src1)->value << " ";
    }
    else {
        os << dynamic_cast<ValueSymbol*>(src1)->name << " ";
    }
    if (dynamic_cast<Immediate*>(src2) != nullptr) {
        os << dynamic_cast<Immediate*>(src2)->value << " ";
    }
    else {
        os << dynamic_cast<ValueSymbol*>(src2)->name << " ";
    }
    if (dynamic_cast<Immediate*>(target) != nullptr) {
        os << dynamic_cast<Immediate*>(target)->value << std::endl;
    }
    else {
        os << dynamic_cast<ValueSymbol*>(target)->name << std::endl;
    }
    return os;
}
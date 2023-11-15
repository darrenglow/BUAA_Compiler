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
    os << label << ":";
    return os;
}

// BasicBlock
void BasicBlock::add(MiddleCodeItem *middleCodeItem) {
    middleCodeItems.push_back(middleCodeItem);
}
std::ostream & BasicBlock::output(std::ostream &os) const {
    os << "### " << type2str[type] << " BEGIN" << std::endl;
    os << *label << std::endl;
    for (auto middleCodeItem : middleCodeItems) {
        os << *middleCodeItem << std::endl;
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
        os << "###### func_size is " << funcSymbolTable->getSize() << "######" << std::endl;
        os << *label << std::endl;
        os << *block;
        os << "###### END_" << funcName << " ######" << std::endl;
    }
    return os;
}
void Func::setFuncBlock(BasicBlock *basicBlock) {
    block = basicBlock;
}
int Func::getSize() {
    return funcSymbolTable->getSize();
}

// MiddleFuncCall
std::ostream & MiddleFuncCall::output(std::ostream &os) const {
    os << "CALL " << funcName << "( ";
    for (auto x : funcRParams) {
        os << x->printMiddleCode() << " ";
    }
    os << ")" << std::endl;
    return os;
}

// FuncParam
std::ostream & MiddleFuncParam::output(std::ostream &os) const {
    os << type2str[type]  << " " << target->printMiddleCode();
    return os;
}

// return
std::ostream & MiddleReturn::output(std::ostream &os) const {
    os << "return ";
    if (target != nullptr) {
        os << target->printMiddleCode();
    }
    return os;
}


// Def
std::ostream & MiddleDef::output(std::ostream &os) const {
    os << type2str[type] << " ";
    if (isInit && type == DEF_VAR && srcValueSymbol != nullptr) {
        //os << srcValueSymbol->printMiddleCode() << " ";
    }
    os << valueSymbol->name << "[0x" << std::hex << valueSymbol->getAddress() << "]";
    return os;
}


// UnaryOp
std::ostream & MiddleUnaryOp::output(std::ostream &os) const {
    os << type2str[type] << " ";
    os << srcValueSymbol->printMiddleCode() << " ";
    os << valueSymbol->printMiddleCode();

    return os;
}

// Offset
std::ostream & MiddleOffset::output(std::ostream &os) const {
    // 如果是立即数的话，就是offset * 4
    os << "OFFSET " << offset->printMiddleCode() << " " << src->printMiddleCode() << " " << ret->printMiddleCode();
    return os;
}

// MemoryOp
std::ostream & MiddleMemoryOp::output(std::ostream &os) const {
    os << type2str[type] << " ";
    os << sym1->printMiddleCode() << " ";
    os << sym2->printMiddleCode();
    return os;
}

// BinaryOp
std::ostream & MiddleBinaryOp::output(std::ostream &os) const {
    os << type2str[type] << " ";
    os << src1->printMiddleCode() << " ";
    os << src2->printMiddleCode() << " ";
    os << target->printMiddleCode();
    return os;
}

// MiddleJump
std::ostream & MiddleJump::output(std::ostream &os) const {
    os << type2str[type] << " ";
    if (src != nullptr)
        os << src->printMiddleCode() << " ";
    os << label->label;
    return os;
}

// MiddleIO
std::ostream & MiddleIO::output(std::ostream &os) const {
    os << type2str[type] << " " << target->printMiddleCode();
    return os;
}
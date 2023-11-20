//
// Created by 安达楷 on 2023/11/19.
//
#include "Middle.h"
// BasicBlock
int BasicBlock::blockID = 0;
void BasicBlock::add(MiddleCodeItem *middleCodeItem) {
    middleCodeItems.push_back(middleCodeItem);
}
std::ostream & BasicBlock::output(std::ostream &os) const {
    os << "### " << type2str[type] << " [" << std::dec << basicBlockID << "] BEGIN" << std::endl;
    os << *label << std::endl;
    for (auto middleCodeItem : middleCodeItems) {
        os << *middleCodeItem << std::endl;
    }
    os << "### " << type2str[type] << " [" << std::dec << basicBlockID << "] END" << std::endl;
    return os;
}
Label * BasicBlock::getLabel() {
    return label;
}
void BasicBlock::setNext(BasicBlock *next) {
    follows.push_back(next);
    next->prevs.push_back(this);
}

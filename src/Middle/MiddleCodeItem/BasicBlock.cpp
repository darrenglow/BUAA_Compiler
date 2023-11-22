//
// Created by 安达楷 on 2023/11/19.
//
#include "Middle.h"
#include "BasicBlock.h"

// BasicBlock
int BasicBlock::blockID = 0;


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

void BasicBlock::calcDefAndUse() {
    this->defSet->clear();
    this->useSet->clear();
    for (auto code : this->middleCodeItems) {
        auto def = code->getDef();
        if (def != nullptr) {
            auto name = def->name;
            if (!this->useSet->count(def)) {
                this->defSet->insert(def);
            }
        }
        auto uses = code->getUse();
        if (uses != nullptr) {
            for (auto use : *uses) {
                if (use != nullptr) {
                    if (!this->defSet->count(use)) {
                        this->useSet->insert(use);
                    }
                }
            }
        }
    }
}

void BasicBlock::add(int *codeIndex, MiddleCodeItem *middleCodeItem) {
    middleCodeItem->setIndex(*codeIndex);
    *codeIndex = *codeIndex + 1;
    middleCodeItems.insert(middleCodeItem);
}
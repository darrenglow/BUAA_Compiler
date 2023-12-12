//
// Created by 安达楷 on 2023/11/19.
//
#include "Func.h"
// Func
std::ostream & Func::output(std::ostream &os) const {
    if (type == DEF_FUNC) {
        os << "###### BEGIN_" << funcName << " ######" << std::endl;
        os << "###### func_size is " << funcSymbolTable->getSize() << "######" << std::endl;
        os << *label << std::endl;
        for (auto basicBlock : basicBlocks) {
            os << *basicBlock << std::endl;
        }
        os << "###### END_" << funcName << " ######" << std::endl;
    }
    return os;
}
void Func::setFuncBlock(class BasicBlock *basicBlock) {
    block = basicBlock;
}
int Func::getSize() {
    return funcSymbolTable->getSize();
}
void Func::addBlock(class BasicBlock *x) {
    basicBlocks.push_back(x);
}

void Func::calculateSymbolUse() {
    for (auto bb : basicBlocks) {
        auto basicBlock = *bb;
        for (auto code : basicBlock.middleCodeItems) {
            auto def = code->getDef();
            symbolUse[def] ++ ;
            auto uses = code->getUse();
            for (auto use : *uses) {
                symbolUse[use] ++ ;
            }
        }
    }
}

void Func::resetCode() {
    middleCodeItems.clear();
    for (auto bb : basicBlocks) {
        for (auto code : bb->middleCodeItems) {
            code->setInFuncIndex(curIndex);
            curIndex ++ ;
            middleCodeItems.insert(code);
        }
    }
}
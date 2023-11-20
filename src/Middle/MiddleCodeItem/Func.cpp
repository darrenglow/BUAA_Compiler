//
// Created by 安达楷 on 2023/11/19.
//
#include "Middle.h"
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
void Func::setFuncBlock(BasicBlock *basicBlock) {
    block = basicBlock;
}
int Func::getSize() {
    return funcSymbolTable->getSize();
}
void Func::addBlock(BasicBlock *block) {
    basicBlocks.push_back(block);
}
//
// Created by 安达楷 on 2023/11/19.
//

#ifndef BUAA_COMPILER_FUNC_H
#define BUAA_COMPILER_FUNC_H
#include "MiddleCodeItem.h"
#include "Label.h"
#include "BasicBlock.h"


class Func : public MiddleCodeItem {
public:
    enum Type{
        DEF_FUNC,
    };

    std::string funcName;
    BasicBlock *block{};    // 这个block就只是作为函数跳转到的入口了
    std::vector<BasicBlock*> basicBlocks;
    SymbolTable * funcSymbolTable = new SymbolTable();
    std::vector<ValueSymbol*> tempSymbols;  // 临时变量
    Label *label;
    Type type;
    BasicBlock *root;   // 数据流分析中的起始
    bool hasReturn{};
    explicit Func(Type type_, std::string &funcName_) : type(type_), funcName(funcName_) {
        std::string tmp = "Func_" + funcName;
        label = new Label(tmp);
    }

    void setFuncBlock(BasicBlock* block);
    int getSize();
    void addBlock(BasicBlock* block);
    OVERRIDE_OUTPUT;
};
#endif //BUAA_COMPILER_FUNC_H

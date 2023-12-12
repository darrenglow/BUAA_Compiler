//
// Created by 安达楷 on 2023/11/19.
//

#ifndef BUAA_COMPILER_FUNC_H
#define BUAA_COMPILER_FUNC_H
#include "MiddleCodeItem.h"
#include "Label.h"
#include "BasicBlock.h"
#include "../../Backend/Register/RegisterAlloc.h"
#include <ostream>

class RegisterAlloc;
class Func : public MiddleCodeItem {
public:
    enum Type{
        DEF_FUNC,
    };
    int curIndex = 0;
    std::string funcName;
    class BasicBlock *block{};    // 这个block就只是作为函数跳转到的入口了
    std::vector<class BasicBlock*> basicBlocks{};
    SymbolTable * funcSymbolTable = new SymbolTable();
    std::vector<ValueSymbol*> tempSymbols;  // 临时变量
    class Label *label;
    FuncSymbol *funcSymbol{};
    Type type;
    class BasicBlock *root{};   // 数据流分析中的起始
    bool hasReturn{};

    std::set<MiddleCodeItem*, InFuncCompareMiddleCodeItem> middleCodeItems;

    // 图着色寄存器时，先保存其分配器
    RegisterAlloc *registerAlloc{};

    std::unordered_map<Symbol*, int> symbolUse;

    explicit Func(Type type_, std::string &funcName_) : MiddleCodeItem(MiddleCodeItem::Func), type(type_), funcName(funcName_) {
        std::string tmp = "Func_" + funcName;
        label = new class Label(tmp);
    }

    void setFuncBlock(class BasicBlock* block);
    int getSize();
    void addBlock(class BasicBlock* block);
    void calculateSymbolUse();
    void resetCode();
    OVERRIDE_OUTPUT;
};
#endif //BUAA_COMPILER_FUNC_H

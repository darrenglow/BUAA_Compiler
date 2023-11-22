//
// Created by 安达楷 on 2023/11/19.
//

#ifndef BUAA_COMPILER_BASICBLOCK_H
#define BUAA_COMPILER_BASICBLOCK_H

#include "MiddleCodeItem.h"
#include "Label.h"
#include "../Optimization/DefinitionSet.h"
#include "../Optimization/PositiveSet.h"

class BasicBlock : public MiddleCodeItem {
public:
    enum Type{
        FUNC,
        BLOCK
    };
    Type type;
    class Label *label;   // FUNC, LOOP, BRANCH
    std::string type2str[2] = {"BLOCK_FUNC", "BLOCK"};
    std::set<MiddleCodeItem*, CompareMiddleCodeItem> middleCodeItems;
    int basicBlockID;
    static int blockID;

    std::vector<BasicBlock*> follows;
    std::vector<BasicBlock*> prevs;

    // 到达定义分析
    DefinitionSet *killSet=new DefinitionSet();
    DefinitionSet *genSet=new DefinitionSet();
    DefinitionSet *inDefSet=new DefinitionSet();    // block中的defset
    DefinitionSet *inDefFlow=new DefinitionSet();
    DefinitionSet *outDefFlow=new DefinitionSet();

    // 活跃变量分析
    std::set<ValueSymbol*> *defSet = new std::set<ValueSymbol*>();
    std::set<ValueSymbol*> *useSet = new std::set<ValueSymbol*>();
    std::set<ValueSymbol*> *outPosFlow = new std::set<ValueSymbol*>();
    std::set<ValueSymbol*> *inPosFlow = new std::set<ValueSymbol*>();
    void calcDefAndUse();

    BasicBlock(Type type_, std::string &name) : MiddleCodeItem(MiddleCodeItem::BasicBlock), type(type_), label(new class Label(name)) { basicBlockID = blockID ++ ; }
    explicit BasicBlock(Type type_) : MiddleCodeItem(MiddleCodeItem::BasicBlock), type(type_), label(new class Label()) { basicBlockID = blockID ++ ;}

    void add(int *codeIndex, MiddleCodeItem *middleCodeItem);

    class Label * getLabel();
    void setNext(BasicBlock *next);
    OVERRIDE_OUTPUT;
};

#endif //BUAA_COMPILER_BASICBLOCK_H

//
// Created by 安达楷 on 2023/11/19.
//

#ifndef BUAA_COMPILER_BASICBLOCK_H
#define BUAA_COMPILER_BASICBLOCK_H

#include "MiddleCodeItem.h"
#include "Label.h"
#include "../Optimization/DefinitionSet.h"

class BasicBlock : public MiddleCodeItem {
public:
    enum Type{
        FUNC,
        BLOCK
    };
    Type type;
    Label *label;   // FUNC, LOOP, BRANCH
    std::string type2str[2] = {"BLOCK_FUNC", "BLOCK"};
    std::vector<MiddleCodeItem*> middleCodeItems;
    int basicBlockID;
    static int blockID;

    std::vector<BasicBlock*> follows;
    std::vector<BasicBlock*> prevs;

    // 到达定义分析
    DefinitionSet *killSet=new DefinitionSet();
    DefinitionSet *genSet=new DefinitionSet();
    DefinitionSet *inDefSet=new DefinitionSet();    // block中的defset
    BasicBlock(Type type_, std::string &name) : type(type_), label(new Label(name)) { basicBlockID = blockID ++ ; }
    explicit BasicBlock(Type type_) : type(type_), label(new Label()) { basicBlockID = blockID ++ ;}

    void add(MiddleCodeItem *middleCodeItem);

    Label * getLabel();
    void setNext(BasicBlock *next);
    OVERRIDE_OUTPUT;
};

#endif //BUAA_COMPILER_BASICBLOCK_H

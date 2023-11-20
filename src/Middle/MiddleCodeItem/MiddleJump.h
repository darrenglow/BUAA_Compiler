//
// Created by 安达楷 on 2023/11/19.
//

#ifndef BUAA_COMPILER_MIDDLEJUMP_H
#define BUAA_COMPILER_MIDDLEJUMP_H
#include "MiddleCodeItem.h"
#include "BasicBlock.h"
// JUMP_EQZ a label
class MiddleJump : public MiddleCodeItem {
public:
    enum Type {
        JUMP,
        JUMP_EQZ,
        JUMP_NEZ,
    };
    std::string type2str[3] = {"JUMP", "JUMP_EQZ", "JUMP_NEZ"};
    Type type;
    Intermediate *src;
    BasicBlock *target;
    BasicBlock *anotherTarget{}; // 针对`visitCond`方便建立数据流
    MiddleJump(Type type_, BasicBlock* target_)
            : type(type_), target(target_), src(nullptr){}
    MiddleJump(Type type_, Intermediate *src_, BasicBlock *target_)
            : type(type_), src(src_), target(target_) {}
    //针对visitCond中的basicBLock，方便后面搭建数据流
    MiddleJump(Type type_, Intermediate *src_, BasicBlock *target_, BasicBlock *anotherTarget_)
            : type(type_), src(src_), target(target_), anotherTarget(anotherTarget_) {}

    Intermediate * getRightIntermediate1() override;
    OVERRIDE_OUTPUT;
};
#endif //BUAA_COMPILER_MIDDLEJUMP_H

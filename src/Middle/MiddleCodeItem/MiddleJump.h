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
    class BasicBlock *target;
    class BasicBlock *anotherTarget{}; // 针对`visitCond`方便建立数据流
    MiddleJump(Type type_, class BasicBlock* target_)
            : MiddleCodeItem(MiddleCodeItem::MiddleJump), type(type_), target(target_), src(nullptr){}
    MiddleJump(Type type_, Intermediate *src_, class BasicBlock *target_)
            : MiddleCodeItem(MiddleCodeItem::MiddleJump), type(type_), src(src_), target(target_) {}
    //针对visitCond中的basicBLock，方便后面搭建数据流
    MiddleJump(Type type_, Intermediate *src_, class BasicBlock *target_, class BasicBlock *anotherTarget_)
            : MiddleCodeItem(MiddleCodeItem::MiddleJump), type(type_), src(src_), target(target_), anotherTarget(anotherTarget_) {}

    Intermediate * _getSrc1() override;
    OVERRIDE_OUTPUT;
};
#endif //BUAA_COMPILER_MIDDLEJUMP_H

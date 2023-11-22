//
// Created by 安达楷 on 2023/11/19.
//

#ifndef BUAA_COMPILER_MIDDLEFUNCCALL_H
#define BUAA_COMPILER_MIDDLEFUNCCALL_H
#include "MiddleCodeItem.h"

class MiddleFuncCall : public MiddleCodeItem {
public:
    std::string funcName;
    std::vector<Intermediate*> funcRParams;
    ValueSymbol *ret{};
    explicit MiddleFuncCall(std::string &funcName_) : MiddleCodeItem(MiddleCodeItem::MiddleFuncCall), funcName(funcName_) {}
    OVERRIDE_OUTPUT;
};
#endif //BUAA_COMPILER_MIDDLEFUNCCALL_H

//
// Created by 安达楷 on 2023/11/5.
//

#ifndef BUAA_COMPILER_MIPSGENERATOR_H
#define BUAA_COMPILER_MIPSGENERATOR_H


#include <vector>
#include <iostream>
#include "Instruction.h"
#include "../Middle/Symbol/Symbol.h"
#include "../Middle/MiddleCodeItem/MiddleCodeItem.h"
#include "../Middle/MiddleCodeItem/MiddleDef.h"
#include "../Middle/MiddleCodeItem/MiddleUnaryOp.h"
#include "../Middle/MiddleCodeItem/MiddleBinaryOp.h"
#include "../Middle/MiddleCodeItem/MiddleOffset.h"
#include "../Middle/MiddleCodeItem/MiddleMemoryOp.h"
#include "../Middle/MiddleCodeItem/MiddleJump.h"
#include "../Middle/MiddleCodeItem/MiddleIO.h"
#include "../Middle/MiddleCodeItem/MiddleFuncCall.h"
#include "../Middle/MiddleCodeItem/MiddleReturn.h"

class MipsGenerator {
public:
    static MipsGenerator& getInstance() {
        static MipsGenerator instance;
        return instance;
    }
    std::vector<Instruction*> mips;

    int curStackSize = 0;
    int heapTop = 0x10008000;  //用于计算字符串的起始地址
    Func* currentFunc;
    BasicBlock *curBlock{};
    // 全局变量的地址表示直接使用立即数的形式
    std::unordered_map<ValueSymbol*, int> globalSymbolAddress;
    std::unordered_map<std::string, int> globalStringAddress;
    RegisterAlloc *allocator{};
    // 记录变量的使用次数
    std::unordered_map<Symbol*, int> symbolUse{};
    void add(Instruction *instruction);
    void doMipsGeneration();
    void translate();
    void translateGlobalStrings(const std::vector<std::string>& strings);
    void translateGlobalValues(const std::vector<ValueSymbol *>& valueSymbols);
    void translateFuncs(const std::vector<Func *>& funcs);
    void consumeUse(Symbol *symbol);


    int TYPE_GLOBAL = 1;
    int TYPE_PARAM = 2;
    int TYPE_TEMP = 4;
    int TYPE_SPILL = 8;
private:
    MipsGenerator()=default;
    ~MipsGenerator()=default;


    void translateFunc(Func *func);

    void translateBlock(BasicBlock *block);

    void translateMiddleDef(MiddleDef *middleDef);

    void translateMiddleUnaryOp(MiddleUnaryOp *middleUnaryOp);

    void translateMiddleBinaryOp(MiddleBinaryOp *middleBinaryOp);

    void translateMiddleOffset(MiddleOffset *middleOffset);

    void translateMiddleMemoryOp(MiddleMemoryOp *middleMemoryOp);

    void translateMiddleJump(MiddleJump *middleJump);

    void translateLabel(Label *label);

    void translateMiddleIO(MiddleIO *middleIO);

    void processTempSymbols(std::vector<ValueSymbol *> &tempSymbols);

    int calculateStringMemorySize(const std::basic_string<char> &basicString);

    void translateMiddleFuncCall(MiddleFuncCall *middleFuncCall);

    void translateMiddleReturn(MiddleReturn *middleReturn);
};

#endif //BUAA_COMPILER_MIPSGENERATOR_H

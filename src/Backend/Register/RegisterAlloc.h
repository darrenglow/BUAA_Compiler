//
// Created by 安达楷 on 2023/11/5.
//

#ifndef BUAA_COMPILER_REGISTERALLOC_H
#define BUAA_COMPILER_REGISTERALLOC_H

#include <unordered_map>
#include "../../Middle/MiddleCode.h"
#include "../../Middle/MiddleCodeItem/MiddleCodeItem.h"
#include "../../Middle/Symbol/Symbol.h"
#include "Register.h"
#include "../Optimization/ColorAllocator.h"
#include <list>
#include <algorithm>
#include <queue>
#include "../../Middle/MiddleCodeItem/Func.h"

class Func;
class RegisterAlloc {
public:
    std::unordered_map<RegType, Symbol*> registerMap = {
//            {RegType::$zero, {nullptr, 0}}, {RegType::$at, {nullptr, 0}},
//            {RegType::$v0, {nullptr, 0}}
            {RegType::$v1, nullptr},
//            {RegType::$a0, nullptr}, {RegType::$a1, nullptr},
            {RegType::$a2, nullptr}, {RegType::$a3, nullptr},
            {RegType::$t0, nullptr},{RegType::$t1, nullptr},{RegType::$t2, nullptr},{RegType::$t3, nullptr},{RegType::$t4, nullptr},{RegType::$t5, nullptr},{RegType::$t6, nullptr},{RegType::$t7, nullptr},
            {RegType::$s0, nullptr},{RegType::$s1, nullptr},{RegType::$s2, nullptr},{RegType::$s3, nullptr},{RegType::$s4, nullptr},{RegType::$s5, nullptr},{RegType::$s6, nullptr},{RegType::$s7, nullptr},
            {RegType::$t8, nullptr},{RegType::$t9, nullptr},
            {RegType::$k0, nullptr},{RegType::$k1, nullptr},
//            {RegType::$gp, {nullptr, 0}},
//            {RegType::$sp, {nullptr, 0}},
            {RegType::$fp, nullptr},
//            {RegType::$ra, {nullptr, 0}}
    };
    std::unordered_map<RegType, bool> dirtyMap = {
//            {RegType::$zero, {nullptr, 0}}, {RegType::$at, {nullptr, 0}},
//            {RegType::$v0, {nullptr, 0}}
            {RegType::$v1, false},
//            {RegType::$a0, nullptr}, {RegType::$a1, nullptr},
            {RegType::$a2, false}, {RegType::$a3, false},
            { RegType::$t0, false }, { RegType::$t1, false }, { RegType::$t2, false }, { RegType::$t3, false }, { RegType::$t4, false }, { RegType::$t5, false }, { RegType::$t6, false }, { RegType::$t7, false },
            { RegType::$s0, false }, { RegType::$s1, false }, { RegType::$s2, false }, { RegType::$s3, false }, { RegType::$s4, false }, { RegType::$s5, false }, { RegType::$s6, false }, { RegType::$s7, false },
            { RegType::$t8, false }, { RegType::$t9, false },
            { RegType::$k0, false }, { RegType::$k1, false },
//            {RegType::$gp, {nullptr, 0}},
//            {RegType::$sp, {nullptr, 0}},
            { RegType::$fp, false },
//            {RegType::$ra, {nullptr, 0}}
    };

    std::vector<RegType> availableRegs = {
            $v1,
            $a1,
            $a2, $a3, // 6-7
            $t0, $t1, $t2, $t3, $t4, $t5, $t6, $t7, // 8-15
            $s0, $s1, $s2, $s3, $s4, $s5, $s6, $s7, // 16-23
            $t8, $t9, // 24-25
            $k0, $k1, // 26-27
            $fp,
    };

    std::vector<RegType> globalRegs = {
            $a1,
            $a2, $a3, // 6-7
            $t0, $t1, // 8-15
    };
    std::vector<RegType> localRegs = {
            $t3,$t4, $t5, $t6, $t7,
            $s0, $s1, $s2, $s3, $s4, $s5, $s6, $s7, // 16-23
            $t8, $t9, // 24-25
            $k0, $k1, // 26-27
            $fp,
    };
    // 使用localRegs
    std::list<RegType> freeRegs = {
            $t3,$t4, $t5, $t6, $t7,
            $s0, $s1, $s2, $s3, $s4, $s5, $s6, $s7, // 16-23
            $t8, $t9, // 24-25
            $k0, $k1, // 26-27
            $fp,
    };
    std::unordered_map<Symbol*, RegType> graphMap{};
    ColorAllocator *colorAllocator{};
    // OPT
    std::unordered_map<Symbol*, RegType> tempRegisters = std::unordered_map<Symbol*, RegType>();
    // 写到后面才发现需要Symbol到reg的对应，纯纯堆屎山了这里。相当于一起维护两个。
    std::unordered_map<RegType, Symbol*> tempRegisters2 = std::unordered_map<RegType, Symbol*>();
    std::list<RegType> registersCache{};

    RegisterAlloc(ColorAllocator *colorAllocator_) : colorAllocator(colorAllocator_) {}
    bool hasFreeReg();







    int ptr = 0;

    // TODO: 先简单暴力地写一个
    RegType allocRegister(Symbol *symbol, BasicBlock *curBlock, int curLine, bool fromMemory=true, bool afterFuncCall=false);

    void dealWithSymbol(Symbol *symbol, RegType reg, bool fromMemory, BasicBlock *curBlock, int curLine);

    RegType findRegister(Symbol *symbol);

    void saveRegisters();

    void pushBackToMem(RegType reg, ValueSymbol *valueSymbol);

    void clearAllRegister();

    void forceSymbolToRegister(ValueSymbol *valueSymbol, RegType reg);

    void clearRegister(RegType reg);

    Symbol * OPT(BasicBlock *curBlock, int curLine);

    void freeSymbolInRegs(Symbol *symbol, bool save);

    void freeRegister(RegType reg);


    int TYPE_GLOBAL = 1;
    int TYPE_PARAM = 2;
    int TYPE_TEMP = 4;
    int TYPE_SPILL = 8;
    void freeRegisters(int type, bool save, Symbol *exceptSymbol = nullptr);
    void saveAllGlobalRegisters(MiddleCodeItem *code);
    void forceAllocRegister(Symbol* symbol, RegType reg);
};

#endif //BUAA_COMPILER_REGISTERALLOC_H

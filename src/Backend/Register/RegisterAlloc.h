//
// Created by 安达楷 on 2023/11/5.
//

#ifndef BUAA_COMPILER_REGISTERALLOC_H
#define BUAA_COMPILER_REGISTERALLOC_H

#include <unordered_map>
#include "../../Middle/MiddleCode.h"
#include "../../Middle/MiddleCodeItem.h"
#include "../../Middle/Symbol/Symbol.h"
#include "Register.h"

class RegisterAlloc {
public:
    static RegisterAlloc& getInstance() {
        static RegisterAlloc instance;
        return instance;
    }
    std::unordered_map<RegType, Symbol*> registerMap = {
//            {RegType::$zero, {nullptr, 0}}, {RegType::$at, {nullptr, 0}},
//            {RegType::$v0, {nullptr, 0}}, {RegType::$v1, {nullptr, 0}},
//            {RegType::$a0, nullptr}, {RegType::$a1, nullptr},
            {RegType::$a2, nullptr}, {RegType::$a3, nullptr},
            {RegType::$t0, nullptr},{RegType::$t1, nullptr},{RegType::$t2, nullptr},{RegType::$t3, nullptr},{RegType::$t4, nullptr},{RegType::$t5, nullptr},{RegType::$t6, nullptr},{RegType::$t7, nullptr},
            {RegType::$s0, nullptr},{RegType::$s1, nullptr},{RegType::$s2, nullptr},{RegType::$s3, nullptr},{RegType::$s4, nullptr},{RegType::$s5, nullptr},{RegType::$s6, nullptr},{RegType::$s7, nullptr},
            {RegType::$t8, nullptr},{RegType::$t9, nullptr},
            {RegType::$k0, nullptr},{RegType::$k1, nullptr},
//            {RegType::$gp, {nullptr, 0}},
//            {RegType::$sp, {nullptr, 0}},
//            {RegType::$fp, nullptr},
//            {RegType::$ra, {nullptr, 0}}
    };
    std::vector<RegType> availableRegs = {
//            $a0, $a1,
            $a2, $a3, // 6-7
            $t0, $t1, $t2, $t3, $t4, $t5, $t6, $t7, // 8-15
            $s0, $s1, $s2, $s3, $s4, $s5, $s6, $s7, // 16-23
            $t8, $t9, // 24-25
            $k0, $k1, // 26-27
//            $fp,
    };
    int ptr = 0;

    // TODO: 先简单暴力地写一个
    RegType allocRegister(Symbol *symbol, bool fromMemory=true);

    RegType findRegister(Symbol *symbol);

    void clearRegister();

    void pushBackToMem(RegType reg, ValueSymbol *valueSymbol);

private:
    RegisterAlloc()=default;
    ~RegisterAlloc()=default;
};

#endif //BUAA_COMPILER_REGISTERALLOC_H

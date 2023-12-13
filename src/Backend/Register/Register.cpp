//
// Created by 安达楷 on 2023/11/6.
//

#include "Register.h"

std::string Register::type2str[33] = {
        "$zero", "$at",
        "$v0", "$v1",
        "$a0", "$a1", "$a2", "$a3",
        "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
        "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
        "$t8", "$t9",
        "$k0", "$k1",
        "$gp",
        "$sp",
        "$fp",
        "$ra",
        "none"
};

int Register::registerValues[33] = {
        0,0,
        0,0,
        0,0,0,0,
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
        0,0,
        0,0,
        0x10008000,
        0x7fffeffc,
        0,
        0,
        0,
};

std::set<RegType> Register::globalRegisters = {
        $a1,
        $a2, $a3, $t0, $t1, $v1,$t2// 6-7
};
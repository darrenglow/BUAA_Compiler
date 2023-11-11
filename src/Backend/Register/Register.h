//
// Created by 安达楷 on 2023/11/6.
//

#ifndef BUAA_COMPILER_REGISTER_H
#define BUAA_COMPILER_REGISTER_H

#include <string>
#include <vector>

enum RegType {
    $zero, $at, // 0-1
    $v0, $v1,   // 2-3 函数返回值和立即数
    $a0, $a1, $a2, $a3, // 4-7
    $t0, $t1, $t2, $t3, $t4, $t5, $t6, $t7, // 8-15
    $s0, $s1, $s2, $s3, $s4, $s5, $s6, $s7, // 16-23
    $t8, $t9, // 24-25
    $k0, $k1, // 26-27
    $gp, //28
    $sp, //29
    $fp, //30
    $ra, //31
    none
};
class Register {
public:
    static std::string type2str[33];
    static int registerValues[33];
};

#endif //BUAA_COMPILER_REGISTER_H

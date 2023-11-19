//
// Created by 安达楷 on 2023/11/5.
//

#ifndef BUAA_COMPILER_INSTRUCTION_H
#define BUAA_COMPILER_INSTRUCTION_H

#include <string>
#include <utility>
#include "../Middle/MiddleCodeItem.h"
#include "Register/RegisterAlloc.h"

#define OVERRIDE_OUTPUT std::ostream& output(std::ostream &os) const override

class Instruction {
    virtual std::ostream& output(std::ostream &os) const = 0;
    friend std::ostream& operator<<(std::ostream& os, const Instruction& obj);
};

class Notation : public Instruction{
public:
    std::string notation;
    explicit Notation(const std::string &notation_) : notation(notation_) {}
    OVERRIDE_OUTPUT;
};

class GlobalString : public Instruction{
public:
    std::string str;
    explicit GlobalString(const std::string &str_) : str(str_) {}
    OVERRIDE_OUTPUT;
};

class GlobalValue : public Instruction {
public:
    ValueSymbol *valueSymbol;
    explicit GlobalValue(ValueSymbol *valueSymbol) : valueSymbol(valueSymbol) {}
    OVERRIDE_OUTPUT;
};

class GlobalArray : public Instruction{
public:
    ValueSymbol *valueSymbol;
    explicit GlobalArray(ValueSymbol *valueSymbol_) : valueSymbol(valueSymbol_) {}
    OVERRIDE_OUTPUT;
};

class MipsLabel : public Instruction{
public:
    Label *label;
    explicit MipsLabel(Label *label_) : label(label_) {}
    OVERRIDE_OUTPUT;
};

// op rd
class R : public Instruction {
public:
    enum Type {
        mflo, mfhi,
        jr
    };
    Type type;
    std::string type2str[3] = {"mflo", "mfhi", "jr"};
    RegType rd;
    explicit R(Type type_, RegType rd_) : type(type_), rd(rd_) {}
    OVERRIDE_OUTPUT;
};

// RI：一个寄存器，一个立即数  op reg immediate
class RI : public Instruction{
public:
    enum Type {
        li
    };
    std::string type2str[1] = {"li"};
    Type type;
    RegType reg;
    int immediate;
    RI(Type type_, RegType reg_, int immediate_) :
        type(type_), reg(reg_), immediate(immediate_) {}
    OVERRIDE_OUTPUT;
};

// RR：两个寄存器 op rs rt
class RR : public Instruction{
public:
    enum Type {
        move,
        div,    // mflo: 商， mfhi: 余数
    };
    std::string type2str[2] = {"move", "div"};
    Type type;
    RegType rs;
    RegType rt;
    RR(Type type_, RegType rs_, RegType rt_) : type(type_), rs(rs_), rt(rt_) {}
    OVERRIDE_OUTPUT;
};

class RRI : public Instruction {
public:
    enum Type {
        addi, addiu, andi, ori, xori, slti, sltiu, subiu,
        sll, srl,
    };
    std::string type2str[10] = {
            "addi", "addiu", "andi", "ori", "xori", "slti", "sltiu", "subiu",
            "sll", "srl"
    };
    Type type;
    RegType rd, rs;
    int immediate;
    RRI(Type type_, RegType rd_, RegType rs_, int immediate_) : type(type_), rd(rd_), rs(rs_), immediate(immediate_) {}
    OVERRIDE_OUTPUT;
};

// 三个寄存器, op rs rd rt
class RRR : public Instruction{
public:
    enum Type {
        add, addu, sub, subu, mul,
        slt, sltu, seq, sgt, sne, sge, sle,
    };
    std::string type2str[12] = {
            "add", "addu", "sub", "subu", "mul",
            "slt", "sltu", "seq", "sgt", "sne", "sge", "sle"
    };
    Type type;
    RegType rd, rs, rt;
    RRR(Type type_, RegType rd_, RegType rs_, RegType rt_) : type(type_), rs(rs_), rd(rd_), rt(rt_) {}
    OVERRIDE_OUTPUT;
};

// lw $rd offset($rs)
class M : public Instruction{
public:
    enum Type {
        lw, sw
    };
    std::string type2str[2] = {"lw", "sw"};
    Type type;
    RegType rs, rd;
    int offset;
    M(Type type_, RegType rd_, int offset_, RegType rs_) : type(type_), rd(rd_), rs(rs_), offset(offset_) {}
    OVERRIDE_OUTPUT;
};

// 操作Label
// j L
class L : public Instruction {
public:
    enum Type {
        j
    };
    std::string type2str[2] = {"j"};
    Type type;
    BasicBlock *target;
    L(Type type_, BasicBlock *target_) : type(type_), target(target_) {}
    OVERRIDE_OUTPUT;
};


class JAL : public Instruction {
public:
    Label *label;
    explicit JAL(Label *label_) : label(label_) {}
    OVERRIDE_OUTPUT;
};

// 根据寄存器的值，操作label
// bnez $t0 label
class RL : public Instruction {
public:
    enum Type {
        bnez,
        beqz
    };
    std::string type2str[2] = {"bnez", "beqz"};
    Type type;
    BasicBlock *target;
    RegType rd;
    RL(Type type_, RegType rd_, BasicBlock*target_) : type(type_), rd(rd_), target(target_) {}
    OVERRIDE_OUTPUT;
};

class Syscall : public Instruction {
public:
    OVERRIDE_OUTPUT;
};

#endif //BUAA_COMPILER_INSTRUCTION_H

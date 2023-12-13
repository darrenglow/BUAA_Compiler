//
// Created by 安达楷 on 2023/11/5.
//

#include "Instruction.h"
#include "Register/Register.h"
#include "Register/RegisterAlloc.h"
#include <iostream>

std::ostream& operator<<(std::ostream &os, const Instruction &obj) {
    return obj.output(os);
}

// notation
std::ostream & Notation::output(std::ostream &os) const {
    os << notation;
    return os;
}

// GlobalString
std::ostream & GlobalString::output(std::ostream &os) const {
    os << str;
    return os;
}

// GlobalValue
std::ostream & GlobalValue::output(std::ostream &os) const {
    os << valueSymbol->name << ": .word " << valueSymbol->initValue;
    return os;
}

// GlobalArray
std::ostream & GlobalArray::output(std::ostream &os) const {
    os << valueSymbol->name << ": .word ";
    for (auto value : valueSymbol->initValues) {
        os << value << ",";
    }
    return os;
}

// MipsLabel
std::ostream & MipsLabel::output(std::ostream &os) const {
    os << *label;
    return os;
}

std::ostream & R::output(std::ostream &os) const {
    os << type2str[type] << " " << Register::type2str[rd];
    return os;
}

std::ostream & RI::output(std::ostream &os) const {
    os << type2str[type] << " " <<  Register::type2str[reg] << " " << immediate;
    return os;
}


std::ostream & RR::output(std::ostream &os) const {
    os << type2str[type] << " " << Register::type2str[rs] << " " << Register::type2str[rt];
    return os;
}

std::ostream & RRR::output(std::ostream &os) const {
    if (rt != none)
        os << type2str[type] << " " << Register::type2str[rd] << " " << Register::type2str[rs] << " " << Register::type2str[rt];
    else {
        os << type2str[type] << " " << Register::type2str[rd] << " " << Register::type2str[rs] << " " << x;
    }
    return os;
}

std::ostream & M::output(std::ostream &os) const {
    os << type2str[type] << " " << Register::type2str[rd] << " " << offset << "(" << Register::type2str[rs] << ")";
    return os;
}

std::ostream & RRI::output(std::ostream &os) const {
    os << type2str[type] << " " << Register::type2str[rd] << " " << Register::type2str[rs] << " " << immediate;
    return os;
}

std::ostream & L::output(std::ostream &os) const {
    os << type2str[type] << " " << target->label->label;
    return os;
}

std::ostream & JAL::output(std::ostream &os) const {
    os << "jal " << label->label;
    return os;
}

std::ostream & RL::output(std::ostream &os) const {
    os << type2str[type] << " " << Register::type2str[rd] << " " << target->label->label;
    return os;
}

std::ostream & Syscall::output(std::ostream &os) const {
    os << "syscall";
    return os;
}

std::ostream & RRL::output(std::ostream &os) const {
    os << type2str[type] << " " << Register::type2str[rs] << " " << Register::type2str[rt] << " " << target->label->label;
    return os;
}
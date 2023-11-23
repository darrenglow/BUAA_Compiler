//
// Created by 安达楷 on 2023/11/5.
//

#include <iostream>
#include <fstream>
#include <typeinfo>
#include <cstring>
#include <sstream>
#include "../Util/Debug.h"
#include "MipsGenerator.h"
#include "../Middle/MiddleCode.h"
#include "../Middle/MiddleCodeItem/MiddleCodeItem.h"
void MipsGenerator::add(Instruction* mipsCode) {
    mips.push_back(mipsCode);
}

extern std::ofstream mipsOutput;
void MipsGenerator::doMipsGeneration() {
    // 生成mips代码
    translate();
    // 输出mips
    for (const auto& code : mips) {
        mipsOutput << *code << std::endl;
    }
}

void MipsGenerator::translate() {
    this->add(new Notation("# ADK 2023 sysy to mips compiler"));
    this->add(new Notation(".data 0x10008000"));
    translateGlobalValues(MiddleCode::getInstance().globalValues);
    translateGlobalStrings(MiddleCode::getInstance().globalStrings);
    this->add(new Notation(".text"));
    translateFuncs(MiddleCode::getInstance().funcs);
}

void MipsGenerator::translateGlobalStrings(const std::vector<std::string>& strings) {
    for (int i = 0; i < strings.size(); i ++ ) {
        auto stringLabel = "str_" + std::to_string(i);
        auto stringContent = ": .asciiz \"" + strings[i] + "\"";
        auto code = new GlobalString(stringLabel + stringContent);
        this->add(code);
        auto strMemSize = calculateStringMemorySize(strings[i]);
        globalStringAddress.insert({stringLabel, heapTop});
        heapTop += strMemSize + 1;  // asciiz的话最后有个'\0'
    }
}

void MipsGenerator::translateGlobalValues(const std::vector<ValueSymbol *>& valueSymbols) {
    for (auto symbol : valueSymbols) {
        if (symbol->valueType == ARRAY) {
            auto code = new GlobalArray(symbol);
            this->add(code);
            auto address = Register::registerValues[$gp] + symbol->getAddress();
            globalSymbolAddress.insert({symbol, address});
            heapTop = address + symbol->getSize();  //主要是取最后一个symbol的地址和大小
        }
        else if (symbol->valueType == SINGLE) {
            this->add(new GlobalValue(symbol));
            auto address = Register::registerValues[$gp] + symbol->getAddress();
            globalSymbolAddress.insert({symbol, address});
            heapTop = address + symbol->getSize();
        }
    }
}

void MipsGenerator::translateFuncs(const std::vector<Func *>& funcs) {
    for (auto func : funcs) {
        if (func->funcName == "main") {
            translateFunc(func);
        }
    }
    for (auto func : funcs) {
        if (func->funcName != "main") {
            translateFunc(func);
        }
    }
}

void MipsGenerator::translateFunc(Func *func) {
    currentFunc = func;
    auto code = new Notation("\n# ===============" +func->funcName + "=================");
    this->add(code);
    this->add(new MipsLabel(func->label));
    curStackSize = func->getSize();
    processTempSymbols(func->tempSymbols);
    for (auto basicBlock : func->basicBlocks) {
        translateBlock(basicBlock);
    }
}

void MipsGenerator::translateBlock(BasicBlock *block) {
    // 每个block开始的时候就清空所有的寄存器
    this->add(new Notation("### BLOCK [" + std::to_string(block->basicBlockID) + "] BEGIN"));
    RegisterAlloc::getInstance().clearAllRegister();

    auto code = new MipsLabel(block->label);
    this->add(code);
    for (auto x : block->middleCodeItems) {
        // 如果不是block块才输出注释
        if (dynamic_cast<BasicBlock*>(x) == nullptr) {
            std::ostringstream ss;
            ss << *x;
            this->add(new Notation("\n# " + ss.str()));
        }


        if (dynamic_cast<MiddleDef*>(x) != nullptr) {
            translateMiddleDef(dynamic_cast<MiddleDef*>(x));
        }
        else if (dynamic_cast<MiddleUnaryOp*>(x) != nullptr) {
            translateMiddleUnaryOp(dynamic_cast<MiddleUnaryOp*>(x));
        }
        else if (dynamic_cast<MiddleBinaryOp*>(x) != nullptr) {
            translateMiddleBinaryOp(dynamic_cast<MiddleBinaryOp*>(x));
        }
        else if (dynamic_cast<MiddleOffset*>(x) != nullptr) {
            translateMiddleOffset(dynamic_cast<MiddleOffset*>(x));
        }
        else if (dynamic_cast<MiddleMemoryOp*>(x) != nullptr) {
            translateMiddleMemoryOp(dynamic_cast<MiddleMemoryOp*>(x));
        }
        else if (dynamic_cast<MiddleJump*>(x) != nullptr) {
            translateMiddleJump(dynamic_cast<MiddleJump*>(x));
        }
        else if (dynamic_cast<Label*>(x) != nullptr) {
            translateLabel(dynamic_cast<Label*>(x));
        }
        else if (dynamic_cast<MiddleIO*>(x) != nullptr) {
            translateMiddleIO(dynamic_cast<MiddleIO*>(x));
        }
        else if (dynamic_cast<BasicBlock*>(x) != nullptr) {
            translateBlock(dynamic_cast<BasicBlock*>(x));
        }
        else if (dynamic_cast<MiddleFuncCall*>(x) != nullptr) {
            translateMiddleFuncCall(dynamic_cast<MiddleFuncCall*>(x));
        }
        else if (dynamic_cast<MiddleReturn*>(x) != nullptr) {
            translateMiddleReturn(dynamic_cast<MiddleReturn*>(x));
        }
    }
    this->add(new Notation("### BLOCK [" + std::to_string(block->basicBlockID) + "] END"));
}

void MipsGenerator::translateMiddleDef(MiddleDef *middleDef) {
    auto type = middleDef->type;
    if (type == MiddleDef::DEF_ARRAY || type == MiddleDef::END_ARRAY)
        return;

    auto src = middleDef->srcValueSymbol;
    auto target = middleDef->valueSymbol;

    if (src != nullptr) {
        if (dynamic_cast<Immediate*>(src) != nullptr) {
            auto rs = dynamic_cast<Immediate*>(src)->value;
            auto rt = RegisterAlloc::getInstance().allocRegister(dynamic_cast<ValueSymbol*>(target), false);
            this->add(new RI(RI::li, rt, rs));
        }
        else if (dynamic_cast<ValueSymbol*>(src)) {
            auto rs = RegisterAlloc::getInstance().allocRegister(dynamic_cast<ValueSymbol*>(src));
            auto rt = RegisterAlloc::getInstance().allocRegister(dynamic_cast<ValueSymbol*>(target), false);
            this->add(new RR(RR::move, rt, rs));
        }
    }

    else if (src == nullptr) {
        RegisterAlloc::getInstance().allocRegister(dynamic_cast<ValueSymbol *>(target), false);
    }
}

void MipsGenerator::translateMiddleUnaryOp(MiddleUnaryOp *middleUnaryOp) {
    auto type = middleUnaryOp->type;
    auto src = middleUnaryOp->srcValueSymbol;
    auto target = middleUnaryOp->valueSymbol;

    if (dynamic_cast<Immediate*>(src)) {
        auto rs = dynamic_cast<Immediate*>(src)->value;
        auto rt = RegisterAlloc::getInstance().allocRegister(dynamic_cast<ValueSymbol*>(target), false);
        switch(type) {
            case MiddleUnaryOp::ASSIGN: this->add(new RI(RI::li, rt, rs)); break;
            case MiddleUnaryOp::POSITIVE: this->add(new RI(RI::li, rt, rs)); break;
            case MiddleUnaryOp::NEGATIVE: this->add(new RI(RI::li, rt, -rs)); break;
            case MiddleUnaryOp::NOT: this->add(new RI(RI::li, rt, rs==0?1:0)); break;
            default: break;
        }
    }
    else if (dynamic_cast<ValueSymbol*>(src)) {
        auto rs = RegisterAlloc::getInstance().allocRegister(dynamic_cast<ValueSymbol*>(src));
        auto rd = RegisterAlloc::getInstance().allocRegister(dynamic_cast<ValueSymbol*>(target), false);
        switch(type) {
            case MiddleUnaryOp::ASSIGN:
            case MiddleUnaryOp::POSITIVE: this->add(new RR(RR::move, rd, rs)); break;
            case MiddleUnaryOp::NEGATIVE: this->add(new RRR(RRR::subu, rd, $zero, rs)); break;
            case MiddleUnaryOp::NOT: this->add(new RRR(RRR::seq, rd, rs, $zero)); break;
            default: break;
        }
    }
}

void MipsGenerator::translateMiddleBinaryOp(MiddleBinaryOp *middleBinaryOp) {
    auto type = middleBinaryOp->type;
    auto src1 = middleBinaryOp->src1;
    auto src2 = middleBinaryOp->src2;
    auto target = middleBinaryOp->target;

    auto t1 = dynamic_cast<Immediate*>(src1);
    auto t2 = dynamic_cast<Immediate*>(src2);
    // 两个立即数
    if (t1 != nullptr && t2 != nullptr) {
        int ans = 0;
        switch (type) {
            case MiddleBinaryOp::ADD: ans = t1->value + t2->value; break;
            case MiddleBinaryOp::SUB: ans = t1->value - t2->value; break;
            case MiddleBinaryOp::MUL: ans = t1->value * t2->value; break;
            case MiddleBinaryOp::DIV: ans = t1->value / t2->value; break;
            case MiddleBinaryOp::MOD: ans = t1->value % t2->value; break;
            case MiddleBinaryOp::EQ: ans = (t1->value == t2->value) ? 1 : 0; break;
            case MiddleBinaryOp::NE: ans = (t1->value != t2->value) ? 1 : 0; break;
            case MiddleBinaryOp::GT: ans = (t1->value > t2->value) ? 1 : 0; break;
            case MiddleBinaryOp::GE: ans = (t1->value >= t2->value) ? 1 : 0; break;
            case MiddleBinaryOp::LT: ans = (t1->value < t2->value) ? 1 : 0; break;
            case MiddleBinaryOp::LE: ans = (t1->value <= t2->value) ? 1 : 0; break;
            default: break;
        }
        auto rt = RegisterAlloc::getInstance().allocRegister(dynamic_cast<ValueSymbol*>(target), false);
        this->add(new RI(RI::li, rt, ans));
    }
    else {
        RegType rd, rs, rt;
        // t1是立即数，t2不是立即数
        if (t1 != nullptr) {
            rs = RegisterAlloc::getInstance().allocRegister(new NumSymbol(t1->value));
            rt = RegisterAlloc::getInstance().allocRegisterAvoid(rs, dynamic_cast<ValueSymbol*>(src2));
            rd = RegisterAlloc::getInstance().allocRegister(dynamic_cast<ValueSymbol*>(target), false);
        }
        // t1不是立即数，t2是立即数
        else if (t2 != nullptr) {
            rs = RegisterAlloc::getInstance().allocRegister(dynamic_cast<ValueSymbol*>(src1));
            rt = RegisterAlloc::getInstance().allocRegisterAvoid(rs, new NumSymbol(t2->value));
            rd = RegisterAlloc::getInstance().allocRegister(dynamic_cast<ValueSymbol*>(target), false);
        }
        // t1, t2都不是立即数
        else {
            rs = RegisterAlloc::getInstance().allocRegister(dynamic_cast<ValueSymbol*>(src1));
            rt = RegisterAlloc::getInstance().allocRegisterAvoid(rs, dynamic_cast<ValueSymbol*>(src2));
            rd = RegisterAlloc::getInstance().allocRegister(dynamic_cast<ValueSymbol*>(target), false);
        }
        switch (type) {
            case MiddleBinaryOp::ADD: this->add(new RRR(RRR::addu, rd, rs, rt)); break;
            case MiddleBinaryOp::SUB: this->add(new RRR(RRR::subu, rd, rs, rt)); break;
            case MiddleBinaryOp::MUL: this->add(new RRR(RRR::mul, rd, rs, rt)); break;
            case MiddleBinaryOp::DIV: this->add(new RR(RR::div, rs, rt));
                                      this->add(new R(R::mflo, rd)); break;
            case MiddleBinaryOp::MOD: this->add(new RR(RR::div, rs, rt));
                                      this->add(new R(R::mfhi, rd)); break;
            case MiddleBinaryOp::EQ: this->add(new RRR(RRR::seq, rd, rs, rt)); break;
            case MiddleBinaryOp::NE: this->add(new RRR(RRR::sne, rd, rs, rt)); break;
            case MiddleBinaryOp::GT: this->add(new RRR(RRR::sgt, rd, rs, rt)); break;
            case MiddleBinaryOp::GE: this->add(new RRR(RRR::sge, rd, rs, rt)); break;
            case MiddleBinaryOp::LT: this->add(new RRR(RRR::slt, rd, rs, rt)); break;
            case MiddleBinaryOp::LE: this->add(new RRR(RRR::sle, rd, rs, rt)); break;
            default: break;
        }
    }
}

// 如果是在函数中是a[1]=10;
// OFFSET 4 a T0; STORE T0 10
void MipsGenerator::translateMiddleOffset(MiddleOffset *middleOffset) {
    auto offset = middleOffset->offset;
    auto base = dynamic_cast<ValueSymbol*>(middleOffset->src);
    auto target = dynamic_cast<ValueSymbol*>(middleOffset->ret);
    auto reg = RegisterAlloc::getInstance().allocRegister(target, false);

    // 如果offset是立即数
    if (dynamic_cast<Immediate*>(offset)) {
        // 如果数组是函数的形参
        if (base->valueType == FUNCFPARAM) {
            auto baseReg = RegisterAlloc::getInstance().allocRegister(base);    // 如果是参数数组的话，此时会将数组的绝对地址放入寄存器baseReg中, base对应的symbol是参数数组
            this->add(new RRI(RRI::addiu, reg, baseReg, dynamic_cast<Immediate*>(offset)->value));
        }
        else {
            if (base->isLocal) {
                this->add(new RRI(RRI::addiu, reg, RegType::$sp, -base->getAddress() + dynamic_cast<Immediate*>(offset)->value));
            }
                // 是全局变量的话
            else {
                this->add(new RRI(RRI::addiu, reg, RegType::$gp, base->getAddress() + dynamic_cast<Immediate*>(offset)->value));
            }
        }
    }
    // 如果offset是变量
    else {
        auto offReg = RegisterAlloc::getInstance().allocRegister(dynamic_cast<ValueSymbol*>(offset));
        // 如果数组是函数的形参
        if (base->valueType == FUNCFPARAM) {
            auto baseReg = RegisterAlloc::getInstance().allocRegister(base);    // 此时会将数组的绝对地址放入寄存器baseReg中
            this->add(new Notation("# !!!!" + base->name + " -> " + Register::type2str[baseReg]));
            this->add(new RRR(RRR::addu, reg, baseReg, offReg));
        }
        else {
            auto baseReg = RegisterAlloc::getInstance().allocRegister(base, false);
            if (base->isLocal) {
                this->add(new RRI(RRI::addi, baseReg, RegType::$sp, -base->getAddress()));
            }
            else {
                this->add(new RRI(RRI::addi, baseReg, RegType::$gp, base->getAddress()));
            }
            this->add(new RRR(RRR::addu, reg, baseReg, offReg));
        }
    }
}

// STORE 1 T0   sw 1 0(T0)
// LOAD T0 T1   lw T0 0(T1) 加载T1的地址的值，存到T0中
void MipsGenerator::translateMiddleMemoryOp(MiddleMemoryOp *middleMemoryOp) {
    auto type = middleMemoryOp->type;
    auto sym1 = middleMemoryOp->sym1;
    auto sym2 = middleMemoryOp->sym2;
    RegType reg1, reg2;
    switch (type) {
        case MiddleMemoryOp::LOAD:
            // load的话，第一个变量不需要从内存中取值
            reg1 = RegisterAlloc::getInstance().allocRegister(dynamic_cast<ValueSymbol*>(sym1), false);
            reg2 = RegisterAlloc::getInstance().allocRegister(dynamic_cast<ValueSymbol*>(sym2));
            this->add(new M(M::lw, reg1, 0, reg2));
            break;
        case MiddleMemoryOp::STORE:
            if (dynamic_cast<Immediate*>(sym1) != nullptr) {
                reg1 = RegisterAlloc::getInstance().allocRegister(new NumSymbol(dynamic_cast<Immediate*>(sym1)->value));
            }
            else {
                reg1 = RegisterAlloc::getInstance().allocRegister(dynamic_cast<ValueSymbol *>(sym1));
            }
            reg2 = RegisterAlloc::getInstance().allocRegister(dynamic_cast<ValueSymbol*>(sym2), true);
            this->add(new M(M::sw, reg1, 0, reg2));
            break;
    }
}

void MipsGenerator::translateMiddleJump(MiddleJump *middleJump) {
    auto type = middleJump->type;
    auto label = middleJump->target;
    auto src = middleJump->src;
    RegType rd = RegType::none;

    RegisterAlloc::getInstance().saveRegisters();

    if (dynamic_cast<Immediate*>(src)) {
        rd = RegisterAlloc::getInstance().allocRegister(new NumSymbol(dynamic_cast<Immediate*>(src)->value));
        this->add(new RI(RI::li, rd, dynamic_cast<Immediate*>(src)->value));
    }
    else if (dynamic_cast<ValueSymbol*>(src)){
        rd = RegisterAlloc::getInstance().allocRegister(dynamic_cast<ValueSymbol*>(src));
    }
    switch(type) {
        case MiddleJump::JUMP: this->add(new L(L::j, label)); break;
        case MiddleJump::JUMP_EQZ: this->add(new RL(RL::beqz, rd, label)); break;
        case MiddleJump::JUMP_NEZ: this->add(new RL(RL::bnez, rd, label)); break;
    }
}

void MipsGenerator::translateLabel(Label *label) {
    this->add(new MipsLabel(label));
}

void MipsGenerator::translateMiddleIO(MiddleIO *middleIO) {
    auto type = middleIO->type;
    auto target = middleIO->target;
    RegType reg = RegType::none;
    switch (type) {
        case MiddleIO::GETINT:
            this->add(new RI(RI::li, $v0, 5));
            this->add(new Syscall());
            if (target == nullptr) {
                return;
            }
            reg = RegisterAlloc::getInstance().allocRegister(dynamic_cast<ValueSymbol*>(target), false);
            if (dynamic_cast<ValueSymbol*>(target)->isArrayElement) {
                this->add(new M(M::sw, $v0, 0, reg));
            }
            else {
                this->add(new RR(RR::move, reg, $v0));
            }
            break;
        case MiddleIO::PRINT_INT:
            // 如果是立即数
            if (dynamic_cast<Immediate*>(target)) {
                this->add(new RI(RI::li, $a0, dynamic_cast<Immediate*>(target)->value));
            }
            // 如果是变量
            else if (dynamic_cast<ValueSymbol*>(target)){
                auto rs = RegisterAlloc::getInstance().allocRegister(dynamic_cast<ValueSymbol*>(target));
                this->add(new RR(RR::move, $a0, rs));
            }
            this->add(new RI(RI::li, $v0, 1));
            this->add(new Syscall());
            break;

        case MiddleIO::PRINT_STR:
            auto strLabel = dynamic_cast<Immediate*>(target)->str;
            DEBUG_PRINT_DIRECT("#######################");
            DEBUG_PRINT_DIRECT(strLabel);
            auto address = globalStringAddress[strLabel];
            this->add(new RI(RI::li, $a0, address));
            this->add(new RI(RI::li, $v0, 4));
            this->add(new Syscall());
            break;
    }
}

void MipsGenerator::processTempSymbols(std::vector<ValueSymbol *> &tempSymbols) {
    for (auto tempSymbol : tempSymbols) {
        curStackSize += tempSymbol->getSize();
        tempSymbol->setAddress(curStackSize);
    }
}

int MipsGenerator::calculateStringMemorySize(const std::basic_string<char> &basicString) {
    int len = basicString.size();
    std::string tempString;
    for (int i = 0; i < len; ++ i) {
        if (basicString[i] == '\\' && i + 1 < len) {
            switch (basicString[i + 1]) {
                case 'n':
                    tempString += '*';  // 替换为你想要的单一字符
                    ++ i;  // 跳过下一个字符
                    break;
                default:
                    tempString += basicString[i];
                    break;
            }
        } else {
            tempString += basicString[i];
        }
    }
    return tempString.size();
}

void MipsGenerator::translateMiddleFuncCall(MiddleFuncCall *middleFuncCall) {
    auto funcName = middleFuncCall->funcName;
    // 先将寄存器的值写回内存中
    this->add(new Notation("# clear Register"));
    RegisterAlloc::getInstance().saveRegisters();
    this->add(new Notation("# end clear Register"));
    // 保存$ra到0($sp)处
    this->add(new M(M::sw, $ra, 0, $sp));
    // 先暂时计算被调用函数栈底地址$a0，因为sp可能会在算地址时还会用到，不能直接移动。
    this->add(new RRI(RRI::addiu, $a0, $sp, -curStackSize - 4));
    // 将实参压入栈中
    int offset = 4;
    for (auto param : middleFuncCall->funcRParams) {
        if (dynamic_cast<Immediate*>(param)) {
            this->add(new RI(RI::li, $v0, dynamic_cast<Immediate*>(param)->value));
            this->add(new M(M::sw, $v0, -offset, $a0));
        }
        // 如果valuesymbol是POINTER类型的话，reg中存的就是POINTER指向的在内存中的地址
        // 如果valueSymbol是FUNCFPAMRA类型的话，他在栈上存储的是数组的起始地址，将这个值load到reg寄存器中。
        else if (dynamic_cast<ValueSymbol*>(param)) {
            auto reg = RegisterAlloc::getInstance().allocRegister(dynamic_cast<ValueSymbol*>(param));
            this->add(new M(M::sw, reg, -offset, $a0));
        }
        offset += 4;
    }

    // 移动$sp
    this->add(new RR(RR::move, $sp, $a0));
    // 调用函数
    auto labelStr = "Func_" + middleFuncCall->funcName;
    auto label = new Label(labelStr);
    this->add(new JAL(label));
    // 清空所有寄存器
    RegisterAlloc::getInstance().clearAllRegister();
    // 回栈
    this->add(new RRI(RRI::addiu, $sp, $sp, curStackSize + 4));
    // 恢复$ra
    this->add(new M(M::lw, $ra, 0, $sp));
    // 获取到返回值，此时$v0中是已经保存了值的
    if (middleFuncCall->ret != nullptr) {
        auto reg = RegisterAlloc::getInstance().allocRegister(middleFuncCall->ret, false);
        this->add(new RR(RR::move, reg, $v0));
    }
}

void MipsGenerator::translateMiddleReturn(MiddleReturn *middleReturn) {
    auto target = middleReturn->target;
    if (currentFunc->funcName == "main") {
        this->add(new RI(RI::li, $v0, 10));
        this->add(new Syscall());
        return;
    }

    if (currentFunc->hasReturn) {
        if (dynamic_cast<Immediate*>(target)) {
            this->add(new RI(RI::li, $v0, dynamic_cast<Immediate*>(target)->value));
        }
        else {
            auto reg = RegisterAlloc::getInstance().allocRegister(dynamic_cast<ValueSymbol*>(target));
            this->add(new RR(RR::move, $v0, reg));
        }
    }
    RegisterAlloc::getInstance().saveRegisters();
    this->add(new R(R::jr, $ra));
}


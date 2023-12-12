//
// Created by 安达楷 on 2023/11/6.
//

#include "RegisterAlloc.h"
#include "../MipsGenerator.h"
#include "Register.h"
#include "../../Util/Debug.h"
#include "../../Middle/MiddleCodeItem/PushParam.h"

// 看Symbol是否在globalRegisters中
// 如果不在，并且满足globalRegister的条件，那就分配一个globalRegister
// 看symbol是否在tempRegisters中
// 如果不在，那就使用OPT的方式分配临时tempRegisters
RegType RegisterAlloc::allocRegister(Symbol *symbol, BasicBlock *curBlock, int curLine, bool fromMemory, bool afterFuncCall) {
//    auto existReg = findRegister(symbol);
//    if (existReg != RegType::none) {
//        return existReg;
//    }
//    auto reg = availableRegs[ptr];
//    ptr ++ ;
//    if (ptr == availableRegs.size()) {
//        ptr = 0;
//    }

    RegType reg = RegType::none;

    // 如果已经分配了寄存器了
    if (colorAllocator->hasGlobalRegister(dynamic_cast<ValueSymbol*>(symbol))) {
        reg = colorAllocator->symbolToGlobalRegister[dynamic_cast<ValueSymbol*>(symbol)];
//        std::cout << dynamic_cast<ValueSymbol*>(symbol)->name << std::endl;
        return reg;
    }
    // 如果满足全局分配器分配条件但是还没有实际分配。
    if (colorAllocator->satisfyGlobalRegister(dynamic_cast<ValueSymbol*>(symbol))) {
        reg = colorAllocator->allocGlobalRegister(dynamic_cast<ValueSymbol*>(symbol));
        // 如果是函数参数的话，需要从内存中取值出来。
        if (dynamic_cast<ValueSymbol*>(symbol) != nullptr && dynamic_cast<ValueSymbol*>(symbol)->valueType == FUNCFPARAM) {
            dealWithSymbol(symbol, reg, fromMemory, curBlock, curLine);
        }
        if (afterFuncCall) {
            dealWithSymbol(symbol, reg, fromMemory, curBlock, curLine);
        }
        return reg;
    }

    // 如果是给数字分配，如果已经存在了该数字，就直接返回。
    if (dynamic_cast<NumSymbol*>(symbol) != nullptr) {
        if (dynamic_cast<NumSymbol*>(symbol)->value == 0) {
            return $zero;
        }
        for (auto x : tempRegisters) {
            if (dynamic_cast<NumSymbol*>(x.first) != nullptr && dynamic_cast<NumSymbol*>(symbol)->value == dynamic_cast<NumSymbol*>(x.first)->value) {
                return x.second;
            }
        }
    }
    // 如果已经在临时寄存器中
    if (tempRegisters.count(symbol) != 0) {
        reg = tempRegisters[symbol];
        return reg;
    }
    // 如果不在临时寄存器中，分配临时寄存器
    // 如果没有空的寄存器了，使用OPT策略free一个
    if (!hasFreeReg()) {
        auto toReplaceSymbol = OPT(curBlock, curLine);
#ifdef DEBUG
        std::cout << "To replace symbol is " << toReplaceSymbol->name << std::endl;
#endif
        freeSymbolInRegs(toReplaceSymbol, true);
    }
    reg = *freeRegs.begin();

    dealWithSymbol(symbol, reg, fromMemory, curBlock, curLine);
    // 如果是溢出的点，虽然分配了临时寄存器，但是本质还是局部变量，归寄存器图管理
    // !!! 且不为TEMP
    // !!! 且不为数组成员或指针。
    if (dynamic_cast<ValueSymbol*>(symbol) != nullptr && colorAllocator->spillSet.count(dynamic_cast<ValueSymbol*>(symbol)) != 0){
        colorAllocator->dealOverflow(dynamic_cast<ValueSymbol*>(symbol), reg);
//        if (afterFuncCall) {
//            dealWithSymbol(symbol, reg, fromMemory, curBlock, curLine);
//        }
    }

    // 对于tempRegisters, freeRegs的更新
    if (dynamic_cast<NumSymbol*>(symbol) != nullptr) {
        tempRegisters[dynamic_cast<NumSymbol*>(symbol)] = reg;
        MipsGenerator::getInstance().add(new Notation("### " + std::to_string(dynamic_cast<NumSymbol*>(symbol)->value) + " -> " + Register::type2str[reg]));
        tempRegisters2[reg] = dynamic_cast<NumSymbol*>(symbol);
        freeRegs.remove(reg);
    }
    else if (dynamic_cast<ValueSymbol*>(symbol) != nullptr) {
        tempRegisters[dynamic_cast<ValueSymbol*>(symbol)] = reg;
        MipsGenerator::getInstance().add(new Notation("### " + dynamic_cast<ValueSymbol*>(symbol)->name + " -> " + Register::type2str[reg]));
        tempRegisters2[reg] = dynamic_cast<ValueSymbol*>(symbol);
        freeRegs.remove(reg);
    }
    return reg;
}

// 只有一处地方能使用！！！。
void RegisterAlloc::forceAllocRegister(Symbol* symbol, RegType reg) {
    auto oldSymbol = tempRegisters2[reg];
    tempRegisters.erase(oldSymbol);
    tempRegisters[symbol] = reg;
    tempRegisters2[reg] = symbol;
}

void RegisterAlloc::dealWithSymbol(Symbol *symbol, RegType reg, bool fromMemory, BasicBlock* curBlock, int curLine) {
    // 如果不为空，将原来的写回内存中
    // TODO: 地址不写回。
//    if (registerMap[reg] != nullptr) {
//        if (dynamic_cast<ValueSymbol*>(registerMap[reg])) {
//            MipsGenerator::getInstance().add(new Notation("# Register[" + Register::type2str[reg] + "] " + dynamic_cast<ValueSymbol*>(registerMap[reg])->name + " to " + symbol->printMiddleCode()));
//            pushBackToMem(reg, dynamic_cast<ValueSymbol*>(registerMap[reg]));
//        }
//    }

    if (fromMemory) {
        if (dynamic_cast<ValueSymbol*>(symbol)) {
            auto valueSymbol = dynamic_cast<ValueSymbol*>(symbol);
            // reg中存pointer指向的地址
            if (valueSymbol->valueType == ValueType::POINTER) {
//                std::cout << "########!!!!!!" << valueSymbol->name << std::endl;
                auto offset = valueSymbol->getPAddress();   // 偏移地址，或绝对地址
//                std::cout << offset->printMiddleCode() << std::endl;
                if (valueSymbol->isAbsoluteAddress) {
                    if (dynamic_cast<Immediate*>(offset) != nullptr) {
                        MipsGenerator::getInstance().add(new RI(RI::li, reg, dynamic_cast<Immediate*>(offset)->value));
                    }
                    else {
                        // TODO: 或许可以直接放到reg上
                        auto offReg = allocRegister(dynamic_cast<ValueSymbol*>(offset), curBlock, curLine);
                        MipsGenerator::getInstance().add(new RR(RR::move, reg, offReg));
                    }
                }
                else if (dynamic_cast<Immediate*>(offset) != nullptr) {
                    if (valueSymbol->isLocal) {
                        MipsGenerator::getInstance().add(new RRI(RRI::addiu, reg, $sp, -dynamic_cast<Immediate*>(offset)->value));
                        // !!!如果是参数数组的话，需要把地址值加载到目标寄存器中
                        if (valueSymbol->isFParam) {
                            MipsGenerator::getInstance().add(new M(M::lw, reg, 0, reg));
                        }
                    }
                    else {
                        MipsGenerator::getInstance().add(new RRI(RRI::addiu, reg, $gp, dynamic_cast<Immediate*>(offset)->value));
                    }
                }
                else {
                    auto offReg = allocRegister(dynamic_cast<ValueSymbol*>(offset), curBlock, curLine);
                    if (valueSymbol->isLocal) {
                        MipsGenerator::getInstance().add(new RRR(RRR::subu, reg, $sp, offReg));
                    }
                    else {
                        MipsGenerator::getInstance().add(new RRR(RRR::addu, reg, $gp, offReg));
                    }
                }
            }
                // 如果是参数数组，需要先根据其在栈中的偏移得到其参数的地址，然后将其值到寄存器中。
//            else if (valueSymbol->valueType == ValueType::FUNCFPARAM) {
//                auto offset = valueSymbol->getPAddress();
//                if (dynamic_cast<Immediate*>(offset)) {
//                    MipsGenerator::getInstance().add(new M(M::lw, reg, -dynamic_cast<Immediate*>(offset)->value, $sp));
//                }
//                else {
//                    auto offReg = allocRegister(dynamic_cast<ValueSymbol*>(offset));
//                    // TODO: 这里随便设了个寄存器，不知道会不会有问题。
//                    auto tempReg = allocRegister(new NumSymbol(1));
//                    MipsGenerator::getInstance().add(new RRR(RRR::subu, tempReg, $sp, offReg));
//                    MipsGenerator::getInstance().add(new M(M::lw, reg, 0, tempReg));
//                }
//            }
            else {
                if (valueSymbol->isLocal) {
                    // !!!如果是表示地址偏移量的变量，不能将其的值加载到寄存器上。
                    // 当做普通变量处理。
//                    if (valueSymbol->name.rfind("ArraY_*$+|!123___", 0) == 0) {
//                        return;
//                    }
                    MipsGenerator::getInstance().add(new M(M::lw, reg, -valueSymbol->getAddress(), $sp));
                }
                else {
                    MipsGenerator::getInstance().add(new M(M::lw, reg, valueSymbol->getAddress(), $gp));
                }
            }
        }
        else if (dynamic_cast<NumSymbol*>(symbol) != nullptr) {
            auto numSymbol = dynamic_cast<NumSymbol*>(symbol);
            MipsGenerator::getInstance().add(new RI(RI::li, reg, numSymbol->value));
        }
    }
    // 只是将变量映射到寄存器上
//    else {
//        MipsGenerator::getInstance().add(new Notation("### " + symbol->name + " -> " + Register::type2str[reg]));
//    }
}


//RegType RegisterAlloc::findRegister(Symbol *symbol) {
//    for (const auto &pair : registerMap) {
//        if (pair.second == symbol) {
//            return pair.first;
//        }
//    }
//    return RegType::none;
//}

//void RegisterAlloc::saveRegisters() {
//    for (auto pair : registerMap) {
//        // 如果寄存器对应有符号，并且符号是valueSymbol的话
//        // TODO: 如果是地址的话，不需要写回
//        if (pair.second != nullptr && dynamic_cast<ValueSymbol*>(pair.second)) {
//            auto valueSymbol = dynamic_cast<ValueSymbol*>(pair.second);
//            pushBackToMem(pair.first, valueSymbol);
//            registerMap[pair.first] = nullptr;
//        }
//    }
//}



void RegisterAlloc::pushBackToMem(RegType reg, ValueSymbol *valueSymbol) {
    // TODO: 这个貌似有问题，参数可以写回内存的
    // ！！！如果参数是普通变量，那么不写回内存中。错错错！可以写回的。
//    if (valueSymbol->valueType == ValueType::FUNCFPARAM) {
//        return;
//    }
    // 如果是数组成员的话，不写回。
    if (valueSymbol == nullptr) {
        return;
    }
//    if (valueSymbol->name.rfind("ArraY_*$+|!123___", 0) == 0 || valueSymbol->name.rfind("PointeR_*$+|!123___", 0) == 0){
//        return;
//    }
    if (valueSymbol->getDim() > 0) {
        return;
    }
    MipsGenerator::getInstance().add(new Notation("# save to value " + valueSymbol->name));
    if (valueSymbol->isLocal) {
        MipsGenerator::getInstance().add(new M(M::sw, reg, -valueSymbol->getAddress(), $sp));
    }
    else {
        MipsGenerator::getInstance().add(new M(M::sw, reg, valueSymbol->getAddress(), $gp));
    }
}

//void RegisterAlloc::clearAllRegister() {
//    for (auto pair : registerMap) {
//        registerMap[pair.first] = nullptr;
//    }
//}
//
//void RegisterAlloc::clearRegister(RegType reg) {
//    if (registerMap[reg] == nullptr) {
//        return;
//    }
//    else if (dynamic_cast<ValueSymbol*>(registerMap[reg])){
//        pushBackToMem(reg, dynamic_cast<ValueSymbol*>(registerMap[reg]));
//    }
//    registerMap[reg] = nullptr;
//}

//void RegisterAlloc::forceSymbolToRegister(ValueSymbol *valueSymbol, RegType reg) {
//    clearRegister(reg);
//    registerMap[reg] = valueSymbol;
//}

bool RegisterAlloc::hasFreeReg() {
    return !freeRegs.empty();
}

Symbol *RegisterAlloc::OPT(BasicBlock *curBlock, int curLine) {
    Symbol *farthestSymbol = nullptr;
    int farthestPlace = -1;
    auto codes = curBlock->middleCodeItems;
#ifdef DEBUG
    std::cout << "[OPT]tempRegisters size: "<< tempRegisters.size() << std::endl;
#endif
    for (auto pair : tempRegisters) {
        auto curSymbol = pair.first;
        int curPlace = codes.size() + 1;
        // 一直出错，直接暴力了。。。
        // !!!罪魁祸首是死代码删除，导致index变了。
        MiddleCodeItem* startCode = nullptr;
        for (auto code : codes) {
            if (code->index == curLine) {
                startCode = code;
            }
        }
        auto it = codes.find(startCode);
#ifdef DEBUG
        std::cout << "[OPT]curLine: " << curLine << std::endl;
        std::cout << "[OPT]curCode: " << *(*it) << std::endl;
#endif
        auto pos = curLine;

        for (; it != codes.end(); it ++ ) {
            auto code = *it;
//            auto def = code->getDef();
//            auto uses = code->getUse();
//            if (curSymbol == def) {
//                curPlace = pos;
//                break;
//            }
//            for (auto use : *uses) {
//                if (curSymbol == use) {
//                    curPlace = pos;
//                    break;
//                }
//            }
            // TODO: 额外判断数组相关的变量！！！，不能使用getUse, getDef()，考虑数组的成员和数组
            auto src1 = code->_getSrc1();
            auto src2 = code->_getSrc2();
            auto ret = code->_getRet();

//            if (dynamic_cast<ValueSymbol*>(curSymbol)!=nullptr && dynamic_cast<ValueSymbol*>(curSymbol)->name == "ArraY_*$+|!123___q_12"
//                && dynamic_cast<ValueSymbol*>(ret) != nullptr && dynamic_cast<ValueSymbol*>(ret)->name == "ArraY_*$+|!123___q_12") {
//                for (auto x : tempRegisters) {
//                    std::cout << "nitian" << std::endl;
//                    if (dynamic_cast<ValueSymbol*>(x.first) != nullptr)
//                        std::cout << dynamic_cast<ValueSymbol*>(x.first)->name << std::endl;
//                    else if (dynamic_cast<NumSymbol*>(x.first) != nullptr) {
//                        std::cout << dynamic_cast<NumSymbol*>(x.first)->value << std::endl;
//                    }
//                }
//            }

            if (curSymbol == src1 || curSymbol == src2 || curSymbol == ret) {
                curPlace = pos;
                break;
            }

            // 还要包括funcCall
            if (code->codeType == MiddleCodeItem::MiddleFuncCall) {
                auto s = dynamic_cast<class MiddleFuncCall*>(code)->funcRParams;
                for (auto x : s) {
                    if (curSymbol == x) {
                        curPlace = pos;
                        break;
                    }
                }
            }



            // 如果是数字的话，比如MUL 114514 a b。114514是一个NumSymbol，但是在代码中他又是一个Intermediate，其实是两个对象，所以这里特殊处理下。
            if (dynamic_cast<NumSymbol*>(curSymbol) != nullptr) {
                if (dynamic_cast<Immediate*>(src1) != nullptr && dynamic_cast<NumSymbol*>(curSymbol)->value == dynamic_cast<Immediate*>(src1)->value) {
                    curPlace = pos;
                    break;
                }
                if (dynamic_cast<Immediate*>(src2) != nullptr && dynamic_cast<NumSymbol*>(curSymbol)->value == dynamic_cast<Immediate*>(src2)->value) {
                    curPlace = pos;
                    break;
                }
            }
            pos ++ ;
        }

        if (curPlace > farthestPlace) {
            farthestPlace = curPlace;
            farthestSymbol = curSymbol;
        }
        if (farthestPlace == codes.size() + 1) {
            break;
        }
    }
    if (farthestSymbol == nullptr) {
        int a = 1;
    }
    return farthestSymbol;
}

// 保存写回symbol，只针对tempRegisters
void RegisterAlloc::freeSymbolInRegs(Symbol *symbol, bool save) {
    // 如果不是valueSymboi的话，如Numsymbol就直接释放即可。
    if (dynamic_cast<NumSymbol*>(symbol) != nullptr) {
        auto reg = tempRegisters[dynamic_cast<NumSymbol*>(symbol)];
        freeRegister(reg);
    }
    // 如果是ValueSymbol，并且是局部变量或是参数，并且是溢出的，判断是否在overflow中
    else if (colorAllocator->hasOverflow(dynamic_cast<ValueSymbol*>(symbol))) {
        colorAllocator->freeOverflow(dynamic_cast<ValueSymbol*>(symbol));
    }
    if (tempRegisters.count(symbol) != 0) {
//        if (dynamic_cast<ValueSymbol*>(symbol) != nullptr && dynamic_cast<ValueSymbol*>(symbol)->name == "ArraY_*$+|!123___c1_0") {
//            std::cout << "222" << std::endl;
//            std::cout << dynamic_cast<ValueSymbol*>(symbol)->name << std::endl;
//            std::cout << "Test tempRegisters2" << std::endl;
//
//            auto reg = tempRegisters[symbol];
//            std::cout << "1" << std::endl;
//            auto s = tempRegisters2[reg];
//            std::cout << "2" << std::endl;
//            if (dynamic_cast<ValueSymbol*>(s) == nullptr) {
//                std::cout << "3" << std::endl;
//                std::cout << "tempregisters" << std::endl;
//                for (auto pair : tempRegisters) {
//                    auto x = pair.first;
//                    if (dynamic_cast<ValueSymbol*>(x) != nullptr) {
//                        std::cout << x << " " << dynamic_cast<ValueSymbol*>(x) << dynamic_cast<ValueSymbol*>(x)->name << std::endl;
//                    }
////                    std::cout << x->printMiddleCode() << std::endl;
//                }
//                std::cout << "tempregisters2" << std::endl;
//                for (auto pair : tempRegisters2) {
//                    auto x = pair.second;
//                    std::cout << x->printMiddleCode() << std::endl;
//                }
//            }
//            else {
//                std::cout << dynamic_cast<ValueSymbol*>(s)->name << std::endl;
//            }
//
//        }
        auto reg = tempRegisters[symbol];

        freeRegister(reg);
        if (save) {
            pushBackToMem(reg, dynamic_cast<ValueSymbol*>(symbol));
        }
    }
}

// !!!只适用于tempRegisters
void RegisterAlloc::freeRegister(RegType reg) {
    auto symbol = tempRegisters2[reg];
    tempRegisters.erase(symbol);
    tempRegisters2.erase(reg);
//    // 因为有可能会free$zero，所以判断是否在原来的localRegister中。
    auto it = std::find(localRegs.begin(), localRegs.end(), reg);
    if (it != localRegs.end()) {
        freeRegs.push_back(reg);
    }
}

// TEMP 表示是临时变量，或者是函数参数。
// PARAM 表示函数参数。
// GLOBAL 表示全局变量
// SPILL 表示溢出变量。
void RegisterAlloc::freeRegisters(int type, bool save) {
    auto toErase = new std::set<Symbol*>();
    for (auto pair : tempRegisters) {
        auto symbol = pair.first;
        // 如果不是valueSymbol,直接解除对应关系。
        if (dynamic_cast<NumSymbol*>(symbol) != nullptr) {
            toErase->insert(symbol);
            continue;
        }
        auto valueSymbol = dynamic_cast<ValueSymbol*>(symbol);
        if ((type & TYPE_GLOBAL) != 0 && !valueSymbol->isLocal) {
            toErase->insert(symbol);
        }
        else if ((type & TYPE_PARAM) != 0 && valueSymbol->valueType == FUNCFPARAM && !colorAllocator->hasGlobalRegister(valueSymbol)) {
            toErase->insert(symbol);
        }
        else if ((type & TYPE_TEMP) != 0 && !colorAllocator->hasGlobalRegister(valueSymbol)) {
            toErase->insert(symbol);
        }
        else if ((type & TYPE_SPILL) != 0 && (colorAllocator->spillSet.count(valueSymbol) != 0)) {
            toErase->insert(symbol);
        }
    }

    for (auto symbol : *toErase) {
        if (dynamic_cast<NumSymbol*>(symbol) != nullptr) {
            freeSymbolInRegs(symbol, false);
        }
        else if (dynamic_cast<ValueSymbol*>(symbol) != nullptr) {
            freeSymbolInRegs(symbol, save);
        }
    }
}

// 只保存在冲突图中的活跃变量
void RegisterAlloc::saveAllGlobalRegisters(MiddleCodeItem *code) {
    auto activeGlobalSymbols = colorAllocator->getActiveGlobalSymbols(code);
#ifdef DEBUG
    std::cout << "[saveAllGlobalRegisters]" << *code << std::endl;
    for (auto s : *activeGlobalSymbols) {
        std::cout << s->name << std::endl;
    }
#endif
    for (auto pair : colorAllocator->symbolToGlobalRegister) {
        auto symbol = pair.first;
        auto reg = pair.second;
        if (activeGlobalSymbols->count(symbol) == 0) {
            continue;
        }
        pushBackToMem(reg, symbol);
    }

    // 在tempRegisters中的溢出节点
    // !!!freeRegister中会改变tempRegisters
    auto tempSet = std::set<RegType>();
    for (auto pair : colorAllocator->spillToRegister) {
        auto symbol = pair.first;
        auto reg = pair.second;
        if (dynamic_cast<ValueSymbol*>(symbol) == nullptr) {
            continue;
        }
        if (activeGlobalSymbols->count(dynamic_cast<ValueSymbol*>(symbol)) == 0) {
            continue;
        }
        pushBackToMem(reg, dynamic_cast<ValueSymbol*>(symbol));
//        freeRegister(reg);
        tempSet.insert(reg);
    }
    for (auto x : tempSet) {
        freeRegister(x);
    }
    colorAllocator->symbolToGlobalRegister.clear();
    colorAllocator->spillToRegister.clear();
//    tempRegisters.clear();
//    tempRegisters2.clear();
}

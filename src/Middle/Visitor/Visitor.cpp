//
// Created by 安达楷 on 2023/10/4.
//

//#define DEBUG

#include "Visitor.h"
#include "../../Fronted/Lexer/Lexer.h"
#include "../MiddleCode.h"
#include "../Util/Calculate.h"
#include "../../Util/Debug.h"
#include "../../Util/Exception.h"
#include "../Intermediate/Immediate.h"

#include "../../Optimization/Config.h"
#include "../Optimization/NoChangeValue.h"
SymbolTable* Visitor::curTable;
SymbolTable* Visitor::curFuncSymbolTable;

void Visitor::visit() {
    auto compUnit = ast->root;
    visitCompUnit(compUnit);
#ifdef NO_CHANGE_VALUE
    // 翻译完所有中间代码后，将其中变量都没变的替换为值
    NoChangeValue::getInstance().replace(funcs);
#endif
}

void Visitor::visitCompUnit(CompUnit *compUnit){
    curTable = new SymbolTable(nullptr);
    curFuncSymbolTable = new SymbolTable(nullptr);
    for (auto decl : compUnit->decls) {
        visitDecl(decl);
    }
    for (auto funcDef : compUnit->funcDefs) {
        visitFuncDef(funcDef);
    }
    visitFuncDef(compUnit->mainFuncDef);
}

void Visitor::visitDecl(Decl *decl) {
    if (decl->constDecl) {
        visitConstDecl(decl->constDecl);
    }
    else if (decl->varDecl) {
        visitVarDecl(decl->varDecl);
    }
}

void Visitor::visitConstDecl(ConstDecl *constDecl) {
    for (auto constDef : constDecl->constDefs) {
        visitConstDef(constDef);
    }
}

void Visitor::visitConstDef(ConstDef *constDef) {
    Token *ident = constDef->ident;
    std::string name = ident->content;
    // 如果当前的符号表中有同名的变量，就会报错
    // b
    if (curTable->contain(name, false)) {
        ErrorTable::getInstance().addError(new Error(Error::DUPLICATE_IDENT, ident->line));
        return;
    }

    if (!constDef->isArray()) {
        // 非数组且初始化，形如const int a = 10;
        // 初始值
        auto constInitVal = Calculate::calcConstExp(constDef->constInitVal->constExp);
        // TODO:添加值 DONE
        auto valueSymbol = new ValueSymbol(name, constInitVal, true);
        curTable->add(valueSymbol);
        curFuncSymbolTable->add(valueSymbol);
        updateCurStackSize(valueSymbol);
        if (curBlockLevel == 0) {
            valueSymbol->setLocal(false);
            valueSymbol->setAddress(valueSymbol->getAddress() - valueSymbol->getSize());
            MiddleCode::getInstance().addGlobalValues(valueSymbol);
        }
        else {
            // TODO: 处理局部常量 DONE
            // const int a = 10;
//            valueSymbol->setLocal(true);
            auto assign = new MiddleDef(MiddleDef::DEF_VAR, valueSymbol, new Immediate(constInitVal));
            curBlock->add(assign);
        }
    }
    else {
        // 数组且初始化，形如const int a[10] = {};
        // TODO:具体维度 Done
        std::vector<int> dims;  //之后通过constInitVal得到
        for (auto x : constDef->constExps) {
            dims.push_back(Calculate::calcConstExp(x));
        }
        auto arraySymbol = new ValueSymbol(name, dims, true);

        curTable->add(arraySymbol);
        curFuncSymbolTable->add(arraySymbol);
        updateCurStackSize(arraySymbol);
        if (dims.size() == 1) {
            // 一维数组
            auto constExps = flattenConstInitVal(constDef->constInitVal);
            std::vector<int> initValues;
            initValues.reserve(constExps.size());
            for (auto constExp : constExps) {
                initValues.push_back(Calculate::calcConstExp(constExp));
            }
            arraySymbol->setInitValues(initValues);
            if (curBlockLevel == 0) {       // 如果是全局变量的话
                arraySymbol->setLocal(false);
                arraySymbol->setAddress(arraySymbol->getAddress() - arraySymbol->getSize());
                MiddleCode::getInstance().addGlobalValues(arraySymbol);
            }
            else {      // 如果不是全局变量的话，每个initvalue生成中间代码
                // TODO: 局部数组 DONE
//                arraySymbol->setLocal(true);
                auto defArray = new MiddleDef(MiddleDef::DEF_ARRAY, arraySymbol);
                curBlock->add(defArray);
                int offsetCount = 0;
                for (auto x : initValues) {
                    auto tmp = new ValueSymbol(getTempName(), ValueType::TEMP);
                    curFunc->tempSymbols.push_back(tmp);
                    auto offset = new MiddleOffset(arraySymbol, new Immediate(offsetCount * sizeof(int)), tmp);
                    auto store = new MiddleMemoryOp(MiddleMemoryOp::STORE, new Immediate(x), tmp);
                    curBlock->add(offset);
                    curBlock->add(store);
                    offsetCount ++ ;
                }
                auto endArray = new MiddleDef(MiddleDef::END_ARRAY, arraySymbol);
                curBlock->add(endArray);
            }
        }
        else if (dims.size() == 2) {
            // 二维数组
            std::vector<int> initValues;
            auto constExps = flattenConstInitVal(constDef->constInitVal);
            initValues.reserve(constExps.size());
            for (auto x : constExps) {
                initValues.push_back(Calculate::calcConstExp(x));
            }
            arraySymbol->setInitValues(initValues);
            if (curBlockLevel == 0) {
                // 全局数组
                arraySymbol->setLocal(false);
                arraySymbol->setAddress(arraySymbol->getAddress() - arraySymbol->getSize());
                MiddleCode::getInstance().addGlobalValues(arraySymbol);
            }
            else {
                // TODO: 局部数组 DONE
                auto defArray = new MiddleDef(MiddleDef::DEF_ARRAY, arraySymbol);
                curBlock->add(defArray);
                int offsetCount = 0;
                for (auto x : initValues) {
                    auto tmp = new ValueSymbol(getTempName(), ValueType::TEMP);
                    curFunc->tempSymbols.push_back(tmp);
                    auto offset = new MiddleOffset(arraySymbol, new Immediate(offsetCount * sizeof(int)), tmp);
                    auto store = new MiddleMemoryOp(MiddleMemoryOp::STORE, new Immediate(x), tmp);
                    curBlock->add(offset);
                    curBlock->add(store);
                    offsetCount ++ ;
                }
                auto endArray = new MiddleDef(MiddleDef::END_ARRAY, arraySymbol);
                curBlock->add(endArray);
            }
        }
    }
}

void Visitor::visitConstInitVal(ConstInitVal *constInitVal) {
    ;
}

void Visitor::visitConstExp(ConstExp *constExp) {
    return;
}

Intermediate *Visitor::visitAddExp(AddExp *addExp) {

    auto res = visitMulExp(addExp->mulExps[0]);

    for (int i = 0; i < addExp->ops.size(); i ++ ) {
        auto op = addExp->ops[i];
        auto middleCodeType = op->tokenType==Token::PLUS ? MiddleBinaryOp::ADD : MiddleBinaryOp::SUB;

        auto src1 = res;
        auto src2 = visitMulExp(addExp->mulExps[i + 1]);

        res = new ValueSymbol(getTempName(), ValueType::TEMP);
        curFunc->tempSymbols.push_back(dynamic_cast<ValueSymbol*>(res));
        auto middleCode = new MiddleBinaryOp(middleCodeType, src1, src2, res);
        curBlock->add(middleCode);
    }
    return res;
}

Intermediate *Visitor::visitMulExp(MulExp *mulExp) {
    auto res = visitUnaryExp(mulExp->unaryExps[0]);
    for (int i = 0; i < mulExp->ops.size(); i ++ ) {
        auto op = mulExp->ops[i];
        MiddleBinaryOp::Type middleCodeType;
        switch (op->tokenType) {
            case Token::MULT:
                middleCodeType = MiddleBinaryOp::MUL;
                break;
            case Token::DIV:
                middleCodeType = MiddleBinaryOp::DIV;
                break;
            case Token::MOD:
                middleCodeType = MiddleBinaryOp::MOD;
                break;
            default:
                middleCodeType = MiddleBinaryOp::ERROR;
        }

        auto src1 = res;
        auto src2 = visitUnaryExp(mulExp->unaryExps[i + 1]);

        res = new ValueSymbol(getTempName(), ValueType::TEMP);
        curFunc->tempSymbols.push_back(dynamic_cast<ValueSymbol*>(res));
        auto middleCode = new MiddleBinaryOp(middleCodeType, src1, src2, res);
        curBlock->add(middleCode);
    }
    return res;
}

Intermediate *Visitor::visitUnaryExp(UnaryExp *unaryExp) {
    auto primaryExp = unaryExp->primaryExp;
    auto ident = unaryExp->ident;
    auto funcRParams = unaryExp->funcRParams;
    auto inUnaryExp = unaryExp->unaryExp;

    if (primaryExp != nullptr) {
        return visitPrimaryExp(primaryExp);
    }
    else if (inUnaryExp != nullptr) {
        // TODO: unaryExp的符号 Done
        auto src = visitUnaryExp(inUnaryExp);
        auto op = unaryExp->unaryOp;
        MiddleUnaryOp::Type middleCodeType;
        switch (op->unaryOp->tokenType) {
            case Token::PLUS:
                middleCodeType = MiddleUnaryOp::POSITIVE;
                break;
            case Token::MINU:
                middleCodeType = MiddleUnaryOp::NEGATIVE;
                break;
            case Token::NOT:
                middleCodeType = MiddleUnaryOp::NOT;
                break;
            default:
                middleCodeType = MiddleUnaryOp::ERROR;
        }
        auto res = new ValueSymbol(getTempName(), ValueType::TEMP);
        curFunc->tempSymbols.push_back(dynamic_cast<ValueSymbol*>(res));
        auto middleCode = new MiddleUnaryOp(middleCodeType, res, src);
        curBlock->add(middleCode);
        return res;
    }
    //函数调用：在所属符号表中查找
    else if (ident != nullptr) {
        std::string name = ident->content;
        // Error c
        // 未定义
        if (!curTable->contain(name, true)) {
            auto error = new Error(Error::UNDEFINED_IDENT, ident->line);
            ErrorTable::getInstance().addError(error);
            return nullptr;
        }
        auto funcSymbol = dynamic_cast<FuncSymbol*>(curTable->getSymbol(name, true));
        auto funcCall = new MiddleFuncCall(name);
        visitFuncRParams(funcRParams, funcSymbol, ident->line, funcCall);
        curBlock->add(funcCall);
        // 如果是int的话，需要有返回值
        if (funcSymbol->basicType == INT) {
            auto ret = new ValueSymbol(getTempName(), ValueType::TEMP);
            funcCall->ret = ret;
            curFunc->tempSymbols.push_back(ret);
            return ret;
        }
    }
    return nullptr;
}

Intermediate * Visitor::visitPrimaryExp(PrimaryExp *primaryExp) {
    auto exp = primaryExp->exp;
    auto lVal = primaryExp->lVal;
    auto number = primaryExp->number;

    if (exp != nullptr) {
        return visitExp(exp);
    }
    else if (lVal != nullptr) {
        return visitLVal(lVal);
    }
    else if (number != nullptr) {
        return visitNumber(number);
    }
    return nullptr;
}

Intermediate *Visitor::visitExp(Exp *exp) {
#ifdef CONSTANT_CALCULATE
    // 尝试计算
    try {
        int x = Calculate::calcExp(exp);
        return new Immediate(x);
    } catch (MyError&) {}
#endif
    return visitAddExp(exp->addExp);
}


Intermediate * Visitor::visitLVal(LVal *lVal, bool isLeft) {
    auto ident = lVal->ident;
    std::string name = ident->content;
    // Error c
    if (!curTable->contain(name, true)) {
        auto error = new Error(Error::UNDEFINED_IDENT, ident->line);
        ErrorTable::getInstance().addError(error);
        return nullptr;
    }

    auto symbol = curTable->getSymbol(name, true);

    // 如果是变量
    // 注意，不能只根据符号表中的维度来判断，还需要根据使用时用了几个维度来判断
    auto valueSymbol = dynamic_cast<ValueSymbol*>(symbol);
    if (valueSymbol != nullptr) {
        int symbolDim = -1;
        symbolDim = valueSymbol->getDim();  // 符号表中该变量的维度
        int realDim = lVal->exps.size();    // 所使用的维度

        // 形如int a; a;
        if (symbolDim == 0) {
#ifdef NO_CHANGE_VALUE
            if (isLeft) {
                NoChangeValue::getInstance().remove(valueSymbol);
            }
#endif
            return valueSymbol;
        }
        else if (symbolDim == 1) {
            // TODO：这里应该是返回地址 DONE
            // TODO: 要考虑是参数数组的情况。DONE
            // 形如int a[10]; a;
            if (realDim == 0) {
                auto pointerName = "P_" + valueSymbol->name;
                auto pointer = new ValueSymbol(ValueType::POINTER, pointerName,new Immediate(valueSymbol->getAddress()), valueSymbol->isLocal);
                if (valueSymbol->valueType == ValueType::FUNCFPARAM) {
                    pointer->setFParam();
                }
                return pointer;
            }
            else if (realDim == 1) {
                auto index = visitExp(lVal->exps[0]);
                Intermediate *offsetCount = getOffsetCount(index);

                auto tmp = new ValueSymbol(getTempName(), ValueType::TEMP);
                curFunc->tempSymbols.push_back(tmp);

                auto offset = new MiddleOffset(valueSymbol, offsetCount, tmp);
                // 如果是在等式左边的话，直接返回了
                if (isLeft) {
                    curBlock->add(offset);
                    return tmp;
                }
                auto ret = new ValueSymbol(getTempName(), ValueType::TEMP);
                curFunc->tempSymbols.push_back(ret);
                auto load = new MiddleMemoryOp(MiddleMemoryOp::LOAD, ret, tmp);
                curBlock->add(offset);
                curBlock->add(load);
                return ret;
            }
        }
        else if (symbolDim == 2) {
            // TODO: 返回地址 DONE
            if (realDim == 0) {
                auto pointerName = "P_" + valueSymbol->name;
                auto pointer = new ValueSymbol(ValueType::POINTER, pointerName, new Immediate(valueSymbol->getAddress()), valueSymbol->isLocal);
                if (valueSymbol->valueType == ValueType::FUNCFPARAM) {
                    pointer->setFParam();
                }
                return pointer;
            }
            // int a[2][2]; func(a[1]);
            // TODO: 可能会出现a[2][2], 传参使用a[x], a[1]的情况
            // !!! 参数数组的话，添加middleoffset
            else if (realDim == 1){
                auto pointerName = "P_" + valueSymbol->name;
                auto index = visitExp(lVal->exps[0]);

                auto tmp = new ValueSymbol(getTempName(), ValueType::TEMP);
                curFunc->tempSymbols.push_back(tmp);
                if (dynamic_cast<Immediate*>(index)){
                    auto offsetCode = new MiddleOffset(valueSymbol, new Immediate(dynamic_cast<Immediate*>(index)->value * valueSymbol->dims[1] * sizeof(int)), tmp);
                    DEBUG_PRINT_DIRECT("!!!!!!!!!!!!");
                    DEBUG_PRINT_DIRECT(dynamic_cast<Immediate*>(index)->value * valueSymbol->dims[1] * sizeof(int));
                    curBlock->add(offsetCode);
                    // tmp得到的是绝对地址
                    auto pointer = new ValueSymbol(true, ValueType::POINTER, pointerName, tmp, valueSymbol->isLocal);
                    if (valueSymbol->valueType == ValueType::FUNCFPARAM) {
                        pointer->setFParam();
                    }
                    return pointer;
                }
                else {
                    auto mulCode = new MiddleBinaryOp(MiddleBinaryOp::MUL, dynamic_cast<ValueSymbol*>(index), new Immediate(valueSymbol->dims[1] * sizeof(int)), tmp);
                    auto offsetCode = new MiddleOffset(valueSymbol, tmp, tmp);
                    curBlock->add(mulCode);
                    curBlock->add(offsetCode);
                    auto pointer = new ValueSymbol(true, ValueType::POINTER, pointerName, tmp, valueSymbol->isLocal);
                    if (valueSymbol->valueType == ValueType::FUNCFPARAM) {
                        pointer->setFParam();
                    }
                    return pointer;
                }
                // 如果是a[1]的传参
//                if (dynamic_cast<Immediate*>(index)) {
//                    int offset = dynamic_cast<Immediate*>(index)->value * valueSymbol->dims[1] * sizeof(int);
//                    DEBUG_PRINT("index value is %d, offset is %d, valueSymbol off is %d\n", dynamic_cast<Immediate*>(index)->value, offset, valueSymbol->getAddress());
//                    if (valueSymbol->isLocal) {
//                        auto pointer = new ValueSymbol(ValueType::POINTER, pointerName, new Immediate(valueSymbol->getAddress() - offset), valueSymbol->isLocal);
//                        if (valueSymbol->valueType == ValueType::FUNCFPARAM) {
//                            pointer->setFParam();
//                        }
//                        return pointer;
//                    }
//                    else {
//                        auto pointer = new ValueSymbol(ValueType::POINTER, pointerName, new Immediate(valueSymbol->getAddress() + offset), valueSymbol->isLocal);
//                        if (valueSymbol->valueType == ValueType::FUNCFPARAM) {
//                            pointer->setFParam();
//                        }
//                        return pointer;
//                    }
//                }
//                // 如果是a[x]的传参
//                else {
//                    auto tmp = new ValueSymbol(getTempName(), TEMP);
//                    auto address = new ValueSymbol(getTempName(), TEMP);
//                    curFunc->tempSymbols.push_back(tmp);
//                    curFunc->tempSymbols.push_back(address);
//                    curBlock->add(new MiddleBinaryOp(MiddleBinaryOp::MUL, index, new Immediate(valueSymbol->dims[1] * sizeof(int)), tmp));
//                    // 注意！！！由于我的傻逼设置，局部数组的数组头在上面，全局数组的数组头在下面
//                    if (valueSymbol->isLocal) {
//                        curBlock->add(new MiddleBinaryOp(MiddleBinaryOp::SUB, new Immediate(valueSymbol->getAddress()), tmp, address));
//                    }
//                    else{
//                        curBlock->add(new MiddleBinaryOp(MiddleBinaryOp::ADD, new Immediate(valueSymbol->getAddress()), tmp, address));
//                    }
//                    auto pointer = new ValueSymbol(ValueType::POINTER, pointerName, address, valueSymbol->isLocal);
//                    if (valueSymbol->valueType == ValueType::FUNCFPARAM) {
//                        pointer->setFParam();
//                    }
//                    return pointer;
//                }
            }
            else if (realDim == 2) {
                auto row = visitExp(lVal->exps[0]);
                auto col = visitExp(lVal->exps[1]);

                // 如果row是立即数，就直接相乘，如果不是，就生成中间代码
                // TODO: 这里或许还能优化，直接替换为常数，但感觉得大改
                Intermediate * offsetNum = nullptr;
                if (dynamic_cast<Immediate*>(row)) {
                    int index1 = dynamic_cast<Immediate*>(row)->value * valueSymbol->dims[1];
                    if (dynamic_cast<Immediate*>(col)) {
                        int index2 = dynamic_cast<Immediate*>(col)->value;
                        offsetNum = new Immediate(index1 + index2);
                    }
                    else {
                        auto tmp = new ValueSymbol(getTempName(), ValueType::TEMP);
                        curFunc->tempSymbols.push_back(tmp);
                        auto add = new MiddleBinaryOp(MiddleBinaryOp::ADD, new Immediate(index1), col, tmp);
                        curBlock->add(add);
                        offsetNum = tmp;
                    }
                }
                else if (dynamic_cast<ValueSymbol*>(row)) {
                    auto tmp = new ValueSymbol(getTempName(), ValueType::TEMP);
                    curFunc->tempSymbols.push_back(tmp);
                    auto mul = new MiddleBinaryOp(MiddleBinaryOp::MUL, row, new Immediate(valueSymbol->dims[1]), tmp);
                    curBlock->add(mul);
                    if (dynamic_cast<Immediate*>(col)) {
                        int index2 = dynamic_cast<Immediate*>(col)->value;
                        auto tmp2 = new ValueSymbol(getTempName(), ValueType::TEMP);
                        curFunc->tempSymbols.push_back(tmp);
                        auto add = new MiddleBinaryOp(MiddleBinaryOp::ADD, tmp, new Immediate(index2), tmp2);
                        curBlock->add(add);
                        offsetNum = tmp2;
                    }
                    else {
                        auto tmp2 = new ValueSymbol(getTempName(), ValueType::TEMP);
                        curFunc->tempSymbols.push_back(tmp);
                        auto add = new MiddleBinaryOp(MiddleBinaryOp::ADD, tmp, col, tmp2);
                        curBlock->add(add);
                        offsetNum = tmp2;
                    }
                }

                auto offsetCount = getOffsetCount(offsetNum);

                auto tmp = new ValueSymbol(getTempName(), ValueType::TEMP);
                curFunc->tempSymbols.push_back(tmp);
                auto offset = new MiddleOffset(valueSymbol, offsetCount, tmp);
                // 等式左边的话直接返回
                if (isLeft) {
                    curBlock->add(offset);
                    return tmp;
                }
                auto ret = new ValueSymbol(getTempName(), ValueType::TEMP);
                curFunc->tempSymbols.push_back(ret);
                auto load = new MiddleMemoryOp(MiddleMemoryOp::LOAD, ret, tmp);
                curBlock->add(offset);
                curBlock->add(load);
                return ret;
            }
        }
    }
    // 如果是立即数的话
    else if (dynamic_cast<Immediate*>(symbol) != nullptr) {
        auto immediateSymbol = dynamic_cast<Immediate*>(symbol);
        return immediateSymbol;
    }
    // 如果是函数的参数，不知道怎么处理比较好，就直接重新处理一编吧
//    else {
//        DEBUG_PRINT_DIRECT("use fparam");
//        auto fParamSymbol = dynamic_cast<FuncFParamSymbol*>(symbol);
//        int dim = -1;
//        if (fParamSymbol != nullptr)
//            dim = fParamSymbol->getDim();
//        if (dim == 0) {
//            DEBUG_PRINT_DIRECT("111");
//            return fParamSymbol;
//        }
//        else if (dim == 1) {
//            int offsetCount = Calculate::calcExp(lVal->exps[0]);
//            auto tmp = new ValueSymbol(getTempName(), ValueType::TEMP);
//            curFunc->tempSymbols.push_back(tmp);
//            auto offset = new MiddleOffset(fParamSymbol, new Immediate(offsetCount * sizeof(int)), tmp);
//            auto ret = new ValueSymbol(getTempName(), ValueType::TEMP);
//            curFunc->tempSymbols.push_back(ret);
//            auto load = new MiddleMemoryOp(MiddleMemoryOp::LOAD, tmp, ret);
//            curBlock->add(offset);
//            curBlock->add(load);
//            return ret;
//        }
//        else if (dim == 2) {
//            int row = Calculate::calcExp(lVal->exps[0]);
//            int col = Calculate::calcExp(lVal->exps[1]);
//            int offsetCount = row * fParamSymbol->dims[1] + col;
//
//            auto tmp = new ValueSymbol(getTempName(), ValueType::TEMP);
//            curFunc->tempSymbols.push_back(tmp);
//            auto offset = new MiddleOffset(fParamSymbol, new Immediate(offsetCount * sizeof(int)), tmp);
//            auto ret = new ValueSymbol(getTempName(), ValueType::TEMP);
//            curFunc->tempSymbols.push_back(ret);
//            auto load = new MiddleMemoryOp(MiddleMemoryOp::LOAD, tmp, ret);
//            curBlock->add(offset);
//            curBlock->add(load);
//            return ret;
//        }
//    }
    return nullptr;
}

void Visitor::visitVarDecl(VarDecl *varDecl) {
    for (auto x : varDecl->varDefs) {
        visitVarDef(x);
    }
}

void Visitor::visitVarDef(VarDef *varDef) {
    auto ident = varDef->ident;
    std::string name = ident->content;
    if (curTable->contain(name, false)) {
        auto error = new Error(Error::DUPLICATE_IDENT, ident->line);
        ErrorTable::getInstance().addError(error);
    }

    if (!varDef->isArray()) {
        //如果不是数组并且没有初始化, int a;
        if (!varDef->isInit()) {
            auto symbol = new ValueSymbol(name);
            curTable->add(symbol);
            curFuncSymbolTable->add(symbol);
            updateCurStackSize(symbol);
            if (curBlockLevel == 0) {
                // 全局变量
#ifdef NO_CHANGE_VALUE
                NoChangeValue::getInstance().add(symbol, 0);
#endif
                symbol->setLocal(false);
                symbol->setAddress(symbol->getAddress() - symbol->getSize());
                MiddleCode::getInstance().addGlobalValues(symbol);
            }
            else {
                // TODO:局部变量 DONE
                symbol->setLocal(true);
                auto def = new MiddleDef(MiddleDef::DEF_VAR, symbol);
                curBlock->add(def);
            }
        }
        //如果不是数组但是初始化了，int a = 10;
        else {
            // 全局变量
            if (curBlockLevel == 0) {
                int value = Calculate::calcExp(varDef->initval->exp);
                auto symbol = new ValueSymbol(name, value, false);
#ifdef NO_CHANGE_VALUE
                NoChangeValue::getInstance().add(symbol, value);
#endif
                curTable->add(symbol);
                curFuncSymbolTable->add(symbol);
                updateCurStackSize(symbol);
                symbol->setLocal(false);
                symbol->setAddress(symbol->getAddress() - symbol->getSize());
                MiddleCode::getInstance().addGlobalValues(symbol);
            }
            else {
                // TODO: 局部变量，运行时计算，即输出中间表达式，在visitExp后输出的可能是值，也可能是valueSymbol, DONE
                auto symbol = new ValueSymbol(name);
                curTable->add(symbol);
                curFuncSymbolTable->add(symbol);
                updateCurStackSize(symbol);
                symbol->setLocal(true);

                auto sym = visitExp(varDef->initval->exp);
                // 如果visitExp的值是立即数
                if (dynamic_cast<Immediate*>(sym) != nullptr) {
                    DEBUG_PRINT_DIRECT("[visitVarDef] Immediate");
#ifdef NO_CHANGE_VALUE
                    NoChangeValue::getInstance().add(symbol, dynamic_cast<Immediate *>(sym)->value);
#endif
                    auto def = new MiddleDef(MiddleDef::DEF_VAR, symbol, dynamic_cast<Immediate*>(sym));
                    curBlock->add(def);
                }
                // 如果是ValueSymbol，即 DEF_VAR b a，将b的值赋给a，但由于b的值还未确定
                else {
                    DEBUG_PRINT_DIRECT("[visitVarDef] ValueSymbol");
                    auto def = new MiddleDef(MiddleDef::DEF_VAR, symbol, dynamic_cast<ValueSymbol*>(sym));
                    curBlock->add(def);
                }
            }
        }
    }
    else {
        // TODO:具体维度 Done
        std::vector<int> dims;  //之后通过constInitVal得到
        for (auto x : varDef->constExps) {
            dims.push_back(Calculate::calcConstExp(x));
        }
        auto arraySymbol = new ValueSymbol(name, dims, false);
        curTable->add(arraySymbol);
        curFuncSymbolTable->add(arraySymbol);
        updateCurStackSize(arraySymbol);
        // 一维数组
        if (dims.size() == 1) {
            // 如果是一维数组并且未初始化
            if (!varDef->isInit()) {
                // 是全局变量且未初始化
                if (curBlockLevel == 0) {
                    arraySymbol->setLocal(false);
                    arraySymbol->setAddress(arraySymbol->getAddress() - arraySymbol->getSize());
                    std::vector<int> initValues(arraySymbol->getFlattenDim(), 0);
                    arraySymbol->setInitValues(initValues);
                    MiddleCode::getInstance().addGlobalValues(arraySymbol);
                }
                else {
                    // TODO: 局部变量且未初始化 DONE
                    arraySymbol->setLocal(true);
                    auto def = new MiddleDef(MiddleDef::DEF_ARRAY, arraySymbol);
                    curBlock->add(def);
                    auto endDef = new MiddleDef(MiddleDef::END_ARRAY, arraySymbol);
                    curBlock->add(endDef);
                }
            }
            // 是一维数组并且初始化
            else {
                auto exps = flattenInitVal(varDef->initval);
                // 全局变量且且初始化
                if (curBlockLevel == 0) {
                    arraySymbol->setLocal(false);
                    arraySymbol->setAddress(arraySymbol->getAddress() - arraySymbol->getSize());
                    std::vector<int> initValues;
                    initValues.reserve(exps.size());
                    for (int i = 0; i < dims[0]; i ++ ) {
                        if (i >= exps.size())
                            initValues.push_back(0);
                        else
                            initValues.push_back(Calculate::calcExp(exps[i]));
                    }
                    arraySymbol->setInitValues(initValues);
                    MiddleCode::getInstance().addGlobalValues(arraySymbol);
                }
                else {
                    // TODO: 局部数组并且初始化 DONE
                    arraySymbol->setLocal(true);
                    auto def = new MiddleDef(MiddleDef::DEF_ARRAY, arraySymbol);
                    curBlock->add(def);
                    int offsetCount = 0;
                    for (auto exp : exps) {
                        auto sym = visitExp(exp);
                        DEBUG_PRINT_DIRECT("[visitVarDef] Immediate");
                        auto tmp = new ValueSymbol(getTempName(), ValueType::TEMP);
                        curFunc->tempSymbols.push_back(tmp);
                        auto offset = new MiddleOffset(arraySymbol, new Immediate(offsetCount * sizeof(int)), tmp);
                        auto store = new MiddleMemoryOp(MiddleMemoryOp::STORE, sym, tmp);
                        curBlock->add(offset);
                        curBlock->add(store);
                        offsetCount ++ ;
                    }
                    auto endDef = new MiddleDef(MiddleDef::END_ARRAY, arraySymbol);
                    curBlock->add(endDef);
                }
            }
        }
        // 二维数组
        else if (dims.size() == 2) {
            // 二维数组并且未初始化
            if (!varDef->isInit()) {
                if (curBlockLevel == 0) {
                    std::vector<int> initValues(dims[0]*dims[1], 0);
                    arraySymbol->setInitValues(initValues);
                    arraySymbol->setLocal(false);
                    arraySymbol->setAddress(arraySymbol->getAddress() - arraySymbol->getSize());
                    MiddleCode::getInstance().addGlobalValues(arraySymbol);
                }
                else {
                    // TODO: 局部数组未初始化 DONE
                    auto def = new MiddleDef(MiddleDef::DEF_ARRAY, arraySymbol);
                    curBlock->add(def);
                    auto endDef = new MiddleDef(MiddleDef::END_ARRAY, arraySymbol);
                    curBlock->add(endDef);
                }
            }
            // 二维数组且初始化
            else {
                auto exps = flattenInitVal(varDef->initval);
                // 全局二维数组
                if (curBlockLevel == 0) {
                    arraySymbol->setLocal(false);
                    arraySymbol->setAddress(arraySymbol->getAddress() - arraySymbol->getSize());
                    std::vector<int> initValues;
                    initValues.reserve(exps.size());
                    for (auto x : exps) {
                        initValues.push_back(Calculate::calcExp(x));
                    }
                    arraySymbol->setInitValues(initValues);
                    MiddleCode::getInstance().addGlobalValues(arraySymbol);
                }
                else {
                    // TODO: 局部数组 DONE
                    auto def = new MiddleDef(MiddleDef::DEF_ARRAY, arraySymbol);
                    curBlock->add(def);
                    int offsetCount = 0;
                    for (auto exp : exps) {
                        auto sym = visitExp(exp);
                        auto tmp = new ValueSymbol(getTempName(), ValueType::TEMP);
                        curFunc->tempSymbols.push_back(tmp);
                        auto offset = new MiddleOffset(arraySymbol, new Immediate(offsetCount * sizeof(int)), tmp);
                        auto store = new MiddleMemoryOp(MiddleMemoryOp::STORE, sym, tmp);
                        curBlock->add(offset);
                        curBlock->add(store);
                        offsetCount ++ ;
                    }
                    auto endDef = new MiddleDef(MiddleDef::END_ARRAY, arraySymbol);
                    curBlock->add(endDef);
                }
            }
        }
    }
}

void Visitor::visitInitVal(InitVal *initVal) {
    if (initVal->isArray()) {
        for (auto x : initVal->initVals) {
            visitInitVal(x);
        }
    }
    else {
        visitExp(initVal->exp);
    }
}

Intermediate * Visitor::visitNumber(Number *number) {
    auto intConst = number->intConst;
    int x = intConst->val;
    return new Immediate(x);
}

void Visitor::visitFuncDef(FuncDef *funcDef) {
    auto ident = funcDef->ident;
    std::string name = ident->content;
    // Error b
    // 函数名相同，注意还是要插入的，处理函数体内。
    // *助教在讨论区说不处理重名内部的错误了，那就直接跳过。
    if (curTable->contain(name, false)) {
        auto error = new Error(Error::DUPLICATE_IDENT, ident->line);
        ErrorTable::getInstance().addError(error);
//        name += "_error_" + std::to_string(++unique);
        return;
    }

    auto middleFunc = new Func(Func::DEF_FUNC, name);
    curFuncSymbolTable = middleFunc->funcSymbolTable;
    curFunc = middleFunc;
    funcs.push_back(curFunc);

    curStackSize = 0;

    auto funcBlock = new BasicBlock(BasicBlock::FUNC);
    curBlock = funcBlock;
    // 维护符号表
    curTable = new SymbolTable(curTable);
    auto retType = funcDef->funcType->funcType->tokenType == Token::VOIDTK ? BasicType::VOID : BasicType::INT;
    middleFunc->hasReturn = !(retType == VOID);

    auto funcSymbol = new FuncSymbol(name, retType, funcDef->getFuncFParamNumber());

    if (funcDef->funcFParams != nullptr) {
        for (auto x : funcDef->funcFParams->funcFParams) {
            visitFuncFParam(x, funcSymbol);
        }
    }
    curTable->parent->add(funcSymbol);

    visitBlock(funcDef->block, false);

    BlockItem *lastBlockItem;
    if (!funcDef->block->blockItems.empty()) {
        lastBlockItem = funcDef->block->blockItems.back();
    }
    else {
        lastBlockItem = nullptr;
    }
    // 如果是void的函数也需要有返回return;
    if (retType == BasicType::VOID && (lastBlockItem == nullptr || lastBlockItem->decl != nullptr ||
        (lastBlockItem->stmt != nullptr && lastBlockItem->stmt->returnStmt == nullptr))) {
        curBlock->add(new MiddleReturn());
    }
    middleFunc->setFuncBlock(funcBlock);
    MiddleCode::getInstance().addFuncs(middleFunc);

    // Error g
    // 如果retType不为VOID，且最后一行没有return
    // 错误行号是函数结尾的}所在行
    if (retType != BasicType::VOID && (lastBlockItem == nullptr || lastBlockItem->decl != nullptr ||
            (lastBlockItem->stmt != nullptr && lastBlockItem->stmt->returnStmt == nullptr))) {
        auto error = new Error(Error::LACK_RETURN, funcDef->block->Rbrace->line);
        ErrorTable::getInstance().addError(error);
    }
    // Error f
    // 如果retType为VOID，且最后一行有return并且有返回值
    // 报错行号为return所在行号
    if (retType == BasicType::VOID &&
        (lastBlockItem != nullptr && lastBlockItem->stmt != nullptr &&
        lastBlockItem->stmt->returnStmt != nullptr && lastBlockItem->stmt->returnStmt->exp != nullptr)) {
        auto error = new Error(Error::VOID_MISMATCH_RETURN, lastBlockItem->stmt->returnStmt->returnTK->line);
        ErrorTable::getInstance().addError(error);
    }
    curTable = curTable->parent;
}

void Visitor::updateCurStackSize(ValueSymbol *valueSymbol) {
    curStackSize += valueSymbol->getSize();
    valueSymbol->setAddress(curStackSize);
}

void Visitor::visitFuncFParam(FuncFParam *funcFParam, FuncSymbol *funcSymbol) {
    auto ident = funcFParam->ident;
    std::string name = ident->content;
    // Error b
    if (curTable->contain(name, false)) {
        auto error = new Error(Error::DUPLICATE_IDENT, ident->line);
        ErrorTable::getInstance().addError(error);
        return;
    }

    // TODO: 维度的具体值的计算 DONE
    std::vector<int> dims;
    ValueSymbol * funcFParamSymbol = nullptr;
    if (funcFParam->isArray) {
        dims.push_back(0);
        for (auto x : funcFParam->constExps) {
            dims.push_back(Calculate::calcConstExp(x));
        }
        funcFParamSymbol = new ValueSymbol(ValueType::FUNCFPARAM, name, dims);
        curTable->add(funcFParamSymbol);

        // 添加进入该函数的符号表中，计算在符号表中的偏移
        curFuncSymbolTable->add(funcFParamSymbol);
        updateCurStackSize(funcFParamSymbol);
        funcFParamSymbol->setLocal(true);

        funcSymbol->addFuncFParamSymbol(funcFParamSymbol);
    }
    else {
        // 参数的普通变量
        funcFParamSymbol = new ValueSymbol(ValueType::SINGLE, name, dims);
        curTable->add(funcFParamSymbol);
        curFuncSymbolTable->add(funcFParamSymbol);
        updateCurStackSize(funcFParamSymbol);
        funcFParamSymbol->setLocal(true);
        funcSymbol->addFuncFParamSymbol(funcFParamSymbol);
    }
    auto middleCode = new MiddleFuncParam(MiddleFuncParam::PARAM, funcFParamSymbol);
    curBlock->add(middleCode);
}

// 如果是函数的{}，那由于需要考虑形参的影响，就不重新建符号表了
void Visitor::visitBlock(Block *block, bool toNew) {
    curBlockLevel ++ ;
    if (toNew) {
        curTable = new SymbolTable(curTable);
    }
    for (auto blockItem : block->blockItems) {
        if (blockItem->decl != nullptr)
            visitDecl(blockItem->decl);
        else if (blockItem->stmt != nullptr) {
            visitStmt(blockItem->stmt);
        }
    }
    if (toNew) {
        curTable = curTable->parent;
    }
    curBlockLevel -- ;
}

void Visitor::visitStmt(Stmt *stmt) {
    if (stmt->returnStmt) visitReturnStmt(stmt->returnStmt);
    else if (stmt->ifStmt) visitIfStmt(stmt->ifStmt);
    else if (stmt->outputStmt) visitOutputStmt(stmt->outputStmt);
    else if (stmt->inputStmt) visitInputStmt(stmt->inputStmt);
    else if (stmt->continueStmt) visitContinueStmt(stmt->continueStmt);
    else if (stmt->breakStmt) visitBreakStmt(stmt->breakStmt);
    else if (stmt->forStmt) visitFORStmt(stmt->forStmt);
    else if (stmt->blockStmt) visitBlockStmt(stmt->blockStmt);
    else if (stmt->expStmt) visitExpStmt(stmt->expStmt);
    else if (stmt->assignStmt) visitAssignStmt(stmt->assignStmt);
}

void Visitor::visitReturnStmt(ReturnStmt *returnStmt) {
    if (returnStmt->exp) {
        auto res = visitExp(returnStmt->exp);
        auto middleCode = new MiddleReturn(res);
        curBlock->add(middleCode);
    }
    else {
        auto middleCode = new MiddleReturn();
        curBlock->add(middleCode);
    }
}

void Visitor::visitIfStmt(IfStmt *ifStmt) {
    if (ifStmt->elseStmt) {
        auto trueBlock = new BasicBlock(BasicBlock::BRANCH);
        auto falseBlock = new BasicBlock(BasicBlock::BRANCH);
        auto ifEnd = new Label();
        visitCond(ifStmt->cond, nullptr, falseBlock->getLabel());

        curBlock->add(trueBlock);
        curBlock->add(falseBlock);

        auto tmpBlock = curBlock;
        // ifstmt
        curBlock = trueBlock;
        visitStmt(ifStmt->ifStmt);
        auto jumpToIfEnd = new MiddleJump(MiddleJump::JUMP, ifEnd);
        curBlock->add(jumpToIfEnd);

        // elseStmt
        curBlock = falseBlock;
        visitStmt(ifStmt->elseStmt);

        // 生成ifEnd
        curBlock = tmpBlock;
        curBlock->add(ifEnd);
    }
    else {
        auto trueBlock = new BasicBlock(BasicBlock::BRANCH);
        auto ifEnd = new Label();
        visitCond(ifStmt->cond, nullptr, ifEnd);

        curBlock->add(trueBlock);
        auto tmpBlock = curBlock;
        // ifStmt
        curBlock = trueBlock;
        visitStmt(ifStmt->ifStmt);
        // 生成ifEnd
        curBlock = tmpBlock;
        curBlock->add(ifEnd);
    }
}

void Visitor::visitCond(Cond *cond, Label *trueLabel, Label *falseLabel) {
    visitLOrExp(cond->lOrExp, trueLabel, falseLabel);
}

void Visitor::visitLOrExp(LOrExp *lOrExp, Label *trueLabel, Label *falseLabel) {
    auto orEnd = new Label();
    DEBUG_PRINT("[visitLOrExp] lOrExp has %d lAddExp\n", lOrExp->lAndExps.size());
    for (auto lAndExp : lOrExp->lAndExps) {
        // 只要其中一个lAndExp是真就跳转到trueLabel
        if (trueLabel != nullptr) {
            visitLAndExp(lAndExp, trueLabel, nullptr);
        }
        else {
            visitLAndExp(lAndExp, orEnd, nullptr);
        }
    }
    // 所有条件都检查完了都没有跳转
    if (falseLabel != nullptr) {
        auto jumpToFalse = new MiddleJump(MiddleJump::JUMP, falseLabel);
        curBlock->add(jumpToFalse);
    }
    // 生成结束Label
    if (trueLabel == nullptr) {
        curBlock->add(orEnd);
    }
}

void Visitor::visitLAndExp(LAndExp *lAndExp, Label *trueLabel, Label *falseLabel) {
    auto andEnd = new Label();
    for (auto eqExp : lAndExp->eqExps) {
        // 只要其中一个eqExp为假，就跳转到falseLabel
        auto ret = visitEqExp(eqExp);
        if (falseLabel != nullptr) {
            auto jumpToFalse = new MiddleJump(MiddleJump::JUMP_EQZ, ret, falseLabel);
            curBlock->add(jumpToFalse);
        }
        else {
            auto jumpToAddEnd = new MiddleJump(MiddleJump::JUMP_EQZ, ret, andEnd);
            curBlock->add(jumpToAddEnd);
        }
    }
    if (trueLabel != nullptr) {
        auto jumpToTrue = new MiddleJump(MiddleJump::JUMP, trueLabel);
        curBlock->add(jumpToTrue);
    }
    if (falseLabel == nullptr) {
        curBlock->add(andEnd);
    }
}

Intermediate * Visitor::visitEqExp(EqExp *eqExp) {
    auto res = visitRelExp(eqExp->relExps[0]);

    for (int i = 0; i < eqExp->ops.size(); i ++ ) {
        auto op = eqExp->ops[i];
        auto middleCodeType = op->tokenType==Token::EQL ? MiddleBinaryOp::EQ : MiddleBinaryOp::NE;

        auto src1 = res;
        auto src2 = visitRelExp(eqExp->relExps[i + 1]);

        res = new ValueSymbol(getTempName(), ValueType::TEMP);
        curFunc->tempSymbols.push_back(dynamic_cast<ValueSymbol*>(res));
        auto middleCode = new MiddleBinaryOp(middleCodeType, src1, src2, res);
        curBlock->add(middleCode);
    }
    return res;
}

Intermediate * Visitor::visitRelExp(RelExp *relExp) {
    auto res = visitAddExp(relExp->addExps[0]);

    for (int i = 0; i < relExp->ops.size(); i ++ ) {
        MiddleBinaryOp::Type middleCodeType;
        auto op = relExp->ops[i];
        switch(op->tokenType) {
            case Token::GRE:
                middleCodeType = MiddleBinaryOp::GT;
                break;
            case Token::GEQ:
                middleCodeType = MiddleBinaryOp::GE;
                break;
            case Token::LSS:
                middleCodeType = MiddleBinaryOp::LT;
                break;
            case Token::LEQ:
                middleCodeType = MiddleBinaryOp::LE;
                break;
            default:
                middleCodeType = MiddleBinaryOp::ERROR;
        }

        auto src1 = res;
        auto src2 = visitAddExp(relExp->addExps[i + 1]);

        res = new ValueSymbol(getTempName(), ValueType::TEMP);
        curFunc->tempSymbols.push_back(dynamic_cast<ValueSymbol*>(res));
        auto middleCode = new MiddleBinaryOp(middleCodeType, src1, src2, res);
        curBlock->add(middleCode);
    }
    return res;
}

void Visitor::visitOutputStmt(OutputStmt *outputStmt) {
    std::string str = outputStmt->formatString->content;
    int cnt = 0;
    for (int i = 0; i < str.size(); i ++ ) {
        if (str[i] == '%' && i + 1 < str.size() && str[i + 1] == 'd') {
            cnt ++ ;
        }
    }
    // Error l
    // 输出字符串中的个数不匹配
    if (cnt != outputStmt->exps.size()) {
        auto error = new Error(Error::PRINTF_MISMATCH_NUM, outputStmt->formatString->line);
        ErrorTable::getInstance().addError(error);
    }

    std::vector<Intermediate*> exps;
    for (auto exp : outputStmt->exps) {
        exps.push_back(visitExp(exp));
    }

    std::string constStr;
    int j = 0;
    str = str.substr(1, str.size() - 2);
    for (int i = 0; i < str.size(); i ++ ) {
        if (str[i] == '%' && i + 1 < str.size()&& str[i + 1] == 'd') {
            if (!constStr.empty()) {
                auto name = "str_" + std::to_string(MiddleCode::getInstance().globalStrings.size());
                MiddleCode::getInstance().addGlobalStrings(constStr);
                auto printStr = new MiddleIO(MiddleIO::PRINT_STR, new Immediate(name));
                curBlock->add(printStr);
                constStr.clear();
            }
            auto printInt = new MiddleIO(MiddleIO::PRINT_INT, exps[j ++ ]);
            curBlock->add(printInt);
            i ++ ;
        }
        else {
            constStr.push_back(str[i]);
        }
    }
    if (!constStr.empty()) {
        auto name = "str_" + std::to_string(MiddleCode::getInstance().globalStrings.size());
        MiddleCode::getInstance().addGlobalStrings(constStr);
        auto printStr = new MiddleIO(MiddleIO::PRINT_STR, new Immediate(name));
        curBlock->add(printStr);
        constStr.clear();
    }
}

void Visitor::visitInputStmt(InputStmt *inputStmt) {
    auto ident = inputStmt->lVal->ident;
    std::string name = ident->content;
    // Error h
    // 改变了const
    if (curTable->contain(name, true)) {
        auto valueSymbol = curTable->getSymbol(name, true);
        if (valueSymbol->isConst()) {
            auto error = new Error(Error::CHANGE_CONST, ident->line);
            ErrorTable::getInstance().addError(error);
            return;
        }
    }
    auto tmp = visitLVal(inputStmt->lVal, true);
    auto input = new MiddleIO(MiddleIO::GETINT, dynamic_cast<ValueSymbol*>(tmp));
    // 如果tmp是TEMP的话，只有可能是通过数组形式赋值的
    if (dynamic_cast<ValueSymbol*>(tmp)->valueType == TEMP) {
        dynamic_cast<ValueSymbol*>(tmp)->setArrayElement();
    }
    curBlock->add(input);
}

void Visitor::visitContinueStmt(ContinueStmt *continueStmt) {
    // Error m
    // 如果continue在循环外
    if (inLoop == 0) {
        auto error = new Error(Error::BREAK_CONTINUE_OUT_LOOP, continueStmt->token->line);
        ErrorTable::getInstance().addError(error);
        return;
    }
    // 跳转到loopLabels.size()-2下标的label
    auto label = loopLabels[loopLabels.size() - 2];
    curBlock->add(new MiddleJump(MiddleJump::JUMP, label));
}

void Visitor::visitBreakStmt(BreakStmt *breakStmt) {
    // Error m
    // 如果break在循环外
    if (inLoop == 0) {
        auto error = new Error(Error::BREAK_CONTINUE_OUT_LOOP, breakStmt->token->line);
        ErrorTable::getInstance().addError(error);
        return;
    }
    auto label = loopLabels[loopLabels.size() - 1];
    curBlock->add(new MiddleJump(MiddleJump::JUMP, label));
}

// forStmt1

// cond
// stmt
// forStmt2

void Visitor::visitFORStmt(FORStmt *forStmt) {
    auto loopBlock = new BasicBlock(BasicBlock::LOOP);
    auto beginLoop = loopBlock->label;
    auto endFor = new Label();
    auto forStmt2Label = new Label();
    // 如果forStmt2，让continue可以直接跳转到beginloop
    if (forStmt->forStmt2 == nullptr) {
        forStmt2Label = beginLoop;
    }

    if (forStmt->forStmt1 != nullptr) {
        visitForStmt(forStmt->forStmt1);
    }

    curBlock->add(loopBlock);
    loopLabels.push_back(forStmt2Label);
    loopLabels.push_back(endFor);
    inLoop ++ ;
    auto tmp = curBlock;
    curBlock = loopBlock;

    if (forStmt->cond != nullptr) {
        visitCond(forStmt->cond, nullptr, endFor);
    }
    visitStmt(forStmt->stmt);

    // !!! 这里！，一定要加上一条跳转到stmt2的语句，不然寄存器分配时会出现问题。
    auto jumpToStmt2 = new MiddleJump(MiddleJump::JUMP, forStmt2Label);
    curBlock->add(jumpToStmt2);

    if (forStmt->forStmt2 != nullptr) {
        curBlock->add(forStmt2Label);
        visitForStmt(forStmt->forStmt2);
    }

    auto jumpToBeginLoop = new MiddleJump(MiddleJump::JUMP, beginLoop);
    curBlock->add(jumpToBeginLoop);

    inLoop -- ;
    loopLabels.pop_back();
    loopLabels.pop_back();
    curBlock = tmp;
    // 生成for结束的label
    curBlock->add(endFor);
}

void Visitor::visitForStmt(ForStmt *forStmt) {
    auto ident = forStmt->lVal->ident;
    auto name = ident->content;
    // Error h:
    // 不能改变常量
    if (curTable->contain(name, true)) {
        auto symbol = curTable->getSymbol(name, true);
        if (symbol->isConst()) {
            auto error = new Error(Error::CHANGE_CONST, ident->line);
            ErrorTable::getInstance().addError(error);
        }
    }
    auto target = visitLVal(forStmt->lVal, true);
    auto src = visitExp(forStmt->exp);

    auto middleCode = new MiddleUnaryOp(MiddleUnaryOp::ASSIGN, target, src);
    curBlock->add(middleCode);
}

void Visitor::visitBlockStmt(BlockStmt *blockStmt) {
    visitBlock(blockStmt->block, true);
}

void Visitor::visitExpStmt(ExpStmt *expStmt) {
    if (expStmt->exp != nullptr) {
        visitExp(expStmt->exp);
    }
}

void Visitor::visitAssignStmt(AssignStmt *assignStmt) {
    auto ident = assignStmt->lVal->ident;
    auto name = ident->content;
    // Error h:
    // 不能改变常量
    if (curTable->contain(name, true)) {
        auto symbol = curTable->getSymbol(name, true);
        if (symbol->isConst()) {
            auto error = new Error(Error::CHANGE_CONST, ident->line);
            ErrorTable::getInstance().addError(error);
            return;
        }
    }
    auto src2 = visitExp(assignStmt->exp);
    auto src1 = visitLVal(assignStmt->lVal, true);

    auto symbol = curTable->getSymbol(name, true);
    if (src1 ==nullptr || src2 == nullptr || symbol==nullptr) {
        return;
    }
    // 如果是数组类型，或者是FUNCFPARAM（仅针对参数数组）
    if (dynamic_cast<ValueSymbol*>(symbol)->valueType == ARRAY || dynamic_cast<ValueSymbol*>(symbol)->valueType == FUNCFPARAM) {
        auto middleCode = new MiddleMemoryOp(MiddleMemoryOp::STORE, src2, src1);
        curBlock->add(middleCode);
    }
    else {
        auto middleCode = new MiddleUnaryOp(MiddleUnaryOp::ASSIGN, src1, src2);
        curBlock->add(middleCode);
    }
}

void Visitor::visitMainFuncDef(MainFuncDef *mainFuncDef) {
    visitBlock(mainFuncDef->block, false);
}

void Visitor::visitFuncRParams(FuncRParams *funcRParams, FuncSymbol* funcSymbol, int line, MiddleFuncCall* funcCall) {
    if (funcRParams != nullptr) {
        DEBUG_PRINT_DIRECT("[visitFuncRParams]");
        // Error d
        // 调用参数个数和定义的参数个数不匹配
        if (funcRParams->exps.size() != funcSymbol->num) {
            auto error = new Error(Error::MISMATCH_PARAM_NUM, line);
            ErrorTable::getInstance().addError(error);
            return;
        }

        // Error e
        // 传入参数的维度和定义参数的维度不匹配
        for (int i = 0; i < funcSymbol->num; ++ i) {
            int realDim = funcRParams->exps[i]->getDim();
            int formDim = funcSymbol->funcFParamSymbols[i]->dims.size();
            if (realDim != formDim) {
                auto error = new Error(Error::MISMATCH_PARAM_TYPE, line);
                ErrorTable::getInstance().addError(error);
                return;
            }

            // 如果实参传入的是立即数，则直接传入
            auto expTmp = visitExp(funcRParams->exps[i]);
            auto immediate = dynamic_cast<Immediate*>(expTmp);
            if (immediate != nullptr) {
                funcCall->funcRParams.push_back(immediate);
            }
            // 如果形参的维度是0的话，需要将实参的值拷贝到tmp中
            else if (formDim == 0) {
                funcCall->funcRParams.push_back(expTmp);
            }
            // 如果形参的维度是1或者2，传入指针
            else {
                auto valueSymbol = dynamic_cast<ValueSymbol*>(expTmp);
                funcCall->funcRParams.push_back(valueSymbol);
            }
        }
    }
    else {
        if (funcSymbol->num != 0) {
            auto error = new Error(Error::MISMATCH_PARAM_NUM, line);
            ErrorTable::getInstance().addError(error);
        }
    }
}

std::vector<ConstExp*> Visitor::flattenConstInitVal(ConstInitVal *constInitVal) {
    std::vector<ConstExp*> constExps;
    if (!constInitVal->constInitVals.empty()) {
        for (auto x : constInitVal->constInitVals) {
            if (x->constExp != nullptr) {
                constExps.push_back(x->constExp);
            }
            else {
                auto v2 = flattenConstInitVal(x);
                constExps.insert(constExps.end(), v2.begin(), v2.end());
            }
        }
    }
    return constExps;
}

std::vector<ConstExp*> Visitor::flattenOneConstInitVal(ConstInitVal *constInitVal) {
    // constInitVal -> { constInitVal, constInitVal ... }，其中每个constInitVal都只有一个constExp
    std::vector<ConstExp*> constExps;
    for (auto x : constInitVal->constInitVals) {
        if (x->constExp != nullptr) {
            constExps.push_back(x->constExp);
        }
    }
    return constExps;
}

std::vector<Exp*> Visitor::flattenInitVal(InitVal *initVal) {
    std::vector<Exp*> exps;
    if (!initVal->initVals.empty()) {
        for (auto x : initVal->initVals) {
            if (x->exp != nullptr) {
                exps.push_back(x->exp);
            }
            else {
                auto v2 = flattenInitVal(x);
                exps.insert(exps.end(), v2.begin(), v2.end());
            }
        }
    }
    return exps;
}

std::vector<Exp *> Visitor::flattenOneInitVal(InitVal *initVal) {
    std::vector<Exp*> exps;
    for (auto x : initVal->initVals) {
        if (x->exp != nullptr) {
            exps.push_back(x->exp);
        }
    }
    return exps;
}

int Visitor::tmpVarNumber = 0;
std::string Visitor::getTempName() {
    return "T" + std::to_string(tmpVarNumber ++ );
}

Intermediate * Visitor::getOffsetCount(Intermediate *intermediate) {
    if (dynamic_cast<Immediate*>(intermediate)) {
        return new Immediate(dynamic_cast<Immediate*>(intermediate)->value * sizeof(int));
    }
    else {
        auto offsetCount = new ValueSymbol(getTempName(), ValueType::TEMP);
        curFunc->tempSymbols.push_back(offsetCount);
        auto mul = new MiddleBinaryOp(MiddleBinaryOp::MUL, intermediate, new Immediate(4), offsetCount);
        curBlock->add(mul);
        return offsetCount;
    }
}
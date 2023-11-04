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

SymbolTable* Visitor::curTable;

void Visitor::visit() {
    auto compUnit = ast->root;
    visitCompUnit(compUnit);
}

void Visitor::visitCompUnit(CompUnit *compUnit){
    curTable = new SymbolTable(nullptr);
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
        if (curBlockLevel == 0) {
            MiddleCode::getInstance().addGlobalValues(valueSymbol);
        }
        else {
            // TODO: 处理局部常量 DONE
            // const int a = 10;
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

        if (dims.size() == 1) {
            // 一维数组
            auto constExps = flattenConstInitVal(constDef->constInitVal);
            std::vector<int> initValues;
            initValues.reserve(constExps.size());
            for (auto constExp : constExps) {
                initValues.push_back(Calculate::calcConstExp(constExp));
            }

            if (curBlockLevel == 0) {       // 如果是全局变量的话
                arraySymbol->setInitValues(initValues);
                MiddleCode::getInstance().addGlobalValues(arraySymbol);
            }
            else {      // 如果不是全局变量的话，每个initvalue生成中间代码
                // TODO: 局部数组 DONE
                auto defArray = new MiddleDef(MiddleDef::DEF_ARRAY, arraySymbol);
                curBlock->add(defArray);
                int offsetCount = 0;
                for (auto x : initValues) {
                    auto tmp = new ValueSymbol(getTempName());
                    auto offset = new MiddleOffset(arraySymbol, offsetCount * sizeof(int), tmp);
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
            if (curBlockLevel == 0) {
                // 全局数组
                arraySymbol->setInitValues(initValues);
                MiddleCode::getInstance().addGlobalValues(arraySymbol);
            }
            else {
                // 局部数组，利用flattenValues和assignPlace来确定要赋值的位置
                // TODO: 局部数组 DONE
                auto defArray = new MiddleDef(MiddleDef::DEF_ARRAY, arraySymbol);
                curBlock->add(defArray);
                int offsetCount = 0;
                for (auto x : initValues) {
                    auto tmp = new ValueSymbol(getTempName());
                    auto offset = new MiddleOffset(arraySymbol, offsetCount * sizeof(int), tmp);
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

        res = new ValueSymbol(getTempName());
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

        res = new ValueSymbol(getTempName());
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
        auto res = new ValueSymbol(getTempName());
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
        visitFuncRParams(funcRParams, funcSymbol, ident->line);

        auto middleCode = new Func(Func::CALL, name);
        curBlock->add(middleCode);
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
    return visitAddExp(exp->addExp);
}


Intermediate * Visitor::visitLVal(LVal *lVal) {
    auto ident = lVal->ident;
    std::string name = ident->content;
    DEBUG_PRINT_DIRECT(name);
    // Error c
    if (!curTable->contain(name, true)) {
        auto error = new Error(Error::UNDEFINED_IDENT, ident->line);
        ErrorTable::getInstance().addError(error);
        return nullptr;
    }

    auto symbol = curTable->getSymbol(name, true);

    auto valueSymbol = dynamic_cast<ValueSymbol*>(symbol);
    if (valueSymbol != nullptr) {
        int dim = -1;
        if (valueSymbol != nullptr)
            dim = valueSymbol->getDim();
        if (dim == 0) {
            return valueSymbol;
        }
            // TODO: dim为其他情况，注意是exp，还要考虑如果是函数的调用？
        else if (dim == 1) {
            int offsetCount = Calculate::calcExp(lVal->exps[0]);
            auto tmp = new ValueSymbol(getTempName());
            auto offset = new MiddleOffset(valueSymbol, offsetCount * sizeof(int), tmp);
            auto ret = new ValueSymbol(getTempName());
            auto load = new MiddleMemoryOp(MiddleMemoryOp::LOAD, tmp, ret);
            curBlock->add(offset);
            curBlock->add(load);
            return ret;
        }
        else if (dim == 2) {
            int row = Calculate::calcExp(lVal->exps[0]);
            int col = Calculate::calcExp(lVal->exps[1]);
            int offsetCount = row * valueSymbol->dims[1] + col;

            auto tmp = new ValueSymbol(getTempName());
            auto offset = new MiddleOffset(valueSymbol, offsetCount * sizeof(int), tmp);
            auto ret = new ValueSymbol(getTempName());
            auto load = new MiddleMemoryOp(MiddleMemoryOp::LOAD, tmp, ret);
            curBlock->add(offset);
            curBlock->add(load);
            return ret;
        }
    }
    // 如果是函数的参数，不知道怎么处理比较好，就直接重新处理一编吧
    else {
        DEBUG_PRINT_DIRECT("use fparam");
        auto fParamSymbol = dynamic_cast<FuncFParamSymbol*>(symbol);
        int dim = -1;
        if (fParamSymbol != nullptr)
            dim = fParamSymbol->getDim();
        if (dim == 0) {
            DEBUG_PRINT_DIRECT("111");
            return fParamSymbol;
        }
        else if (dim == 1) {
            int offsetCount = Calculate::calcExp(lVal->exps[0]);
            auto tmp = new ValueSymbol(getTempName());
            auto offset = new MiddleOffset(fParamSymbol, offsetCount * sizeof(int), tmp);
            auto ret = new ValueSymbol(getTempName());
            auto load = new MiddleMemoryOp(MiddleMemoryOp::LOAD, tmp, ret);
            curBlock->add(offset);
            curBlock->add(load);
            return ret;
        }
        else if (dim == 2) {
            int row = Calculate::calcExp(lVal->exps[0]);
            int col = Calculate::calcExp(lVal->exps[1]);
            int offsetCount = row * fParamSymbol->dims[1] + col;

            auto tmp = new ValueSymbol(getTempName());
            auto offset = new MiddleOffset(fParamSymbol, offsetCount * sizeof(int), tmp);
            auto ret = new ValueSymbol(getTempName());
            auto load = new MiddleMemoryOp(MiddleMemoryOp::LOAD, tmp, ret);
            curBlock->add(offset);
            curBlock->add(load);
            return ret;
        }
    }
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

            if (curBlockLevel == 0) {
                // 全局变量
                MiddleCode::getInstance().addGlobalValues(symbol);
            }
            else {
                // TODO:局部变量 DONE
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
                curTable->add(symbol);
                MiddleCode::getInstance().addGlobalValues(symbol);
            }
            else {
                // TODO: 局部变量，运行时计算，即输出中间表达式，在visitExp后输出的可能是值，也可能是valueSymbol, DONE
                auto symbol = new ValueSymbol(name);
                curTable->add(symbol);

                auto sym = visitExp(varDef->initval->exp);
                // 如果visitExp的值是立即数
                if (dynamic_cast<Immediate*>(sym) != nullptr) {
                    DEBUG_PRINT_DIRECT("[visitVarDef] Immediate");
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

        // 一维数组
        if (dims.size() == 1) {
            // 如果是一维数组并且未初始化
            if (!varDef->isInit()) {
                // 是全局变量且未初始化
                if (curBlockLevel == 0) {
                    std::vector<int> initValues(arraySymbol->getFlattenDim(), 0);
                    arraySymbol->setInitValues(initValues);
                    MiddleCode::getInstance().addGlobalValues(arraySymbol);
                }
                else {
                    // TODO: 局部变量且未初始化 DONE
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
                    auto def = new MiddleDef(MiddleDef::DEF_ARRAY, arraySymbol);
                    curBlock->add(def);
                    int offsetCount = 0;
                    for (auto exp : exps) {
                        auto sym = visitExp(exp);
                        DEBUG_PRINT_DIRECT("[visitVarDef] Immediate");
                        auto tmp = new ValueSymbol(getTempName());
                        auto offset = new MiddleOffset(arraySymbol, offsetCount * sizeof(int), tmp);
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
                        auto tmp = new ValueSymbol(getTempName());
                        auto offset = new MiddleOffset(arraySymbol, offsetCount * sizeof(int), tmp);
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
    auto funcBlock = new BasicBlock(BasicBlock::FUNC);
    curBlock = funcBlock;
    // 维护符号表
    curTable = new SymbolTable(curTable);
    auto retType = funcDef->funcType->funcType->tokenType == Token::VOIDTK ? BasicType::VOID : BasicType::INT;
    auto funcSymbol = new FuncSymbol(name, retType, funcDef->getFuncFParamNumber());
    if (funcDef->funcFParams != nullptr) {
        for (auto x : funcDef->funcFParams->funcFParams) {
            visitFuncFParam(x, funcSymbol);
        }
    }
    curTable->parent->add(funcSymbol);

    visitBlock(funcDef->block, false);
    middleFunc->setFuncBlock(funcBlock);
    MiddleCode::getInstance().addFuncs(middleFunc);
    // -------- 后面仅针对错误处理 --------
    BlockItem *lastBlockItem;
    if (!funcDef->block->blockItems.empty()) {
        lastBlockItem = funcDef->block->blockItems.back();
    }
    else {
        lastBlockItem = nullptr;
    }
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

void Visitor::visitFuncFParam(FuncFParam *funcFParam, FuncSymbol *funcSymbol) {
    auto ident = funcFParam->ident;
    std::string name = ident->content;
    // Error b
    if (curTable->contain(name, false)) {
        auto error = new Error(Error::DUPLICATE_IDENT, ident->line);
        ErrorTable::getInstance().addError(error);
        return;
    }

    // TODO: 维度的具体值的计算
    std::vector<int> dims;
    FuncFParamSymbol * funcFParamSymbol = nullptr;
    if (funcFParam->isArray) {
        dims.push_back(1);
        for (auto x : funcFParam->constExps) {
            dims.push_back(1);
            visitConstExp(x);
        }
        funcFParamSymbol = new FuncFParamSymbol(name, dims);
        curTable->add(funcFParamSymbol);
        funcSymbol->addFuncFParamSymbol(funcFParamSymbol);
    }
    else {
        funcFParamSymbol = new FuncFParamSymbol(name, dims);
        curTable->add(funcFParamSymbol);
        funcSymbol->addFuncFParamSymbol(funcFParamSymbol);
    }
    auto middleCode = new MiddleFuncCall(MiddleFuncCall::PARAM, funcFParamSymbol);
    auto label = new Label();
    curBlock->add(middleCode);
    curBlock->add(label);
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
        auto middleCode = new MiddleFuncCall(MiddleFuncCall::RETURN, res);
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

        res = new ValueSymbol(getTempName());
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

        res = new ValueSymbol(getTempName());
        auto middleCode = new MiddleBinaryOp(middleCodeType, src1, src2, res);
        curBlock->add(middleCode);
    }
    return res;
}

void Visitor::visitOutputStmt(OutputStmt *outputStmt) {
    for (auto exp : outputStmt->exps) {
        visitExp(exp);
    }
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
    visitLVal(inputStmt->lVal);
}

void Visitor::visitContinueStmt(ContinueStmt *continueStmt) {
    // Error m
    // 如果continue在循环外
    if (inLoop == 0) {
        auto error = new Error(Error::BREAK_CONTINUE_OUT_LOOP, continueStmt->token->line);
        ErrorTable::getInstance().addError(error);
    }
}

void Visitor::visitBreakStmt(BreakStmt *breakStmt) {
    // Error m
    // 如果break在循环外
    if (inLoop == 0) {
        auto error = new Error(Error::BREAK_CONTINUE_OUT_LOOP, breakStmt->token->line);
        ErrorTable::getInstance().addError(error);
    }
}

void Visitor::visitFORStmt(FORStmt *forStmt) {
    auto loopBlock = new BasicBlock(BasicBlock::LOOP);
    auto beginLoop = loopBlock->label;
    auto endFor = new Label();

    if (forStmt->forStmt1 != nullptr) {
        visitForStmt(forStmt->forStmt1);
    }

    curBlock->add(loopBlock);
    inLoop ++ ;
    auto tmp = curBlock;
    curBlock = loopBlock;

    if (forStmt->cond != nullptr) {
        visitCond(forStmt->cond, nullptr, endFor);
    }
    visitStmt(forStmt->stmt);
    if (forStmt->forStmt2 != nullptr) {
        visitForStmt(forStmt->forStmt2);
    }

    auto jumpToBeginLoop = new MiddleJump(MiddleJump::JUMP, beginLoop);
    curBlock->add(jumpToBeginLoop);

    inLoop -- ;
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
    auto target = visitLVal(forStmt->lVal);
    auto src = visitExp(forStmt->exp);
    auto middleCode = new MiddleUnaryOp(MiddleUnaryOp::ASSIGN, src, target);
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
    auto src1 = visitLVal(assignStmt->lVal);
    auto src2 = visitExp(assignStmt->exp);
    auto middleCode = new MiddleUnaryOp(MiddleUnaryOp::ASSIGN, src2, src1);
    curBlock->add(middleCode);
}

void Visitor::visitMainFuncDef(MainFuncDef *mainFuncDef) {
    visitBlock(mainFuncDef->block, false);
}

void Visitor::visitFuncRParams(FuncRParams *funcRParams, FuncSymbol* funcSymbol, int line) {
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
            visitExp(funcRParams->exps[i]);
            int realDim = funcRParams->exps[i]->getDim();
            int formDim = funcSymbol->funcFParamSymbols[i]->dims.size();
            if (realDim != formDim) {
                auto error = new Error(Error::MISMATCH_PARAM_TYPE, line);
                ErrorTable::getInstance().addError(error);
                return;
            }

            // 如果实参传入的是立即数，则直接传入
            // TODO: 验证是否会经过这个if
            auto immediate = dynamic_cast<Immediate*>(visitExp(funcRParams->exps[i]));
            if (immediate != nullptr) {
                auto pushValue = new MiddleFuncCall(MiddleFuncCall::PUSH_PARAM, immediate);
                curBlock->add(pushValue);
            }
            // 如果形参的维度是0的话，需要将实参的值拷贝到tmp中
            else if (formDim == 0) {
                auto tmp = new ValueSymbol(getTempName());
                auto rParam = visitExp(funcRParams->exps[i]);
                auto defTmp = new MiddleDef(MiddleDef::DEF_VAR, tmp, rParam);
                curBlock->add(defTmp);
                auto pushTmp = new MiddleFuncCall(MiddleFuncCall::PUSH_PARAM, tmp);
                curBlock->add(pushTmp);
            }
            // 如果形参的维度是1或者2，直接传入值
            // TODO:这里需要算出地址？
            else {
                auto valueSymbol = dynamic_cast<ValueSymbol*>(visitExp(funcRParams->exps[i]));
                if (valueSymbol != nullptr) {
                    auto pushValue = new MiddleFuncCall(MiddleFuncCall::PUSH_PARAM, valueSymbol);
                    curBlock->add(pushValue);
                }
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

int Visitor::alloc(int mem) {
    stackSize += mem;
    return stackSize;
}

std::string Visitor::getTempName() {
    return "T" + std::to_string(tmpVarNumber ++ );
}
//
// Created by 安达楷 on 2023/10/4.
//

#define DEBUG

#include "Visitor.h"
#include "../Lexer/Lexer.h"
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
        visitConstInitVal(constDef->constInitVal);
        // TODO:添加值
        curTable->add(new ValueSymbol(name, 0, true));
    }
    else {
        // 数组且初始化，形如const int a[10] = {...};
        // 初始值
        visitConstInitVal(constDef->constInitVal);
        // TODO:具体维度，初始化的值
        std::vector<int> dims;  //之后通过constInitVal得到
        curTable->add(new ValueSymbol(name, dims, true));
    }
}

void Visitor::visitConstInitVal(ConstInitVal *constInitVal) {
    if (constInitVal->isArray()) {
        for (auto x : constInitVal->constInitVals) {
            visitConstInitVal(x);
        }
    }
    else {
        visitConstExp(constInitVal->constExp);
    }
}

void Visitor::visitConstExp(ConstExp *constExp) {
    visitAddExp(constExp->addExp);
}

void Visitor::visitAddExp(AddExp *addExp) {
    for (auto mulExp : addExp->mulExps) {
        visitMulExp(mulExp);
    }
}

void Visitor::visitMulExp(MulExp *mulExp) {
    for (auto x : mulExp->unaryExps) {
        visitUnaryExp(x);
    }
}

void Visitor::visitUnaryExp(UnaryExp *unaryExp) {
    auto primaryExp = unaryExp->primaryExp;
    auto ident = unaryExp->ident;
    auto funcRParams = unaryExp->funcRParams;
    auto inUnaryExp = unaryExp->unaryExp;
    if (primaryExp != nullptr) {
        visitPrimaryExp(primaryExp);
    }
    if (inUnaryExp != nullptr) {
        visitUnaryExp(inUnaryExp);
    }
    //函数调用：在所属符号表中查找
    else if (ident != nullptr) {
        std::string name = ident->content;
        // Error c
        // 未定义
        if (!curTable->contain(name, true)) {
            auto error = new Error(Error::UNDEFINED_IDENT, ident->line);
            ErrorTable::getInstance().addError(error);
            return;
        }
        auto funcSymbol = dynamic_cast<FuncSymbol*>(curTable->getSymbol(name, true));
        visitFuncRParams(funcRParams, funcSymbol, ident->line);
    }
}

void Visitor::visitPrimaryExp(PrimaryExp *primaryExp) {
    auto exp = primaryExp->exp;
    auto lVal = primaryExp->lVal;
    auto number = primaryExp->number;
    if (exp != nullptr) {
        visitExp(exp);
    }
    else if (lVal != nullptr) {
        visitLVal(lVal);
    }
    else if (number != nullptr) {
        visitNumber(number);
    }
}

void Visitor::visitExp(Exp *exp) {
    visitAddExp(exp->addExp);
}


void Visitor::visitLVal(LVal *lVal) {
    auto ident = lVal->ident;
    std::string name = ident->content;
    // Error c
    if (!curTable->contain(name, true)) {
        auto error = new Error(Error::UNDEFINED_IDENT, ident->line);
        ErrorTable::getInstance().addError(error);
    }
    for (auto x : lVal->exps) {
        visitExp(x);
    }
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
        if (!varDef->isInit()) {
            //如果不是数组并且没有初始化, int a;
            curTable->add(new ValueSymbol(name));
        }
        else {
            //如果不是数组但是初始化了，int a = 10;
            visitInitVal(varDef->initval);
            //TODO:得到初始值
            int x = 0;
            curTable->add(new ValueSymbol(name, x, false));
        }
    }
    else {
        //TODO:具体值
        std::vector<int> dims;
        for (auto x : varDef->constExps) {
            visitConstExp(x);
            dims.push_back(0);
        }
        if (!varDef->isInit()) {
            //如果是数组并且没有初始化，int a[10];
            curTable->add(new ValueSymbol(name, dims, false));
        }
        else {
            //是数组且初始化，int a[10] = {...};
            visitInitVal(varDef->initval);
            std::vector<int> initVals;
            curTable->add(new ValueSymbol(name, dims, initVals, false));
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

void Visitor::visitNumber(Number *number) {
    auto intConst = number->intConst;
    int x = intConst->val;
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
#ifdef DEBUG
    std::cout << "Start FuncDef: " << name << std::endl;
#endif
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

#ifdef DEBUG
    std::cout << name << " FuncFParam ready, " << "have " << funcSymbol->funcFParamSymbols.size() << std::endl;
#endif
    visitBlock(funcDef->block, false);

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
    if (funcFParam->isArray) {
        dims.push_back(1);
        for (auto x : funcFParam->constExps) {
            dims.push_back(1);
            visitConstExp(x);
        }
        auto funcFParamSymbol = new FuncFParamSymbol(name, dims);
        curTable->add(funcFParamSymbol);
        funcSymbol->addFuncFParamSymbol(funcFParamSymbol);
    }
    else {
        auto funcFParamSymbol = new FuncFParamSymbol(name, dims);
        curTable->add(funcFParamSymbol);
        funcSymbol->addFuncFParamSymbol(funcFParamSymbol);
    }
}

// 如果是函数的{}，那由于需要考虑形参的影响，就重新建符号表了
void Visitor::visitBlock(Block *block, bool toNew) {
    if (toNew) {
        curTable = new SymbolTable(curTable);
    }
#ifdef DEBUG
    std::cout << "in visitBlock():" << std::endl;
#endif
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
}

void Visitor::visitStmt(Stmt *stmt) {
#ifdef DEBUG
    std::cout << "visit Stmt" << std::endl;
#endif
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
#ifdef DEBUG
    std::cout << "in returnStmt" << std::endl;
#endif
    if (returnStmt->exp) {
        visitExp(returnStmt->exp);
    }
}

void Visitor::visitIfStmt(IfStmt *ifStmt) {
    visitCond(ifStmt->cond);
    visitStmt(ifStmt->ifStmt);
    if (ifStmt->elseStmt) {
        visitStmt(ifStmt->elseStmt);
    }
}

void Visitor::visitCond(Cond *cond) {
    visitLOrExp(cond->lOrExp);
}

void Visitor::visitLOrExp(LOrExp *lOrExp) {
    for (auto lAndExp : lOrExp->lAndExps) {
        visitLAndExp(lAndExp);
    }
}

void Visitor::visitLAndExp(LAndExp *lAndExp) {
    for (auto eqExp : lAndExp->eqExps) {
        visitEqExp(eqExp);
    }
}

void Visitor::visitEqExp(EqExp *eqExp) {
    for (auto relExp : eqExp->relExps) {
        visitRelExp(relExp);
    }
}

void Visitor::visitRelExp(RelExp *relExp) {
    for (auto addExp : relExp->addExps) {
        visitAddExp(addExp);
    }
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
#ifdef DEBUG
    std::cout << "in visitFORStmt" << std::endl;
#endif
    if (forStmt->forStmt1 != nullptr) {
        visitForStmt(forStmt->forStmt1);
    }
    if (forStmt->cond != nullptr) {
        visitCond(forStmt->cond);
    }
    if (forStmt->forStmt2 != nullptr) {
        visitForStmt(forStmt->forStmt2);
    }
    inLoop ++ ;
    visitStmt(forStmt->stmt);
    inLoop -- ;
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
    visitLVal(forStmt->lVal);
    visitExp(forStmt->exp);
}

void Visitor::visitBlockStmt(BlockStmt *blockStmt) {
#ifdef DEBUG
    std::cout << "visit blockStmt" << std::endl;
#endif
    visitBlock(blockStmt->block, true);
}

void Visitor::visitExpStmt(ExpStmt *expStmt) {
#ifdef DEBUG
    std::cout << "visit ExpStmt" << std::endl;
#endif
    if (expStmt->exp != nullptr) {
        visitExp(expStmt->exp);
    }
}

void Visitor::visitAssignStmt(AssignStmt *assignStmt) {
#ifdef DEBUG
    std::cout << "in visitAssignStmt" << std::endl;
#endif
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
    visitLVal(assignStmt->lVal);
    visitExp(assignStmt->exp);
}

void Visitor::visitMainFuncDef(MainFuncDef *mainFuncDef) {
    visitBlock(mainFuncDef->block, false);
}

void Visitor::visitFuncRParams(FuncRParams *funcRParams, FuncSymbol* funcSymbol, int line) {
    if (funcRParams != nullptr) {
        // Error d
        // 调用参数个数和定义的参数个数不匹配
        if (funcRParams->exps.size() != funcSymbol->num) {
#ifdef DEBUG
            std::cout << "realDim: " << funcRParams->exps.size() << std::endl;
            std::cout << "formDim: " << funcSymbol->num << std::endl;
#endif
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
#ifdef DEBUG
            std::cout << funcSymbol->name << " has FuncFParam num: " << funcSymbol->funcFParamSymbols.size() << std::endl;
            std::cout << "realDim: " << realDim << std::endl;
            std::cout << "formDim: " << formDim << std::endl;
#endif
            if (realDim != formDim) {
                auto error = new Error(Error::MISMATCH_PARAM_TYPE, line);
                ErrorTable::getInstance().addError(error);
                return;
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
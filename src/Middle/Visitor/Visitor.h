//
// Created by 安达楷 on 2023/10/4.
//

#ifndef BUAA_COMPILER_VISITOR_H
#define BUAA_COMPILER_VISITOR_H

#include "../../Fronted/Error/ErrorTable.h"
#include "../../Fronted/Parser/AST.h"
#include "../Symbol/SymbolTable.h"
#include "../MiddleCodeItem/Middle.h"
#include "../MiddleCodeItem/Func.h"

class Visitor {
private:
    int inLoop = 0;
public:
    ErrorTable * errorTable;
    AST * ast;
    static SymbolTable * curTable;
    int unique = 0;
    int curBlockLevel = 0;
    BasicBlock *curBlock{};
    int curStackSize = 0;
    int curCodeIndex = 0;
    static int tmpVarNumber;
    static SymbolTable * curFuncSymbolTable;
    Func *curFunc;
    std::vector<Func*> funcs;   // 在NoChangeValue中使用，遍历所有的中间代码

    void optimizeMiddle();

    void updateCurStackSize(ValueSymbol* valueSymbol);
    std::vector<BasicBlock*> loopBlocks;    // 确定continue /break跳转位置

    static std::string getTempName();

    Intermediate * getOffsetCount(Intermediate * intermediate);

    Visitor(AST* ast_) : ast(ast_) {}

    void visit();

    void visitCompUnit(CompUnit *compUnit);

    void visitDecl(Decl *decl);

    void visitConstDecl(ConstDecl *constDecl);

    void visitConstDef(ConstDef *constDef);

    void visitConstExp(ConstExp *constExp);

    Intermediate * visitAddExp(AddExp *addExp);

    void visitConstInitVal(ConstInitVal *constInitVal);

    Intermediate * visitMulExp(MulExp *mulExp);

    Intermediate * visitUnaryExp(UnaryExp *unaryExp);

    Intermediate * visitPrimaryExp(PrimaryExp *primaryExp);

    Intermediate * visitExp(Exp *exp);

    Intermediate * visitLVal(LVal *lVal, bool isLeft=false);

    void visitVarDecl(VarDecl *varDecl);

    void visitVarDef(VarDef *varDef);

    void visitInitVal(InitVal *initVal);

    Intermediate * visitNumber(Number *number);

    void visitFuncDef(FuncDef *funcDef);

    void visitFuncFParam(FuncFParam *funcFParam, FuncSymbol *funcSymbol);

    BasicBlock* visitBlock(Block *block, bool toNew, std::string &name);

    void visitStmt(Stmt *stmt);

    void visitReturnStmt(ReturnStmt *returnStmt);

    void visitIfStmt(IfStmt *ifStmt);

    void visitCond(Cond *cond, BasicBlock *trueLabel, BasicBlock *falseLabel);

    void visitLOrExp(LOrExp *lOrExp, BasicBlock *trueLabel, BasicBlock *falseLabel);

    void visitLAndExp(LAndExp *lAndExp, BasicBlock *trueLabel, BasicBlock *falseLabel);

    Intermediate * visitEqExp(EqExp *eqExp);

    Intermediate * visitRelExp(RelExp *relExp);

    void visitOutputStmt(OutputStmt *outputStmt);

    void visitInputStmt(InputStmt *inputStmt);

    void visitContinueStmt(ContinueStmt *continueStmt);

    void visitBreakStmt(BreakStmt *breakStmt);

    void visitFORStmt(FORStmt *forStmt);

    void visitForStmt(ForStmt *forStmt);

    void visitBlockStmt(BlockStmt *blockStmt);

    void visitExpStmt(ExpStmt *expStmt);

    void visitAssignStmt(AssignStmt *assignStmt);

    void visitMainFuncDef(MainFuncDef *mainFuncDef);

    void visitFuncRParams(FuncRParams *funcRParams, FuncSymbol* funcSymbol, int line, MiddleFuncCall* funcCall);

    std::vector<ConstExp*> flattenConstInitVal(ConstInitVal *constInitVal);

    std::vector<ConstExp*> flattenOneConstInitVal(ConstInitVal *constInitVal);

    std::vector<Exp*> flattenInitVal(InitVal *initVal);

    std::vector<Exp*> flattenOneInitVal(InitVal *InitVal);
};
#endif //BUAA_COMPILER_VISITOR_H

//
// Created by 安达楷 on 2023/10/4.
//

#ifndef BUAA_COMPILER_VISITOR_H
#define BUAA_COMPILER_VISITOR_H

#include "../Error/ErrorTable.h"
#include "../Parser/AST.h"
#include "../Symbol/SymbolTable.h"
class Visitor {
private:
    int inLoop = 0;
public:
    ErrorTable * errorTable;
    AST * ast;
    static SymbolTable * curTable;
    int unique = 0;

    Visitor(AST* ast_) : ast(ast_) {}

    void visit();

    void visitCompUnit(CompUnit *compUnit);

    void visitDecl(Decl *decl);

    void visitConstDecl(ConstDecl *constDecl);

    void visitConstDef(ConstDef *constDef);

    void visitConstExp(ConstExp *constExp);

    void visitAddExp(AddExp *addExp);

    void visitConstInitVal(ConstInitVal *constInitVal);

    void visitMulExp(MulExp *mulExp);

    void visitUnaryExp(UnaryExp *unaryExp);

    void visitPrimaryExp(PrimaryExp *primaryExp);

    void visitExp(Exp *exp);

    void visitLVal(LVal *lVal);

    void visitVarDecl(VarDecl *varDecl);

    void visitVarDef(VarDef *varDef);

    void visitInitVal(InitVal *initVal);

    void visitNumber(Number *number);

    void visitFuncDef(FuncDef *funcDef);

    void visitFuncFParam(FuncFParam *funcFParam, FuncSymbol *funcSymbol);

    void visitBlock(Block *block, bool toNew);

    void visitStmt(Stmt *stmt);

    void visitReturnStmt(ReturnStmt *returnStmt);

    void visitIfStmt(IfStmt *ifStmt);

    void visitCond(Cond *cond);

    void visitLOrExp(LOrExp *lOrExp);

    void visitLAndExp(LAndExp *lAndExp);

    void visitEqExp(EqExp *eqExp);

    void visitRelExp(RelExp *relExp);

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

    void visitFuncRParams(FuncRParams *funcRParams, FuncSymbol* funcSymbol, int line);
};
#endif //BUAA_COMPILER_VISITOR_H

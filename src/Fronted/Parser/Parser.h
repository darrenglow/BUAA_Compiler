//
// Created by 安达楷 on 2023/9/19.
//

#ifndef BUAA_COMPILER_PARSER_H
#define BUAA_COMPILER_PARSER_H

#include <vector>
#include "../Lexer/Token.h"
#include "AST.h"

class Parser{
private:
    TokenList &tokenList;
public:
    explicit Parser(TokenList& tokenList_) : tokenList(tokenList_) {}

    AST* ast;

    AST* getAST();

    void printAST();

    void parse();

    CompUnit* parseCompUnit();

    Decl* parseDecl();

    ConstDecl* parseConstDecl();

    ConstDef* parseConstDef();

    VarDecl* parseVarDecl();

    ConstExp *parseConstExp();

    AddExp *parseAddExp();

    MulExp *parseMulExp();

    UnaryOp *parseUnaryOp();

    UnaryExp *parseUnaryExp();

    PrimaryExp *parsePrimaryExp();

    FuncRParams *parseFuncRParams();

    Exp *parseExp();

    LVal *parseLVal();

    Number *parseNumber();

    VarDef *parseVarDef();

    ConstInitVal *parseConstInitVal();

    FuncDef *parseFuncDef();

    FuncType *parseFuncType();

    FuncFParams *parseFuncFParams();

    FuncFParam *parseFuncFParam();

    BType *parseBType();

    Block *parseBlock();

    BlockItem *parseBlockItem();

    Stmt *parseStmt();

    IfStmt *parseIfStmt();

    Cond *parseCond();

    LOrExp *parseLOrExp();

    LAndExp *parseLAndExp();

    EqExp *parseEqExp();

    RelExp *parseRelExp();

    FORStmt *parseFORStmt();

    ForStmt *parseForStmt();

    BreakStmt *parseBreakStmt();

    ContinueStmt *parseContinueStmt();

    ReturnStmt *parseReturnStmt();

    OutputStmt *parseOutputStmt();

    BlockStmt *parseBlockStmt();

    ExpStmt *parseExpStmt();

    AssignStmt *parseAssignStmt();

    InputStmt *parseInputStmt();

    MainFuncDef *parseMainFuncDef();

    InitVal *parseInitVal();
};

#endif //BUAA_COMPILER_PARSER_H

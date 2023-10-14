//
// Created by 安达楷 on 2023/9/21.
//

#ifndef BUAA_COMPILER_AST_H
#define BUAA_COMPILER_AST_H

#define DECLARE_OSTREAM(cls) \
    friend std::ostream& operator<<(std::ostream& ostream, const cls& obj);

#include <vector>
#include <iostream>
#include "../Lexer/Token.h"

class CompUnit;
class AST{
public:
    CompUnit* root;
};

class Decl;
class FuncDef;
class MainFuncDef;
class CompUnit{
public:
    std::vector<Decl*> decls;
    std::vector<FuncDef*> funcDefs;
    MainFuncDef* mainFuncDef;
    DECLARE_OSTREAM(CompUnit);
};

class FuncFParams;
class Block;
class FuncType;
class FuncDef{
public:
    FuncType* funcType;
    Token* ident;
    FuncFParams* funcFParams;
    Block* block;
    DECLARE_OSTREAM(FuncDef);
    int getFuncFParamNumber();
};

class FuncType{
public:
    Token* funcType;
    DECLARE_OSTREAM(FuncType);
};

class FuncFParam;
class FuncFParams{
public:
    std::vector<FuncFParam*> funcFParams;
    DECLARE_OSTREAM(FuncFParams);
};

class ConstExp;
class BType;
class FuncFParam{
public:
    BType* bType;
    Token* ident;
    bool isArray;
    std::vector<ConstExp*> constExps;
    DECLARE_OSTREAM(FuncFParam);
};

class BType{
public:
    Token* bType;
    DECLARE_OSTREAM(BType);
};

class AddExp;
class ConstExp{
public:
    AddExp* addExp;
    DECLARE_OSTREAM(ConstExp);
};

class MulExp;
class UnaryOp;
class AddExp{
public:
    std::vector<MulExp*> mulExps;
    std::vector<Token*> ops;
    DECLARE_OSTREAM(AddExp);
};

class UnaryExp;
class MulExp{
public:
    std::vector<UnaryExp*> unaryExps;
    std::vector<Token*> ops;
    DECLARE_OSTREAM(MulExp);
};

class UnaryOp{
public:
    Token* unaryOp;
    DECLARE_OSTREAM(UnaryOp);
};

class PrimaryExp;
class FuncRParams;
class UnaryExp{
public:
    PrimaryExp* primaryExp;
    Token* ident;
    FuncRParams* funcRParams;
    UnaryOp* unaryOp;
    UnaryExp* unaryExp;
    DECLARE_OSTREAM(UnaryExp);
    int getDim();
};

class Exp;
class LVal;
class Number;
class PrimaryExp{
public:
    Exp* exp;
    LVal* lVal;
    Number* number;
    DECLARE_OSTREAM(PrimaryExp);
    int getDim();
};

class LVal{
public:
    Token* ident;
    std::vector<Exp*> exps;
    DECLARE_OSTREAM(LVal);
};

class Number{
public:
    Token* intConst;
    DECLARE_OSTREAM(Number);
};

class FuncRParams{
public:
    std::vector<Exp*> exps;
    DECLARE_OSTREAM(FuncRParams);
};

class Exp{
public:
    AddExp* addExp;
    DECLARE_OSTREAM(Exp);
    int getDim();
};

class LAndExp;
class LOrExp{
public:
    std::vector<LAndExp*> lAndExps;
    std::vector<Token*> ops;
    DECLARE_OSTREAM(LOrExp);
};

class EqExp;
class LAndExp{
public:
    std::vector<EqExp*> eqExps;
    std::vector<Token*> ops;
    DECLARE_OSTREAM(LAndExp);
};

class RelExp;
class EqExp{
public:
    std::vector<RelExp*> relExps;
    std::vector<Token*> ops;
    DECLARE_OSTREAM(EqExp);
};

class RelExp{
public:
    std::vector<AddExp*> addExps;
    std::vector<Token*> ops;
    DECLARE_OSTREAM(RelExp);
};

class BlockItem;
class Block{
public:
    std::vector<BlockItem*> blockItems;
    Token *Rbrace;
    DECLARE_OSTREAM(Block);
};

class Stmt;
class BlockItem{
public:
    Decl* decl;
    Stmt* stmt;
    DECLARE_OSTREAM(BlockItem);
};

class AssignStmt{
public:
    LVal* lVal;
    Exp* exp;
    DECLARE_OSTREAM(AssignStmt);
};

class ExpStmt{
public:
    Exp* exp;
    DECLARE_OSTREAM(ExpStmt);
};

class BlockStmt{
public:
    Block* block;
    DECLARE_OSTREAM(BlockStmt);
};

class Cond;
class IfStmt{
public:
    Cond* cond;
    Stmt* ifStmt;
    Stmt* elseStmt;
    DECLARE_OSTREAM(IfStmt);
};

class ForStmt;
class FORStmt{
public:
    ForStmt* forStmt1;
    Cond* cond;
    ForStmt* forStmt2;
    Stmt* stmt;
    DECLARE_OSTREAM(FORStmt);
};

class ForStmt{
public:
    LVal* lVal;
    Exp* exp;
    DECLARE_OSTREAM(ForStmt);
};

class BreakStmt{
public:
    Token* token;
    DECLARE_OSTREAM(BreakStmt);
};

class ContinueStmt{
public:
    Token* token;
    DECLARE_OSTREAM(ContinueStmt);
};

class ReturnStmt{
public:
    Token *returnTK;
    Exp* exp;
    DECLARE_OSTREAM(ReturnStmt);
};

class InputStmt{
public:
    LVal* lVal;
    DECLARE_OSTREAM(InputStmt);
};

class OutputStmt{
public:
    Token* formatString;
    std::vector<Exp*> exps;
    DECLARE_OSTREAM(OutputStmt);
};

class Stmt{
public:
    AssignStmt* assignStmt;
    ExpStmt* expStmt;
    BlockStmt* blockStmt;
    IfStmt* ifStmt;
    FORStmt* forStmt;
    BreakStmt* breakStmt;
    ContinueStmt* continueStmt;
    ReturnStmt* returnStmt;
    InputStmt* inputStmt;
    OutputStmt* outputStmt;
    DECLARE_OSTREAM(Stmt);
};

class Cond{
public:
    LOrExp* lOrExp;
    DECLARE_OSTREAM(Cond);
};

class Def;
class ConstDecl;
class VarDecl;
class Decl{
public:
    ConstDecl* constDecl;
    VarDecl* varDecl;
    DECLARE_OSTREAM(Decl);
};

class ConstDef;
class ConstDecl{
public:
    BType* bType;
    std::vector<ConstDef*> constDefs;
    DECLARE_OSTREAM(ConstDecl);
};

class VarDef;
class VarDecl{
public:
    BType* bType;
    std::vector<VarDef*> varDefs;
    DECLARE_OSTREAM(VarDecl);
};

class Def{
public:
    Def()=default;
    ~Def()=default;
    DECLARE_OSTREAM(Def);
};

class ConstInitVal;
class ConstDef : public Def{
public:
    Token* ident;
    std::vector<ConstExp*> constExps;
    ConstInitVal* constInitVal;
    DECLARE_OSTREAM(ConstDef);
    bool isArray();
    bool isInit();
};

class ConstInitVal{
public:
    ConstExp* constExp;
    std::vector<ConstInitVal*> constInitVals;
    DECLARE_OSTREAM(ConstInitVal);
    bool isArray();
};

class InitVal;
class VarDef : public Def{
public:
    Token* ident;
    std::vector<ConstExp*> constExps;
    InitVal* initval;
    DECLARE_OSTREAM(VarDef);
    bool isArray();
    bool isInit();
};

class InitVal{
public:
    Exp* exp;
    std::vector<InitVal*> initVals;
    DECLARE_OSTREAM(InitVal);
    bool isArray();
};

class MainFuncDef : public FuncDef{
public:
    DECLARE_OSTREAM(MainFuncDef);
};
#endif //BUAA_COMPILER_AST_H

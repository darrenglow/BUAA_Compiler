//
// Created by 安达楷 on 2023/9/19.
//
#include <iostream>
#include <fstream>
#include "Parser.h"
#include "AST.h"

//#define DEBUG

extern std::ofstream output;
void Parser::printAST(){
    output << *ast->root;
}


AST * Parser::getAST() {
    return ast;
}

void Parser::parse() {
    ast = new AST();
    ast->root = parseCompUnit();
#ifdef DEBUG
    std::cout << "parse over" << std::endl;
#endif
}

//CompUnit → {Decl} {FuncDef} MainFuncDef
CompUnit * Parser::parseCompUnit() {
    auto compUnit = new CompUnit();

    while (tokenList.getNext(2)->tokenType != Token::LPARENT){
        compUnit->decls.push_back(parseDecl());
    }
    while (tokenList.getNext(1)->tokenType != Token::MAINTK){
        compUnit->funcDefs.push_back(parseFuncDef());
    }
    compUnit->mainFuncDef = parseMainFuncDef();
    return compUnit;
}

//Decl → ConstDecl | VarDecl
Decl * Parser::parseDecl() {
    Decl* decl = new Decl();
    if (tokenList.get()->tokenType == Token::CONSTTK){
        decl->constDecl = parseConstDecl();
    }
    else {
        decl->varDecl = parseVarDecl();
    }
    return decl;
}

//ConstDecl → 'const' BType ConstDef { ',' ConstDef } ';'
ConstDecl * Parser::parseConstDecl() {
    auto constDecl = new ConstDecl();
    tokenList.popExpect(Token::CONSTTK);
    constDecl->bType = parseBType();
    constDecl->constDefs.push_back(parseConstDef());
    while (tokenList.get()->tokenType == Token::COMMA){
        tokenList.popExpect(Token::COMMA);
        constDecl->constDefs.push_back(parseConstDef());
    }
    tokenList.popExpect(Token::SEMICN);
    return constDecl;
}

//ConstDef → Ident { '[' ConstExp ']' } '=' ConstInitVal
ConstDef * Parser::parseConstDef() {
    auto constDef = new ConstDef();
    constDef->ident = tokenList.pop();
    while (tokenList.get()->tokenType == Token::LBRACK){
        tokenList.popExpect(Token::LBRACK);
        constDef->constExps.push_back(parseConstExp());
        tokenList.popExpect(Token::RBRACK);
    }
    tokenList.popExpect(Token::ASSIGN);
    constDef->constInitVal = parseConstInitVal();
    return constDef;
}

//VarDecl → BType VarDef { ',' VarDef } ';'
VarDecl * Parser::parseVarDecl() {
    auto varDecl = new VarDecl();
    varDecl->bType = parseBType();
    varDecl->varDefs.push_back(parseVarDef());
    while (tokenList.get()->tokenType == Token::COMMA){
        tokenList.popExpect(Token::COMMA);
        varDecl->varDefs.push_back(parseVarDef());
    }
    tokenList.popExpect(Token::SEMICN);
    return varDecl;
}

//ConstExp → AddExp
ConstExp *Parser::parseConstExp() {
    auto constExp = new ConstExp();
    constExp->addExp = parseAddExp();
    return constExp;
}

// AddExp -> MulExp { ('+' | '−') MulExp }
AddExp *Parser::parseAddExp() {
    auto addExp = new AddExp();
    addExp->mulExps.push_back(parseMulExp());
    while (tokenList.get()->tokenType == Token::PLUS ||
    tokenList.get()->tokenType == Token::MINU){
        addExp->ops.push_back(tokenList.pop());
        addExp->mulExps.push_back(parseMulExp());
    }
    return addExp;
}

//MulExp -> UnaryExp { ('*' | '/' | '%') UnaryExp }
MulExp *Parser::parseMulExp() {
    auto mulExp = new MulExp();
    mulExp->unaryExps.push_back(parseUnaryExp());
    while (tokenList.get()->tokenType == Token::MULT ||
    tokenList.get()->tokenType == Token::DIV ||
    tokenList.get()->tokenType == Token::MOD) {
        mulExp->ops.push_back(tokenList.pop());
        mulExp->unaryExps.push_back(parseUnaryExp());
    }
    return mulExp;
}

//UnaryOp → '+' | '−' | '!'
UnaryOp *Parser::parseUnaryOp() {
    auto unaryOp = new UnaryOp();
    unaryOp->unaryOp = tokenList.pop();
    return unaryOp;
}

//UnaryExp → PrimaryExp | Ident '(' [FuncRParams] ')'
//      | UnaryOp UnaryExp
UnaryExp *Parser::parseUnaryExp() {
    auto unaryExp = new UnaryExp();
    if (tokenList.get()->tokenType == Token::IDENFR &&
    tokenList.getNext(1)->tokenType == Token::LPARENT){
        unaryExp->ident = tokenList.popExpect(Token::IDENFR);
        tokenList.popExpect(Token::LPARENT);
        if (tokenList.get()->tokenType != Token::RPARENT){
            unaryExp->funcRParams = parseFuncRParams();
        }
        tokenList.popExpect(Token::RPARENT);
    }
    else if (tokenList.get()->tokenType == Token::PLUS ||
    tokenList.get()->tokenType == Token::MINU ||
    tokenList.get()->tokenType == Token::NOT) {
        unaryExp->unaryOp = parseUnaryOp();
        unaryExp->unaryExp = parseUnaryExp();
    }
    else{
        unaryExp->primaryExp = parsePrimaryExp();
    }
    return unaryExp;
}

//PrimaryExp → '(' Exp ')' | LVal | Number
PrimaryExp *Parser::parsePrimaryExp() {
    auto primaryExp = new PrimaryExp();
    if (tokenList.get()->tokenType == Token::LPARENT){
        tokenList.popExpect(Token::LPARENT);
        primaryExp->exp = parseExp();
        tokenList.popExpect(Token::RPARENT);
    }
    else if (tokenList.get()->tokenType == Token::IDENFR){
        primaryExp->lVal = parseLVal();
    }
    else {
        primaryExp->number = parseNumber();
    }
    return primaryExp;
}

//FuncRParams → Exp { ',' Exp }
FuncRParams *Parser::parseFuncRParams() {
    auto funcRParams = new FuncRParams();
    funcRParams->exps.push_back(parseExp());
    while (tokenList.get()->tokenType == Token::COMMA){
        tokenList.popExpect(Token::COMMA);
        funcRParams->exps.push_back(parseExp());
    }
    return funcRParams;
}

//Exp → AddExp
Exp *Parser::parseExp() {
    auto exp = new Exp();
    exp->addExp = parseAddExp();
    return exp;
}

//LVal → Ident {'[' Exp ']'}
LVal *Parser::parseLVal() {
    auto lVal = new LVal();
    lVal->ident = tokenList.popExpect(Token::IDENFR);
    while (tokenList.get()->tokenType == Token::LBRACK){
        tokenList.popExpect(Token::LBRACK);
        lVal->exps.push_back(parseExp());
        tokenList.popExpect(Token::RBRACK);
    }
    return lVal;
}

// Number → IntConst
Number *Parser::parseNumber() {
    auto number = new Number();
    number->intConst = tokenList.popExpect(Token::INTCON);
    return number;
}

//VarDef → Ident { '[' ConstExp ']' }
// | Ident { '[' ConstExp ']' } '=' InitVal
VarDef *Parser::parseVarDef() {
    auto varDef = new VarDef();
    varDef->ident = tokenList.popExpect(Token::IDENFR);
    while (tokenList.get()->tokenType == Token::LBRACK){
        tokenList.popExpect(Token::LBRACK);
        varDef->constExps.push_back(parseConstExp());
        tokenList.popExpect(Token::RBRACK);
    }
    if (tokenList.get()->tokenType == Token::ASSIGN) {
        tokenList.popExpect(Token::ASSIGN);
        varDef->initval = parseInitVal();
    }
    return varDef;
}



// ConstInitVal → ConstExp
//  | '{' [ ConstInitVal { ',' ConstInitVal } ] '}'
ConstInitVal *Parser::parseConstInitVal() {
    auto constInitVal = new ConstInitVal();
    if (tokenList.get()->tokenType != Token::LBRACE){
        constInitVal->constExp = parseConstExp();
    }
    else {
        tokenList.popExpect(Token::LBRACE);
        if (tokenList.get()->tokenType != Token::RBRACE){
            constInitVal->constInitVals.push_back(parseConstInitVal());
            while (tokenList.get()->tokenType == Token::COMMA){
                tokenList.popExpect(Token::COMMA);
                constInitVal->constInitVals.push_back(parseConstInitVal());
            }
        }
        tokenList.popExpect(Token::RBRACE);
    }
    return constInitVal;
}

// FuncDef → FuncType Ident '(' [FuncFParams] ')' Block
FuncDef *Parser::parseFuncDef() {
#ifdef DEBUG
    std::cout << "funcdef" << std::endl;
#endif
    auto funcDef = new FuncDef();
    funcDef->funcType = parseFuncType();
    funcDef->ident = tokenList.popExpect(Token::IDENFR);
    tokenList.popExpect(Token::LPARENT);
    if (tokenList.get()->tokenType != Token::RPARENT){
        funcDef->funcFParams = parseFuncFParams();
    }
    tokenList.popExpect((Token::RPARENT));
    funcDef->block = parseBlock();
    return funcDef;
}

FuncType *Parser::parseFuncType() {
    auto funcType = new FuncType();
    funcType->funcType = tokenList.pop();
    return funcType;
}

// FuncFParams → FuncFParam { ',' FuncFParam }
FuncFParams *Parser::parseFuncFParams() {
    auto funcFParams = new FuncFParams();
    funcFParams->funcFParams.push_back(parseFuncFParam());
    while (tokenList.get()->tokenType == Token::COMMA){
        tokenList.popExpect(Token::COMMA);
        funcFParams->funcFParams.push_back(parseFuncFParam());
    }
    return funcFParams;
}

//FuncFParam → BType Ident ['[' ']' { '[' ConstExp ']' }]
FuncFParam *Parser::parseFuncFParam() {
    auto funcFParam = new FuncFParam();
    funcFParam->bType = parseBType();
    funcFParam->ident = tokenList.popExpect(Token::IDENFR);
    if (tokenList.get()->tokenType == Token::LBRACK){
        funcFParam->isArray = true;
        tokenList.popExpect(Token::LBRACK);
        tokenList.popExpect(Token::RBRACK);
        while (tokenList.get()->tokenType == Token::LBRACK) {
            tokenList.popExpect(Token::LBRACK);
            funcFParam->constExps.push_back(parseConstExp());
            tokenList.popExpect(Token::RBRACK);
        }
    }
    else {
        funcFParam->isArray = false;
    }
    return funcFParam;
}

// BType → 'int'
BType *Parser::parseBType() {
    auto bType = new BType();
    bType->bType = tokenList.popExpect(Token::INTTK);
    return bType;
}

// Block → '{' { BlockItem } '}'
Block *Parser::parseBlock() {
    auto block = new Block();
    tokenList.popExpect(Token::LBRACE);
    while (tokenList.get()->tokenType != Token::RBRACE){
        block->blockItems.push_back(parseBlockItem());
    }
    block->Rbrace = tokenList.popExpect(Token::RBRACE);
    return block;
}

// BlockItem → Decl | Stmt
BlockItem *Parser::parseBlockItem() {
    auto blockItem = new BlockItem();
    if (tokenList.get()->tokenType == Token::CONSTTK ||
    tokenList.get()->tokenType == Token::INTTK){
#ifdef DEBUG
        std::cout << "blockItem->decl" << std::endl;
#endif
        blockItem->decl = parseDecl();
    }
    else {
#ifdef DEBUG
        std::cout << "blockItem->stmt" << std::endl;
#endif
        blockItem->stmt = parseStmt();
    }
    return blockItem;
}

Stmt *Parser::parseStmt() {
    auto stmt = new Stmt();
    if (tokenList.get()->tokenType == Token::IFTK){
        stmt->ifStmt = parseIfStmt();
    }
    else if (tokenList.get()->tokenType == Token::FORTK){
        stmt->forStmt = parseFORStmt();
    }
    else if (tokenList.get()->tokenType == Token::BREAKTK){
        stmt->breakStmt = parseBreakStmt();
    }
    else if (tokenList.get()->tokenType == Token::CONTINUETK){
        stmt->continueStmt = parseContinueStmt();
    }
    else if (tokenList.get()->tokenType == Token::RETURNTK){
        stmt->returnStmt = parseReturnStmt();
    }
    else if (tokenList.get()->tokenType == Token::PRINTFTK){
        stmt->outputStmt = parseOutputStmt();
    }
    else if (tokenList.get()->tokenType == Token::LBRACE){
        stmt->blockStmt = parseBlockStmt();
    }
    //exp->addexp->mulexp->unaryexp->primaryexp->'('
    //                             ->ident '(
    //                             ->unaryop
    else if ((tokenList.get()->tokenType == Token::IDENFR && tokenList.getNext(1)->tokenType == Token::LPARENT) ||
    tokenList.get()->tokenType == Token::LPARENT ||
    tokenList.get()->tokenType == Token::PLUS || tokenList.get()->tokenType == Token::MINU ||
    tokenList.get()->tokenType == Token::SEMICN) {
        stmt->expStmt = parseExpStmt();
    }
    else {
        //得到当前的pos
        int prePos = tokenList.getPos();
        parseLVal();
        if (tokenList.getNext(1)->tokenType != Token::GETINTTK){
#ifdef DEBUG
            std::cout << "stmt->assignStmt" << std::endl;
#endif
            tokenList.backTo(prePos);
            stmt->assignStmt = parseAssignStmt();
        }
        else{
            tokenList.backTo(prePos);
#ifdef DEBUG
            std::cout << "inputstmt" << std::endl;
#endif
            stmt->inputStmt = parseInputStmt();
        }
    }
    return stmt;
}

//'if' '(' Cond ')' Stmt [ 'else' Stmt ]
IfStmt *Parser::parseIfStmt() {
    auto ifStmt = new IfStmt();
    tokenList.popExpect(Token::IFTK);
    tokenList.popExpect(Token::LPARENT);
    ifStmt->cond = parseCond();
    tokenList.popExpect(Token::RPARENT);
    ifStmt->ifStmt = parseStmt();
    if (tokenList.get()->tokenType == Token::ELSETK) {
        tokenList.popExpect(Token::ELSETK);
        ifStmt->elseStmt = parseStmt();
    }
    return ifStmt;
}

// Cond → LOrExp
Cond *Parser::parseCond() {
    auto cond = new Cond();
    cond->lOrExp = parseLOrExp();
    return cond;
}

//LOrExp -> LAndExp { '||' LAndExp }
LOrExp *Parser::parseLOrExp() {
    auto lOrExp = new LOrExp();
    lOrExp->lAndExps.push_back(parseLAndExp());
    while (tokenList.get()->tokenType == Token::OR) {
        lOrExp->ops.push_back(tokenList.pop());
        lOrExp->lAndExps.push_back(parseLAndExp());
    }
    return lOrExp;
}

//LAndExp -> EqExp { '&&' EqExp }
LAndExp *Parser::parseLAndExp() {
    auto lAndExp = new LAndExp();
    lAndExp->eqExps.push_back(parseEqExp());
    while (tokenList.get()->tokenType == Token::AND) {
        lAndExp->ops.push_back(tokenList.pop());
        lAndExp->eqExps.push_back(parseEqExp());
    }
    return lAndExp;
}

//EqExp -> RelExp { ('==' | '!=') RelExp }
EqExp *Parser::parseEqExp() {
    auto eqExp = new EqExp();
    eqExp->relExps.push_back(parseRelExp());
    while (tokenList.get()->tokenType == Token::EQL ||
    tokenList.get()->tokenType == Token::NEQ) {
        eqExp->ops.push_back(tokenList.pop());
        eqExp->relExps.push_back(parseRelExp());
    }
    return eqExp;
}

//RelExp -> AddExp { ('<' | '>' | '<=' | '>=') AddExp }
RelExp *Parser::parseRelExp() {
    auto relExp = new RelExp();
    relExp->addExps.push_back(parseAddExp());
    while (tokenList.get()->tokenType == Token::LSS ||
    tokenList.get()->tokenType == Token::GRE ||
    tokenList.get()->tokenType == Token::LEQ ||
    tokenList.get()->tokenType == Token::GEQ){
        relExp->ops.push_back(tokenList.pop());
        relExp->addExps.push_back(parseAddExp());
    }
    return relExp;
}

// 'for' '(' [ForStmt] ';' [Cond] ';' [ForStmt] ')' Stmt
FORStmt *Parser::parseFORStmt() {
    auto forStmt = new FORStmt();
    tokenList.popExpect(Token::FORTK);
    tokenList.popExpect(Token::LPARENT);
    if (tokenList.get()->tokenType != Token::SEMICN){
        forStmt->forStmt1 = parseForStmt();
    }
    tokenList.popExpect(Token::SEMICN);
    if (tokenList.get()->tokenType != Token::SEMICN){
        forStmt->cond = parseCond();
    }
    tokenList.popExpect(Token::SEMICN);
    if (tokenList.get()->tokenType != Token::RPARENT){
        forStmt->forStmt2 = parseForStmt();
    }
    tokenList.popExpect(Token::RPARENT);
    forStmt->stmt = parseStmt();
    return forStmt;
}

// ForStmt → LVal '=' Exp
ForStmt *Parser::parseForStmt() {
    auto forStmt = new ForStmt();
    forStmt->lVal = parseLVal();
    tokenList.popExpect(Token::ASSIGN);
    forStmt->exp = parseExp();
    return forStmt;
}

//  'break' ';'
BreakStmt *Parser::parseBreakStmt() {
    auto breakStmt = new BreakStmt();
    breakStmt->token = tokenList.popExpect(Token::BREAKTK);
    tokenList.popExpect(Token::SEMICN);
    return breakStmt;
}

//  'continue' ';'
ContinueStmt *Parser::parseContinueStmt() {
    auto continueStmt = new ContinueStmt();
    continueStmt->token = tokenList.popExpect(Token::CONTINUETK);
    tokenList.popExpect(Token::SEMICN);
    return continueStmt;
}

// 'return' [Exp] ';'
ReturnStmt *Parser::parseReturnStmt() {
#ifdef DEBUG
    std::cout << "start returnStmt" << std::endl;
#endif
    auto returnStmt = new ReturnStmt();
    returnStmt->returnTK = tokenList.popExpect(Token::RETURNTK);
    if (tokenList.get()->tokenType != Token::SEMICN) {
        returnStmt->exp = parseExp();
    }
    tokenList.popExpect(Token::SEMICN);
    return returnStmt;
}

// 'printf''('FormatString{','Exp}')'';'
OutputStmt *Parser::parseOutputStmt() {
    auto outputStmt = new OutputStmt();
    tokenList.popExpect(Token::PRINTFTK);
    tokenList.popExpect(Token::LPARENT);
    outputStmt->formatString = tokenList.popExpect(Token::STRCON);
    while (tokenList.get()->tokenType == Token::COMMA){
        tokenList.popExpect(Token::COMMA);
        outputStmt->exps.push_back(parseExp());
    }
    tokenList.popExpect(Token::RPARENT);
    tokenList.popExpect(Token::SEMICN);
    return outputStmt;
}

// Block
BlockStmt *Parser::parseBlockStmt() {
    auto blockStmt = new BlockStmt();
    blockStmt->block = parseBlock();
    return blockStmt;
}

//[Exp] ';'
ExpStmt *Parser::parseExpStmt() {
    auto expStmt = new ExpStmt();
    if (tokenList.get()->tokenType != Token::SEMICN){
        expStmt->exp = parseExp();
    }
    tokenList.popExpect(Token::SEMICN);
    return expStmt;
}

// LVal '=' Exp ';'
AssignStmt *Parser::parseAssignStmt() {
    auto assignStmt = new AssignStmt();
    assignStmt->lVal = parseLVal();
    tokenList.popExpect(Token::ASSIGN);
    assignStmt->exp = parseExp();
    tokenList.popExpect(Token::SEMICN);
    return assignStmt;
}

// LVal '=' 'getint''('')'';'
InputStmt *Parser::parseInputStmt() {
    auto inputStmt = new InputStmt();
    inputStmt->lVal = parseLVal();
    tokenList.popExpect(Token::ASSIGN);
    tokenList.popExpect(Token::GETINTTK);
    tokenList.popExpect(Token::LPARENT);
    tokenList.popExpect(Token::RPARENT);
    tokenList.popExpect(Token::SEMICN);
    return inputStmt;
}

MainFuncDef *Parser::parseMainFuncDef() {
    auto mainFuncDef = new MainFuncDef();
    tokenList.popExpect(Token::INTTK);
    tokenList.popExpect(Token::MAINTK);
    tokenList.popExpect(Token::LPARENT);
    tokenList.popExpect(Token::RPARENT);
    mainFuncDef->block = parseBlock();
#ifdef DEBUG
    std::cout << "mainFuncDef over" << std::endl;
#endif
    return mainFuncDef;
}

// InitVal → Exp | '{' [ InitVal { ',' InitVal } ] '}'/
InitVal *Parser::parseInitVal() {
    auto initVal = new InitVal();
    if (tokenList.get()->tokenType != Token::LBRACE){
        initVal->exp = parseExp();
    }
    else {
        tokenList.popExpect(Token::LBRACE);
        if (tokenList.get()->tokenType != Token::RBRACE) {
            initVal->initVals.push_back(parseInitVal());
            while (tokenList.get()->tokenType == Token::COMMA) {
                tokenList.popExpect(Token::COMMA);
                initVal->initVals.push_back(parseInitVal());
            }
        }
        tokenList.popExpect(Token::RBRACE);
    }
    return initVal;
}
//LOrExp → LAndExp | LOrExp '||' LAndExp




















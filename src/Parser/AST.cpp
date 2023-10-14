//
// Created by 安达楷 on 2023/9/23.
//
#define DEBUG

#include "AST.h"
#include "../Lexer/Token.h"
#include "../Visitor/Visitor.h"
#include <iostream>

std::ostream& operator<<(std::ostream &ostream, const CompUnit &obj) {
    for (auto decl : obj.decls) {
        ostream << *decl << std::endl;
    }
    for (auto funcDef : obj.funcDefs) {
        ostream << *funcDef << std::endl;
    }
    ostream << *obj.mainFuncDef << std::endl;
    ostream << "<CompUnit>";
    return ostream;
}

std::ostream& operator<<(std::ostream &ostream, const Decl &obj) {
    if (obj.constDecl != nullptr) {
        ostream << *obj.constDecl;
    }
    else {
        ostream << *obj.varDecl;
    }
    return ostream;
}

std::ostream& operator<<(std::ostream &ostream, const ConstDecl &obj) {
    ostream << Token::CONSTTK << std::endl;
    ostream << *obj.bType << std::endl;
    ostream << *obj.constDefs.at(0) << std::endl;
    for (size_t i = 1; i < obj.constDefs.size(); ++ i) {
        ostream << Token::COMMA << std::endl;
        ostream << *obj.constDefs.at(i) << std::endl;
    }
    ostream << Token::SEMICN << std::endl;
    ostream << "<ConstDecl>";
    return ostream;
}

std::ostream& operator<<(std::ostream &ostream, const BType &obj) {
    ostream << *obj.bType;
    return ostream;
}

std::ostream& operator<<(std::ostream &ostream, const ConstDef &obj) {
    ostream << *obj.ident << std::endl;
    for (auto constExp : obj.constExps) {
        ostream << Token::LBRACK << std::endl;
        ostream << *constExp << std::endl;
        ostream << Token::RBRACK << std::endl;
    }
    ostream << Token::ASSIGN << std::endl;
    ostream << *obj.constInitVal << std::endl;
    ostream << "<ConstDef>";
    return ostream;
}

std::ostream& operator<<(std::ostream &ostream, const ConstInitVal &obj) {
    if (obj.constExp != nullptr){
        ostream << *obj.constExp << std::endl;
    }
    else {
        ostream << Token::LBRACE << std::endl;
        if (!obj.constInitVals.empty()) {
            ostream << *obj.constInitVals.at(0) << std::endl;
            for (int i = 1; i < obj.constInitVals.size(); i ++ ){
                ostream << Token::COMMA << std::endl;
                ostream << *obj.constInitVals.at(i) << std::endl;
            }
        }
        ostream << Token::RBRACE << std::endl;
    }
    ostream << "<ConstInitVal>";
    return ostream;
}

std::ostream& operator<<(std::ostream &ostream, const VarDecl &obj) {
    ostream << *obj.bType << std::endl;
    ostream << *obj.varDefs.at(0) << std::endl;
    for (int i = 1; i < obj.varDefs.size(); i ++ ){
        ostream << Token::COMMA << std::endl;
        ostream << *obj.varDefs.at(i) << std::endl;
    }
    ostream << Token::SEMICN << std::endl;
    ostream << "<VarDecl>";
    return ostream;
}

std::ostream& operator<<(std::ostream &ostream, const VarDef &obj) {
    ostream << *obj.ident << std::endl;
    for (auto constExp : obj.constExps){
        ostream << Token::LBRACK << std::endl;
        ostream << *constExp << std::endl;
        ostream << Token::RBRACK << std::endl;
    }
    if (obj.initval != nullptr){
        ostream << Token::ASSIGN << std::endl;
        ostream << *obj.initval << std::endl;
    }
    ostream << "<VarDef>";
    return ostream;
}

std::ostream& operator<<(std::ostream &ostream, const InitVal &obj) {
    if (obj.exp != nullptr) {
        ostream << *obj.exp << std::endl;
    }
    else {
        ostream << Token::LBRACE << std::endl;
        if (!obj.initVals.empty()) {
            ostream << *obj.initVals.at(0) << std::endl;
            for (int i = 1; i < obj.initVals.size(); i ++ ){
                ostream << Token::COMMA << std::endl;
                ostream << *obj.initVals.at(i) << std::endl;
            }
        }
        ostream << Token::RBRACE << std::endl;
    }
    ostream << "<InitVal>";
    return ostream;
}

std::ostream& operator<<(std::ostream &ostream, const FuncDef &obj) {
    ostream << *obj.funcType << std::endl;
    ostream << *obj.ident << std::endl;
    ostream << Token::LPARENT << std::endl;
    if (obj.funcFParams != nullptr){
        ostream << *obj.funcFParams << std::endl;
    }
    ostream << Token::RPARENT << std::endl;
    ostream << *obj.block << std::endl;
    ostream << "<FuncDef>";
    return ostream;
}

std::ostream& operator<<(std::ostream &ostream, const MainFuncDef &obj) {
    ostream << Token::INTTK << std::endl;
    ostream << Token::MAINTK << std::endl;
    ostream << Token::LPARENT << std::endl;
    ostream << Token::RPARENT << std::endl;
    ostream << *obj.block << std::endl;
    ostream << "<MainFuncDef>";
    return ostream;
}

std::ostream& operator<<(std::ostream &ostream, const FuncType &obj) {
    ostream << *obj.funcType << std::endl;
    ostream << "<FuncType>";
    return ostream;
}

std::ostream& operator<<(std::ostream &ostream, const FuncFParams &obj) {
    ostream << *obj.funcFParams.at(0) << std::endl;
    for (int i = 1; i < obj.funcFParams.size(); i ++ ){
        ostream << Token::COMMA << std::endl;
        ostream << *obj.funcFParams.at(i) << std::endl;
    }
    ostream << "<FuncFParams>";
    return ostream;
}

std::ostream& operator<<(std::ostream &ostream, const FuncFParam &obj) {
    ostream << *obj.bType << std::endl;
    ostream << *obj.ident << std::endl;
    if (obj.isArray){
        ostream << Token::LBRACK << std::endl;
        ostream << Token::RBRACK << std::endl;
        for (auto constExp : obj.constExps){
            ostream << Token::LBRACK << std::endl;
            ostream << *constExp << std::endl;
            ostream << Token::RBRACK << std::endl;
        }
    }
    ostream << "<FuncFParam>";
    return ostream;
}

std::ostream& operator<<(std::ostream &ostream, const Block &obj) {
    ostream << Token::LBRACE << std::endl;
    for (auto blockItem : obj.blockItems){
        ostream << *blockItem << std::endl;
    }
    ostream << Token::RBRACE << std::endl;
    ostream << "<Block>";
    return ostream;
}

std::ostream& operator<<(std::ostream &ostream, const BlockItem &obj) {
    if (obj.decl != nullptr){
        ostream << *obj.decl;
    }
    else{
        ostream << *obj.stmt;
    }
    return ostream;
}

std::ostream& operator<<(std::ostream &ostream, const Stmt &obj) {
    if (obj.assignStmt != nullptr){
        ostream << *obj.assignStmt;
    }
    else if (obj.expStmt != nullptr){
        ostream << *obj.expStmt;
    }
    else if (obj.blockStmt != nullptr){
        ostream << *obj.blockStmt;
    }
    else if (obj.ifStmt != nullptr){
        ostream << *obj.ifStmt;
    }
    else if (obj.forStmt != nullptr){
        ostream << *obj.forStmt;
    }
    else if (obj.breakStmt != nullptr){
        ostream << *obj.breakStmt;
    }
    else if (obj.continueStmt != nullptr){
        ostream << *obj.continueStmt;
    }
    else if (obj.returnStmt != nullptr){
        ostream << *obj.returnStmt;
    }
    else if (obj.inputStmt != nullptr){
        ostream << *obj.inputStmt;
    }
    else if (obj.outputStmt != nullptr){
        ostream << *obj.outputStmt;
    }
    ostream << "<Stmt>";
    return ostream;
}

std::ostream& operator<<(std::ostream &ostream, const AssignStmt &obj) {
    ostream << *obj.lVal << std::endl;
    ostream << Token::ASSIGN << std::endl;
    ostream << *obj.exp << std::endl;
    ostream << Token::SEMICN << std::endl;
    return ostream;
}

std::ostream& operator<<(std::ostream &ostream, const ExpStmt &obj) {
    if (obj.exp != nullptr){
        ostream << *obj.exp << std::endl;
    }
    ostream << Token::SEMICN << std::endl;
    return ostream;
}

std::ostream& operator<<(std::ostream &ostream, const BlockStmt &obj) {
    ostream << *obj.block << std::endl;
    return ostream;
}

std::ostream& operator<<(std::ostream &ostream, const IfStmt &obj) {
    ostream << Token::IFTK << std::endl;
    ostream << Token::LPARENT << std::endl;
    ostream << *obj.cond << std::endl;
    ostream << Token::RPARENT << std::endl;
    ostream << *obj.ifStmt << std::endl;
    if (obj.elseStmt != nullptr){
        ostream << Token::ELSETK << std::endl;
        ostream << *obj.elseStmt << std::endl;
    }
    return ostream;
}

std::ostream& operator<<(std::ostream &ostream, const FORStmt &obj) {
    ostream << Token::FORTK << std::endl;
    ostream << Token::LPARENT << std::endl;
    if (obj.forStmt1 != nullptr){
        ostream << *obj.forStmt1 << std::endl;
    }
    ostream << Token::SEMICN << std::endl;
    if (obj.cond != nullptr){
        ostream << *obj.cond << std::endl;
    }
    ostream << Token::SEMICN << std::endl;
    if (obj.forStmt2 != nullptr){
        ostream << *obj.forStmt2 << std::endl;
    }
    ostream << Token::RPARENT << std::endl;
    ostream << *obj.stmt << std::endl;
    return ostream;
}

std::ostream& operator<<(std::ostream &ostream, const ForStmt &obj) {
    ostream << *obj.lVal << std::endl;
    ostream << Token::ASSIGN << std::endl;
    ostream << *obj.exp << std::endl;
    ostream << "<ForStmt>";
    return ostream;
}

std::ostream& operator<<(std::ostream &ostream, const BreakStmt &obj) {
    ostream << *obj.token << std::endl;
    ostream << Token::SEMICN << std::endl;
    return ostream;
}

std::ostream& operator<<(std::ostream &ostream, const ContinueStmt &obj) {
    ostream << *obj.token << std::endl;
    ostream << Token::SEMICN << std::endl;
    return ostream;
}

std::ostream& operator<<(std::ostream &ostream, const ReturnStmt &obj) {
    ostream << Token::RETURNTK << std::endl;
    if (obj.exp != nullptr){
        ostream << *obj.exp << std::endl;
    }
    ostream << Token::SEMICN << std::endl;
    return ostream;
}

std::ostream& operator<<(std::ostream &ostream, const InputStmt &obj) {
    ostream << *obj.lVal << std::endl;
    ostream << Token::ASSIGN << std::endl;
    ostream << Token::GETINTTK << std::endl;
    ostream << Token::LPARENT << std::endl;
    ostream << Token::RPARENT << std::endl;
    ostream << Token::SEMICN << std::endl;
    return ostream;
}

std::ostream& operator<<(std::ostream &ostream, const OutputStmt &obj) {
    ostream << Token::PRINTFTK << std::endl;
    ostream << Token::LPARENT << std::endl;
    ostream << *obj.formatString << std::endl;
    for (auto exp : obj.exps){
        ostream << Token::COMMA << std::endl;
        ostream << *exp << std::endl;
    }
    ostream << Token::RPARENT << std::endl;
    ostream << Token::SEMICN << std::endl;
    return ostream;
}

std::ostream& operator<<(std::ostream &ostream, const Exp &obj) {
    ostream << *obj.addExp << std::endl;
    ostream << "<Exp>";
    return ostream;
}

std::ostream& operator<<(std::ostream &ostream, const Cond &obj) {
    ostream << *obj.lOrExp << std::endl;
    ostream << "<Cond>";
    return ostream;
}

std::ostream& operator<<(std::ostream &ostream, const LVal &obj) {
    ostream << *obj.ident << std::endl;
    for (auto exp : obj.exps){
        ostream << Token::LBRACK << std::endl;
        ostream << *exp << std::endl;
        ostream << Token::RBRACK<< std::endl;
    }
    ostream << "<LVal>";
    return ostream;
}

std::ostream& operator<<(std::ostream &ostream, const PrimaryExp &obj) {
    if (obj.exp != nullptr) {
        ostream << Token::LPARENT << std::endl;
        ostream << *obj.exp << std::endl;
        ostream << Token::RPARENT << std::endl;
    }
    else if (obj.lVal != nullptr) {
        ostream << *obj.lVal << std::endl;
    }
    else if (obj.number != nullptr) {
        ostream << *obj.number << std::endl;
    }
    ostream << "<PrimaryExp>";
    return ostream;
}

std::ostream& operator<<(std::ostream &ostream, const Number &obj) {
    ostream << *obj.intConst << std::endl;
    ostream << "<Number>";
    return ostream;
}

std::ostream& operator<<(std::ostream &ostream, const UnaryExp &obj) {
    if (obj.primaryExp != nullptr) {
        ostream << *obj.primaryExp << std::endl;
    }
    else if (obj.ident != nullptr) {
        ostream << *obj.ident << std::endl;
        ostream << Token::LPARENT << std::endl;
        if (obj.funcRParams != nullptr) {
            ostream << *obj.funcRParams << std::endl;
        }
        ostream << Token::RPARENT << std::endl;
    }
    else if (obj.unaryOp != nullptr) {
        ostream << *obj.unaryOp << std::endl;
        ostream << *obj.unaryExp << std::endl;
    }
    ostream << "<UnaryExp>";
    return ostream;
}

std::ostream& operator<<(std::ostream &ostream, const UnaryOp &obj) {
    ostream << *obj.unaryOp << std::endl;
    ostream << "<UnaryOp>";
    return ostream;
}

std::ostream& operator<<(std::ostream &ostream, const FuncRParams &obj) {
    ostream << *obj.exps.at(0) << std::endl;
    for (int i = 1; i < obj.exps.size(); i ++ ){
        ostream << Token::COMMA << std::endl;
        ostream << *obj.exps.at(i) << std::endl;
    }
    ostream << "<FuncRParams>";
    return ostream;
}

std::ostream& operator<<(std::ostream &ostream, const MulExp &obj) {
    ostream << *obj.unaryExps.at(0) << std::endl;
    for (int i = 1; i < obj.unaryExps.size(); i ++ ) {
        ostream << "<MulExp>" << std::endl;
        ostream << *obj.ops.at(i - 1) << std::endl;
        ostream << *obj.unaryExps.at(i) << std::endl;
    }
    ostream << "<MulExp>";
    return ostream;
}

std::ostream& operator<<(std::ostream &ostream, const AddExp &obj) {
    ostream << *obj.mulExps.at(0) << std::endl;
    for (int i = 1; i < obj.mulExps.size(); i ++ ) {
        ostream << "<AddExp>" << std::endl;
        ostream << *obj.ops.at(i - 1) << std::endl;
        ostream << *obj.mulExps.at(i) << std::endl;
    }
    ostream << "<AddExp>";
    return ostream;
}

std::ostream& operator<<(std::ostream &ostream, const RelExp &obj) {
    ostream << *obj.addExps.at(0) << std::endl;
    for (int i = 1; i < obj.addExps.size(); i ++ ) {
        ostream << "<RelExp>" << std::endl;
        ostream << *obj.ops.at(i - 1) << std::endl;
        ostream << *obj.addExps.at(i) << std::endl;
    }
    ostream << "<RelExp>";
    return ostream;
}

std::ostream& operator<<(std::ostream &ostream, const EqExp &obj) {
    ostream << *obj.relExps.at(0) << std::endl;
    for (int i = 1; i < obj.relExps.size(); i ++ ) {
        ostream << "<EqExp>" << std::endl;
        ostream << *obj.ops.at(i - 1) << std::endl;
        ostream << *obj.relExps.at(i) << std::endl;
    }
    ostream << "<EqExp>";
    return ostream;
}

std::ostream& operator<<(std::ostream &ostream, const LAndExp &obj) {
    ostream << *obj.eqExps.at(0) << std::endl;
    for (int i = 1; i < obj.eqExps.size(); i ++ ) {
        ostream << "<LAndExp>" << std::endl;
        ostream << *obj.ops.at(i - 1) << std::endl;
        ostream << *obj.eqExps.at(i) << std::endl;
    }
    ostream << "<LAndExp>";
    return ostream;
}

std::ostream& operator<<(std::ostream &ostream, const LOrExp &obj) {
    ostream << *obj.lAndExps.at(0) << std::endl;
    for (int i = 1; i < obj.lAndExps.size(); i ++ ) {
        ostream << "<LOrExp>" << std::endl;
        ostream << *obj.ops.at(i - 1) << std::endl;
        ostream << *obj.lAndExps.at(i) << std::endl;
    }
    ostream << "<LOrExp>";
    return ostream;
}

std::ostream& operator<<(std::ostream &ostream, const ConstExp &obj) {
    ostream << *obj.addExp << std::endl;
    ostream << "<ConstExp>";
    return ostream;
}

bool ConstDef::isArray() {
    return !constExps.empty();
}

bool ConstDef::isInit() {
    return true;
}

bool VarDef::isArray() {
    return !constExps.empty();
}

bool VarDef::isInit() {
    return initval != nullptr;
}

bool ConstInitVal::isArray() {
    return constExp == nullptr;
}

bool InitVal::isArray() {
    return exp == nullptr;
}

int Exp::getDim() {
    if (addExp != nullptr) {
        if (!addExp->mulExps.empty()) {
            if (!addExp->mulExps[0]->unaryExps.empty()) {
                auto tmpUnaryExp = addExp->mulExps[0]->unaryExps[0];
                int dim = tmpUnaryExp->getDim();
#ifdef DEBUG
                std::cout << "in Exp::getDIm, the realDim is: " << dim << std::endl;
#endif
                return dim;
            }
        }
    }
    std::cout << "Error in exp::getDim()" << std::endl;
    return -100;
}

int UnaryExp::getDim() {
#ifdef DEBUG
    std::cout << "Start UnaryExp::getDim()" << std::endl;
#endif
    if (this->primaryExp != nullptr) {
        return this->primaryExp->getDim();
    }
    else if (this->ident != nullptr) {
        // 如果是函数调用，就只会返回int型
        // 错了！还真会返回void型
        auto funcSymbol = dynamic_cast<FuncSymbol*>(Visitor::curTable->getSymbol(this->ident->content, true));
        return funcSymbol->basicType == BasicType::INT ? 0 : -1;

    }
    else if (this->unaryExp != nullptr) {
        return this->unaryExp->getDim();
    }
    std::cout << "Error in UnaryExp::getDim" << std::endl;
    return -100;
}

int PrimaryExp::getDim() {
#ifdef DEBUG
    std::cout << "start PrimaryExp::getDim" << std::endl;
#endif
    if (this->lVal != nullptr) {
        int formDim = this->lVal->exps.size();  // 看上去的维度
        auto ident = this->lVal->ident;
        auto name = ident->content;
        auto symbol = Visitor::curTable->getSymbol(name, true);
        int realDim = (int)(symbol->getDim()) - formDim;   // 实际传入的维度
        return realDim;
    }
    else if (this->number != nullptr) {
        return 0;
    }
    else if (this->exp != nullptr) {
        return this->exp->getDim();
    }
    std::cout << "error in PrimaryExp::getDIm" << std::endl;
    return -100;
}

int FuncDef::getFuncFParamNumber() {
    if (this->funcFParams == nullptr) {
        return 0;
    }
    return this->funcFParams->funcFParams.size();
}
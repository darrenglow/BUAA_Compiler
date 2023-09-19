//
// Created by 安达楷 on 2023/9/13.
//
#include "Token.h"
#include <iostream>

//与tokenType一一对应
std::string Token::type2str[50] = {
        "IDENFR", "INTCON", "STRCON", "MAINTK", "CONSTTK", "INTTK", "BREAKTK",
        "CONTINUETK", "IFTK", "ELSETK", "NOT", "AND", "OR", "FORTK", "GETINTTK",
        "PRINTFTK", "RETURNTK", "PLUS", "MINU", "VOIDTK", "MULT", "DIV", "MOD", "LSS",
        "LEQ", "GRE", "GEQ", "EQL", "NEQ", "ASSIGN", "SEMICN", "COMMA", "LPARENT", "RPARENT",
        "LBRACK", "RBRACK", "LBRACE", "RBRACE", "UNDEFINE"
//        "Ident", "IntConst", "FormatString", "main", "const", "int", "break",
//        "continue", "if", "else", "!", "&&", "||", "while", "getint",
//        "printf", "return", "+", "-", "void", "*", "/", "%", "<",
//        "<=", ">", ">=", "==", "!=", "=", ";", ",", "(", ")",
//        "[", "]", "{", "}"
};

std::ostream& operator<<(std::ostream &ostream, const Token &token) {
    ostream << Token::type2str[token.tokenType] << " " << token.content;
    return ostream;
}


void TokenList::addToken(Token *token) {
    tokens.push_back(token);
}
std::ostream& operator<<(std::ostream &ostream, const TokenList &tokenList) {
    for (auto token : tokenList.tokens) {
        ostream << Token::type2str[token->tokenType] << " " << token->content << std::endl;
    }
    return ostream;
}



//
// Created by 安达楷 on 2023/9/13.
//

#ifndef BUAA_COMPILER_TOKEN_H
#define BUAA_COMPILER_TOKEN_H

#include <string>
#include <vector>
class Token{
public:
    enum TokenType{
        IDENFR, INTCON, STRCON, MAINTK, CONSTTK, INTTK, BREAKTK,
        CONTINUETK, IFTK, ELSETK, NOT, AND, OR, WHILETK, GETINTTK,
        PRINTFTK, RETURNTK, PLUS, MINU, VOIDTK, MULT, DIV, MOD, LSS,
        LEQ, GRE, GEQ, EQL, NEQ, ASSIGN, SEMICN, COMMA, LPARENT, RPARENT,
        LBRACK, RBRACK, LBRACE, RBRACE, UNDEFINE
    };
private:
    TokenType tokenType;
    std::string content;
    int val;
    int line;
public:
    static std::string type2str[50];
    Token(TokenType tokenType_, std::string content_, int line_, int val_=0)
        : tokenType(tokenType_), content(content_), line(line_), val(val_) {}
    friend std::ostream &operator<<(std::ostream &ostream, const Token &token);
};
#endif //BUAA_COMPILER_TOKEN_H

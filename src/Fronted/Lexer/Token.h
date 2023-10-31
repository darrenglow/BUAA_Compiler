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
        CONTINUETK, IFTK, ELSETK, NOT, AND, OR, FORTK, GETINTTK,
        PRINTFTK, RETURNTK, PLUS, MINU, VOIDTK, MULT, DIV, MOD, LSS,
        LEQ, GRE, GEQ, EQL, NEQ, ASSIGN, SEMICN, COMMA, LPARENT, RPARENT,
        LBRACK, RBRACK, LBRACE, RBRACE, UNDEFINE
    };
    friend std::ostream &operator<<(std::ostream &ostream, const Token::TokenType &token);
public:
    TokenType tokenType;
    std::string content;
    int val;
    int line;

    static std::string type2str[50];
    Token(TokenType tokenType_, std::string content_, int line_, int val_=0)
        : tokenType(tokenType_), content(content_), line(line_), val(val_) {};
    Token(TokenType tokenType_, std::string content_)
        : tokenType(tokenType_), content(content_) {}
    friend std::ostream &operator<<(std::ostream &ostream, const Token &token);
};


class TokenList{
private:
    std::vector<Token*> tokens;
    int pos=0;
public:
    std::vector<Token*> getTokens();
    //判断是否还有下一个token
    bool hasNext();
    ///插入token
    void addToken(Token* token);
    //获取tokenList当前的token
    Token* get();
    //获取tokenList后面第i个的token
    Token* getNext(int i);
    //弹出当前token
    Token* pop();
    //弹出当前希望的TokenType，如果不是就报错
    Token* popExpect(Token::TokenType type);
    //用于回退到指定的位置
    void backTo(int x);
    //得到当前的位置
    int getPos();
};
#endif //BUAA_COMPILER_TOKEN_H

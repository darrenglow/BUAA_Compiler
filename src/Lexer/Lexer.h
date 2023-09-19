//
// Created by 安达楷 on 2023/9/13.
//

#ifndef BUAA_COMPILER_LEXER_H
#define BUAA_COMPILER_LEXER_H

#include <string>
#include <vector>
#include <fstream>
#include <map>
#include "Token.h"

class Lexer{
private:
    TokenList tokens;
    std::string fileStr;
    int curLine = 1;
    int pos = 0;
    char ch;
    static std::map<std::string, Token::TokenType> str2type;
public:
    Lexer(std::string& fileStr_) : fileStr(fileStr_) {}


    void lex();
    char getChar();
    void nextToken();
    void printTokens() const;
    TokenList& getTokens() const;
};
#endif //BUAA_COMPILER_LEXER_H

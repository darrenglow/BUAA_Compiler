//
// Created by 安达楷 on 2023/9/19.
//

#ifndef BUAA_COMPILER_PARSER_H
#define BUAA_COMPILER_PARSER_H

#include "../Lexer/Token.h"
#include <vector>

class Parser{
private:
    TokenList &tokens;
public:
    Parser(TokenList& tokens_) : tokens(tokens_) {}
    void printParse();
};

#endif //BUAA_COMPILER_PARSER_H

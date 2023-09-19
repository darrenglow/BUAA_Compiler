//
// Created by 安达楷 on 2023/9/19.
//

#ifndef BUAA_COMPILER_PARSER_H
#define BUAA_COMPILER_PARSER_H

#include "../Lexer/Token.h"
#include <vector>

class Parser{
private:
    TokenList &tokenList;
public:
    explicit Parser(TokenList& tokenList_) : tokenList(tokenList_) {}

    void parse();
    void printParse();
};

#endif //BUAA_COMPILER_PARSER_H

//
// Created by 安达楷 on 2023/9/19.
//

#ifndef BUAA_COMPILER_PARSER_H
#define BUAA_COMPILER_PARSER_H

#include "../Lexer/Token.h"
#include <vector>

class Parser{
private:
    std::vector<Token*> tokens;
public:
    Parser(std::vector<Token*>& tokens_) : tokens(tokens_){}
};

#endif //BUAA_COMPILER_PARSER_H

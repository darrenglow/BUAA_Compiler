#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "Lexer/Lexer.h"
#include "Parser/Parser.h"

#define LexicalAnalysis
#define ParseAnalysis

static const std::string prePath = "../Test/LexicalAnalysis/";
//static const std::string prePath = "./";
std::ifstream input(prePath + "testfile.txt");
std::ofstream output(prePath + "output.txt");

int main()
{
    // 文件的读取
    std::stringstream buffer;
    buffer << input.rdbuf(); // 将文件内容读取到缓冲区
    std::string fileStr = buffer.str(); // 将缓冲区内容转换为字符串

    // Lexer Initialization
    auto *lexer = new Lexer(fileStr);
    lexer -> lex();
    auto *parser = new Parser(lexer->getTokens());
#ifdef LexicalAnalysis
    lexer->printTokens();
#endif
#ifdef ParseAnalysis
    parser->printParse();
#endif
    return 0;
}


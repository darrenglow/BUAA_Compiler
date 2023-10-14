#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "Lexer/Lexer.h"
#include "Parser/Parser.h"
#include "Error/ErrorTable.h"
#include "Visitor/Visitor.h"
//#define LexicalAnalysis
//#define ParseAnalysis
#define ErrorHandle


//static const std::string prePath = "../Test/";
static const std::string prePath = "./";
std::ifstream input(prePath + "testfile.txt");
std::ofstream output(prePath + "output.txt");
std::ofstream errorOutput(prePath + "error.txt");
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
    parser -> parse();
    auto *visitor = new Visitor(parser->getAST());
    visitor -> visit();
#ifdef LexicalAnalysis
    lexer->printTokens();
#endif
#ifdef ParseAnalysis
    parser -> printAST();
#endif
#ifdef ErrorHandle
    ErrorTable::getInstance().printErrors();
#endif
    return 0;
}


#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "Fronted/Lexer/Lexer.h"
#include "Fronted/Parser/Parser.h"
#include "Fronted/Error/ErrorTable.h"
#include "Middle/Visitor/Visitor.h"
#include "Middle/MiddleCode.h"
#include "Backend/MipsGenerator.h"

//#define LexicalAnalysis
//#define ParseAnalysis
#define ErrorHandle
//#define MiddleCodeGeneration
#define MipsGeneration

//static const std::string prePath = "../Test/";
static const std::string prePath = "./";
std::ifstream input(prePath + "testfile.txt");
std::ofstream output(prePath + "output.txt");
std::ofstream middleCodeOutput(prePath + "middleCode.txt");
std::ofstream mipsOutput(prePath + "mips.txt");
std::ofstream errorOutput(prePath + "error.txt");
int main()
{
    // 文件的读取
    std::stringstream buffer;
    buffer << input.rdbuf(); // 将文件内容读取到缓冲区
    std::string fileStr = buffer.str(); // 将缓冲区内容转换为字符串

    // =========== FRONTED ==========
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
    if (ErrorTable::getInstance().hasError()) {
        ErrorTable::getInstance().printErrors();
        return 0;
    }
#endif
    //============ MIDDLE =================
#ifdef MiddleCodeGeneration
    MiddleCode::doMiddleCodeGeneration();
#endif
    //============ BACKEND ================
#ifdef MipsGeneration
    MipsGenerator::getInstance().doMipsGeneration();
#endif
    return 0;
}


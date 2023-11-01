cmake_minimum_required(VERSION 3.15)
project(BUAA_Compiler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)

# 添加源文件
aux_source_directory(./src DIR_SRC)
aux_source_directory(src/Fronted/Lexer DIR_Lexer)

add_executable(BUAA_Compiler ${DIR_SRC} ${DIR_Lexer} src/Fronted/Parser/Parser.h src/Fronted/Parser/Parser.cpp src/Fronted/Parser/AST.h src/Fronted/Parser/AST.cpp src/Fronted/Error/Error.h src/Fronted/Error/ErrorTable.h src/Fronted/Error/ErrorTable.cpp src/Middle/Visitor/Visitor.h src/Middle/Visitor/Visitor.cpp src/Middle/Symbol/Symbol.h src/Middle/Symbol/SymbolTable.h src/Middle/Symbol/SymbolTable.cpp src/Middle/MiddleCode.h src/Middle/MiddleCode.cpp src/Middle/MiddleCodeItem.h src/Middle/MiddleCodeItem.cpp src/Middle/Util/Calculate.h src/Middle/Util/Calculate.cpp src/Util/Debug.h src/Middle/Symbol/Symbol.cpp src/Middle/Util/Flatten.h src/Util/Exception.h src/Middle/Intermediate/Intermediate.h src/Middle/Intermediate/Immediate.h)
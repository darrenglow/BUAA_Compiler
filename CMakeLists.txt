cmake_minimum_required(VERSION 3.15)
project(BUAA_Compiler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)

# 添加源文件
aux_source_directory(./src DIR_SRC)
aux_source_directory(./src/Lexer DIR_Lexer)

add_executable(BUAA_Compiler ${DIR_SRC} ${DIR_Lexer} src/Parser/Parser.h src/Parser/Parser.cpp)
//
// Created by 安达楷 on 2023/10/4.
//

#ifndef BUAA_COMPILER_ERROR_H
#define BUAA_COMPILER_ERROR_H
#include <string>

class Error {
public:
    enum ErrorType {
            ILLEGAL_FORMATSTRING,             // a
            DUPLICATE_IDENT,           // b
            UNDEFINED_IDENT,           // c
            MISMATCH_PARAM_NUM,        // d
            MISMATCH_PARAM_TYPE,       // e
            VOID_MISMATCH_RETURN,        // f
            LACK_RETURN,         // g
            CHANGE_CONST,        // h
            LACK_SEMICN,            // i
            LACK_RIGHT_PARENT,         // j
            LACK_RIGHT_BRACK,        // k
            PRINTF_MISMATCH_NUM,    // l
            BREAK_CONTINUE_OUT_LOOP    // m
    };
    const std::string type2String[13] = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m"};

    ErrorType errorType;
    int line;
    Error(const ErrorType errorType_, const int line_) : errorType(errorType_), line(line_) {}

    std::string getTypeString(ErrorType type) {
        return type2String[type];
    }
};

#endif //BUAA_COMPILER_ERROR_H

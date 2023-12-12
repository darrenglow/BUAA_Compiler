//
// Created by 安达楷 on 2023/10/24.
//

#ifndef BUAA_COMPILER_MIDDLECODE_H
#define BUAA_COMPILER_MIDDLECODE_H

#include <iostream>
#include <vector>
//#include "MiddleCodeItem/Middle.h"
#include "MiddleCodeItem/Func.h"

class Func;
class MiddleCode {
public:
    static MiddleCode& getInstance() {
        static MiddleCode instance;
        return instance;
    }
    static void doMiddleCodeGeneration();

    std::vector<ValueSymbol*> globalValues;
    std::vector<std::string> globalStrings;
    std::vector<Func*> funcs;

    void addGlobalValues(ValueSymbol* valueSymbol);
    void addGlobalStrings(std::string str);
    void addFuncs(Func *func);

    friend std::ostream &operator<<(std::ostream &ostream, const MiddleCode &middleCode);
private:
    MiddleCode()=default;
    ~MiddleCode()=default;


};

#endif //BUAA_COMPILER_MIDDLECODE_H

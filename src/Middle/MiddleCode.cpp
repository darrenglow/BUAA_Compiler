//
// Created by 安达楷 on 2023/10/24.
//
#include "MiddleCode.h"
#include <iostream>
#include <fstream>

std::ostream& operator<<(std::ostream &ostream, const MiddleCode &middleCode) {
    ostream << "######### Middle Code Start #########" << std::endl;

    ostream << "GLOBAL VALUE: " << std::endl;
    for (auto x : middleCode.globalValues) {
        if (x->valueType == SINGLE) {
            ostream << "VAR ";
            ostream << x->name << " " << x->initValue << std::endl;
        }
        else if (x->valueType == ARRAY) {
            ostream << "ARRAY ";
            ostream << x->name << " ";
            int len = x->initValues.size();
            for (int i = 0; i < len - 1; i ++ )
                ostream << x->initValues[i] << ",";
            ostream << x->initValues[len - 1] << std::endl;
        }
    }

    ostream << "GLOBAL STRING: " << std::endl;
    int cnt = 0;
    for (const auto& x : middleCode.globalStrings) {
        ostream << "str_" << cnt ++ << ": " << x << std::endl;
    }

    ostream << "###########################" << std::endl;

    for (auto x : middleCode.funcs) {
        ostream << *x << std::endl;
    }

    ostream << "######### Middle Code End #########" << std::endl;
    return ostream;
}

extern std::ofstream middleCodeOutput;
void MiddleCode::doMiddleCodeGeneration() {
    middleCodeOutput << MiddleCode::getInstance();
}

void MiddleCode::addGlobalValues(ValueSymbol *valueSymbol) {
    globalValues.push_back(valueSymbol);
}

void MiddleCode::addGlobalStrings(std::string str) {
    globalStrings.push_back(str);
}

void MiddleCode::addFuncs(Func *func) {
    funcs.push_back(func);
}


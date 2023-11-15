//
// Created by 安达楷 on 2023/11/14.
//

#ifndef BUAA_COMPILER_NOCHANGEVALUE_H
#define BUAA_COMPILER_NOCHANGEVALUE_H

#include <unordered_map>
#include "../Symbol/Symbol.h"
#include "../MiddleCodeItem.h"

class NoChangeValue {
public:
    static NoChangeValue& getInstance() {
        static NoChangeValue instance;
        return instance;
    }
    std::unordered_map<ValueSymbol*, int> noChangeValues;
    std::vector<ValueSymbol*> inFuncLVals;  // 处理函数参数的不变

    void add(ValueSymbol* valueSymbol, int value);
    void remove(ValueSymbol* valueSymbol);
    void replace(std::vector<Func*> &funcs);
    bool exist(ValueSymbol* valueSymbol);
private:
    NoChangeValue()= default;
    ~NoChangeValue()= default;
};

#endif //BUAA_COMPILER_NOCHANGEVALUE_H

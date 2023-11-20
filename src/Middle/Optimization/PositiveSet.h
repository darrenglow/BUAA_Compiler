//
// Created by 安达楷 on 2023/11/20.
//

#ifndef BUAA_COMPILER_POSITIVESET_H
#define BUAA_COMPILER_POSITIVESET_H

#include <set>
#include "../Symbol/Symbol.h"

class PositiveSet {
public:
    std::set<ValueSymbol*> symbols;

    PositiveSet()=default;
    explicit  PositiveSet(PositiveSet *posSet) {
        for (auto symbol : posSet->symbols) {
            symbols.insert(symbol);
        }
    }

    void add(ValueSymbol* symbol);

    void remove(ValueSymbol *symbol);

    bool contain(ValueSymbol *symbol);

    void plus(PositiveSet *posSet);

    void sub(PositiveSet *posSet);

    bool equalTo(PositiveSet *posSet);

};

#endif //BUAA_COMPILER_POSITIVESET_H

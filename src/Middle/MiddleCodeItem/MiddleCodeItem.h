//
// Created by 安达楷 on 2023/10/24.
//

#ifndef BUAA_COMPILER_MIDDLECODEITEM_H
#define BUAA_COMPILER_MIDDLECODEITEM_H

#define OVERRIDE_OUTPUT std::ostream& output(std::ostream &os) const override

#include <string>
#include <iostream>
#include <vector>
#include "../Symbol/Symbol.h"
#include "../Intermediate/Immediate.h"
#include "../Symbol/SymbolTable.h"

class MiddleCodeItem {
public:
    MiddleCodeItem()=default;

    std::vector<int> killSetIndex;

    int index;

    void setIndex(int x);

    virtual std::ostream& output(std::ostream &os) const = 0;

    virtual Intermediate* getLeftIntermediate();
    friend std::ostream& operator<<(std::ostream& os, const MiddleCodeItem& obj);
};

struct CompareMiddleCodeItem {
    bool operator()(const MiddleCodeItem* a, const MiddleCodeItem* b) const {
        return a->index < b->index;
    }
};
#endif //BUAA_COMPILER_MIDDLECODEITEM_H

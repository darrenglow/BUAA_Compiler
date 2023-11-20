//
// Created by 安达楷 on 2023/11/19.
//

#ifndef BUAA_COMPILER_DEFINITIONSET_H
#define BUAA_COMPILER_DEFINITIONSET_H

#include "../MiddleCodeItem/MiddleCodeItem.h"
#include <set>
class DefinitionSet {
public:
    // 遍历时会按照code的index从大到小
    std::set<MiddleCodeItem *, CompareMiddleCodeItem> defs;
    int setIndex[1000] = {0};

    void add(MiddleCodeItem *def, int index);

    DefinitionSet() = default;

    DefinitionSet* sub(DefinitionSet *defSet);

    DefinitionSet* plus(DefinitionSet *defSet);

    DefinitionSet *erase(MiddleCodeItem *def);

    DefinitionSet *eraseNew(MiddleCodeItem *def);
};
#endif //BUAA_COMPILER_DEFINITIONSET_H

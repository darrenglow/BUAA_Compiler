//
// Created by 安达楷 on 2023/11/19.
//

#include "DefinitionSet.h"

void DefinitionSet::add(MiddleCodeItem *def, int index) {
    defs.insert(def);
    setIndex[index] = 1;
}

DefinitionSet* DefinitionSet::sub(DefinitionSet *defSet) {
    if (defSet == nullptr) {
        return this;
    }
    for (auto def : defSet->defs) {
        if (defs.count(def) > 0)
            defs.erase(def);
    }
    for (int i = 0; i < 100000; i ++ ) {
        if (defSet->setIndex[i] > 0) {
            setIndex[i] = 0;
        }
    }
    return this;
}

DefinitionSet * DefinitionSet::plus(DefinitionSet *defSet) {
    if (defSet == nullptr) {
        return this;
    }
    for (auto def : defSet->defs) {
        defs.insert(def);
    }
    for (int i = 0; i < 100000; i ++ ){
        if (defSet->setIndex[i] > 0) {
            setIndex[i] = 1;
        }
    }
    return this;
}

DefinitionSet * DefinitionSet::erase(MiddleCodeItem *def) {
    defs.erase(def);
    setIndex[def->index] = 0;
    return this;
}

DefinitionSet * DefinitionSet::eraseNew(MiddleCodeItem *def) {
    auto newDefSet = new DefinitionSet();
    for (auto x : defs) {
        newDefSet->add(x, x->index);
    }
    for (int i = 0; i < 100000; i ++ ) {
        newDefSet->setIndex[i] = setIndex[i];
    }
    return newDefSet->erase(def);
}

bool DefinitionSet::equalTo(DefinitionSet *defSet) {
    for (int i = 0; i < 100000; i ++ ) {
        if (setIndex[i] != defSet->setIndex[i]) {
            return false;
        }
    }
    return true;
}
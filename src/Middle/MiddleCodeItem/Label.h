//
// Created by 安达楷 on 2023/11/19.
//

#ifndef BUAA_COMPILER_LABEL_H
#define BUAA_COMPILER_LABEL_H

#include "MiddleCodeItem.h"

class Label : public MiddleCodeItem {
public:
    static int count;
    std::string label;
    Label() : MiddleCodeItem(MiddleCodeItem::Label), label("LABEL_" + std::to_string(count)) {
        count ++ ;
    }
    explicit Label(std::string &label_) : MiddleCodeItem(MiddleCodeItem::Label), label(label_) {}
    OVERRIDE_OUTPUT;
};

#endif //BUAA_COMPILER_LABEL_H

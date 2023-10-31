//
// Created by 安达楷 on 2023/10/4.
//

#include "ErrorTable.h"
#include <iostream>
#include <fstream>
#include <algorithm>

void ErrorTable::addError(Error *error) {
    errors.push_back(error);
}

extern std::ofstream errorOutput;

void ErrorTable::printErrors() {
    std::sort(errors.begin(), errors.end(), [](const Error* a, const Error* b) {
        return a->line < b->line;
    });

    for (auto error : errors) {
        errorOutput << std::to_string(error->line) << " " << error->getTypeString(error->errorType) << std::endl;
    }
}


bool ErrorTable::hasError() const {
    return !errors.empty();
}
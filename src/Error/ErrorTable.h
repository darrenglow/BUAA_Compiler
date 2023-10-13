//
// Created by 安达楷 on 2023/10/4.
//

#ifndef BUAA_COMPILER_ERRORTABLE_H
#define BUAA_COMPILER_ERRORTABLE_H

#include <vector>
#include "Error.h"

class ErrorTable {
public:
    static ErrorTable& getInstance() {
        static ErrorTable instance;
        return instance;
    }

    void addError(Error* error);
    void printErrors();

private:
    ErrorTable()=default;  // 私有构造函数
    ~ErrorTable()=default; // 私有析构函数
    std::vector<Error*> errors;
};


#endif //BUAA_COMPILER_ERRORTABLE_H

//
// Created by 安达楷 on 2023/11/1.
//

#ifndef BUAA_COMPILER_EXCEPTION_H
#define BUAA_COMPILER_EXCEPTION_H

#include <exception>

class MyError : public std::exception {
public:
    const char* what() const noexcept override {
        return "";  // 返回空字符串，不输出任何东西
    }
};

#endif //BUAA_COMPILER_EXCEPTION_H

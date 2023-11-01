//
// Created by 安达楷 on 2023/10/25.
//

#ifndef BUAA_COMPILER_DEBUG_H
#define BUAA_COMPILER_DEBUG_H

// 定义 DEBUG 宏

#define DEBUG

#include <stdio.h>

#ifdef DEBUG
// 定义 DEBUG_PRINT 宏，用于打印调试信息
#define DEBUG_PRINT(format, ...) \
        do { \
            printf(format, __VA_ARGS__); \
        } while(0)

#define DEBUG_PRINT_DIRECT(x) std::cout << x << std::endl
#else
// 如果没有定义 DEBUG 宏，则将 DEBUG_PRINT 定义为空
    #define DEBUG_PRINT(message)
    #define DEBUG_PRINT_DIRECT(message)
#endif

#endif //BUAA_COMPILER_DEBUG_H

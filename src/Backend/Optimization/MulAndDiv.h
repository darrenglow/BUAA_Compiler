//
// Created by 安达楷 on 2023/12/12.
//

#ifndef BUAA_COMPILER_MULANDDIV_H
#define BUAA_COMPILER_MULANDDIV_H

#include <vector>
#include "../Instruction.h"

class MulAndDiv {
    static int limit;
    static int divLimit;
    static int N;
public:
    static std::vector<Instruction*>* mul(RegType rd, RegType rt, int num);
    static std::vector<Instruction*>* div(RegType rd, RegType rt, int num, bool isDiv);
    static long long* chooseMultiplier(int d, int p);
};
#endif //BUAA_COMPILER_MULANDDIV_H

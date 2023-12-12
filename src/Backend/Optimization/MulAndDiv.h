//
// Created by 安达楷 on 2023/12/12.
//

#ifndef BUAA_COMPILER_MULANDDIV_H
#define BUAA_COMPILER_MULANDDIV_H

#include <vector>
#include "../Instruction.h"

class MulAndDiv {
    static int limit;

public:
    static std::vector<Instruction*>* mul(RegType rd, RegType rt, int num);
};
#endif //BUAA_COMPILER_MULANDDIV_H

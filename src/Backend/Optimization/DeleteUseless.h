//
// Created by 安达楷 on 2023/12/12.
//

#ifndef BUAA_COMPILER_DELETEUSELESS_H
#define BUAA_COMPILER_DELETEUSELESS_H

#include "../Instruction.h"
#include <vector>
class DeleteUseless {
public:
    std::vector<Instruction*> *instructions;

    explicit DeleteUseless(std::vector<Instruction*> *instructions_) : instructions(instructions_) {}

    void deleteUselessJump();
};

#endif //BUAA_COMPILER_DELETEUSELESS_H

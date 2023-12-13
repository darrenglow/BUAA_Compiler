//
// Created by 安达楷 on 2023/12/12.
//

#ifndef BUAA_COMPILER_PEEPHOLE_H
#define BUAA_COMPILER_PEEPHOLE_H

#include "../Instruction.h"
#include <vector>
class PeepHole {
public:
    std::vector<Instruction*> *instructions;

    explicit PeepHole(std::vector<Instruction*> *instructions_) : instructions(instructions_) {}

    void doPeepHole();
    void deleteUselessJump();
    void deleteUselessMove();
    void optimCompareAndSet();
    void optimImmediate();
    void deleteRepeatLwSw();
    void mergeMove();
    void optimSeq();
};

#endif //BUAA_COMPILER_PEEPHOLE_H

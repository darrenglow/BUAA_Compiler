//
// Created by 安达楷 on 2023/12/12.
//

#include "PeepHole.h"

void PeepHole::doPeepHole() {
    deleteUselessJump();
    deleteUselessMove();
    deleteRepeatLwSw();
    optimSeq();
    // 激进优化下考虑
    mergeMove();
    optimCompareAndSet();
    optimImmediate();
}

void PeepHole::deleteUselessJump() {
    auto toEraseSet = new std::set<Instruction*>();
    for (int i = 0; i < instructions->size(); i ++ ){
        auto x = (*instructions)[i];
        if (dynamic_cast<L*>(x) && dynamic_cast<L*>(x)->type == L::j) {
            auto target = dynamic_cast<L*>(x)->target;
            auto y = (*instructions)[i + 1];
            if (dynamic_cast<MipsLabel*>(y) != nullptr) {
                auto yLabel = dynamic_cast<MipsLabel*>(y)->label->label;
                if (target->label->label == yLabel) {
                    toEraseSet->insert(x);
                }
            }
        }
    }
    for (auto x : *toEraseSet) {
        auto it = std::find(instructions->begin(), instructions->end(), x);
        instructions->erase(it);
    }
}

void PeepHole::deleteUselessMove() {
    auto toEraseSet = new std::set<Instruction*>();
    for (int i = 0; i < instructions->size(); i ++ ){
        auto x = (*instructions)[i];
        if (dynamic_cast<RR*>(x) && dynamic_cast<RR*>(x)->type == RR::move) {
            auto reg1 = dynamic_cast<RR*>(x)->rs;
            auto reg2 = dynamic_cast<RR*>(x)->rt;
            if (reg1 == reg2) {
                toEraseSet->insert(x);
            }
        }
    }
    for (auto x : *toEraseSet) {
        auto it = std::find(instructions->begin(), instructions->end(), x);
        instructions->erase(it);
    }
}

void PeepHole::optimCompareAndSet() {
    auto toEraseSet = new std::set<Instruction*>();
    for (int i = 0; i < instructions->size(); i ++ ){
        auto x = (*instructions)[i];
        // sle -> slti
        if (dynamic_cast<RRR*>(x) != nullptr && dynamic_cast<RRR*>(x)->type == RRR::sle) {
            auto pre_x = (*instructions)[i - 1];
            if (dynamic_cast<RI*>(pre_x) != nullptr && dynamic_cast<RI*>(pre_x)->type == RI::li) {
                toEraseSet->insert(pre_x);
                (*instructions)[i] = new RRI(RRI::slti, dynamic_cast<RRR*>(x)->rd, dynamic_cast<RRR*>(x)->rs,
                                            dynamic_cast<RI*>(pre_x)->immediate + 1);
                free(x);
            }
        }
        // slt -> slti
        else if (dynamic_cast<RRR*>(x) != nullptr && dynamic_cast<RRR*>(x)->type == RRR::slt) {
            auto pre_x = (*instructions)[i - 1];
            if (dynamic_cast<RI*>(pre_x) != nullptr && dynamic_cast<RI*>(pre_x)->type == RI::li) {
                toEraseSet->insert(pre_x);
                (*instructions)[i] = new RRI(RRI::slti, dynamic_cast<RRR*>(x)->rd, dynamic_cast<RRR*>(x)->rs,
                                             dynamic_cast<RI*>(pre_x)->immediate);
                free(x);
            }
        }
        // sge -> sgt
        else if (dynamic_cast<RRR*>(x) != nullptr && dynamic_cast<RRR*>(x)->type == RRR::sge) {
            auto pre_x = (*instructions)[i - 1];
            if (dynamic_cast<RI*>(pre_x) != nullptr && dynamic_cast<RI*>(pre_x)->type == RI::li) {
                toEraseSet->insert(pre_x);
                (*instructions)[i] = new RRI(RRI::sgt, dynamic_cast<RRR*>(x)->rd, dynamic_cast<RRR*>(x)->rs,
                                             dynamic_cast<RI*>(pre_x)->immediate - 1);
                free(x);
            }
        }
    }
    for (auto x : *toEraseSet) {
        auto it = std::find(instructions->begin(), instructions->end(), x);
        instructions->erase(it);
    }
}

void PeepHole::optimImmediate() {
    auto toEraseSet = new std::set<Instruction*>();
    for (int i = 0; i < instructions->size(); i ++ ){
        auto x = (*instructions)[i];
        if (dynamic_cast<RI*>(x) && dynamic_cast<RI*>(x)->type == RI::li) {
            auto follow_x = (*instructions)[i + 1];
            // addu -> addiu
            if (dynamic_cast<RRR*>(follow_x) && dynamic_cast<RRR*>(follow_x)->type == RRR::addu) {
                auto liReg = dynamic_cast<RI*>(x)->reg;
                auto reg1 = dynamic_cast<RRR*>(follow_x)->rt;
                auto reg2 = dynamic_cast<RRR*>(follow_x)->rs;
                if (reg1 == liReg) {
                    toEraseSet->insert(x);
                    (*instructions)[i + 1] = new RRI(RRI::addiu, dynamic_cast<RRR*>(follow_x)->rd, reg2, dynamic_cast<RI*>(x)->immediate);
                }
                else if (reg2 == liReg) {
                    toEraseSet->insert(x);
                    (*instructions)[i + 1] = new RRI(RRI::addiu, dynamic_cast<RRR*>(follow_x)->rd, reg1, dynamic_cast<RI*>(x)->immediate);
                }
            }
            // subu -> subiu
            // 更慢了！！！
//            else if (dynamic_cast<RRR*>(follow_x) && dynamic_cast<RRR*>(follow_x)->type == RRR::subu) {
//                auto liReg = dynamic_cast<RI*>(x)->reg;
//                auto reg = dynamic_cast<RRR*>(follow_x)->rt;
//                if (reg == liReg) {
//                    toEraseSet->insert(x);
//                    (*instructions)[i + 1] = new RRI(RRI::subiu, dynamic_cast<RRR*>(follow_x)->rd, dynamic_cast<RRR*>(follow_x)->rs, dynamic_cast<RI*>(x)->immediate);
//                }
//            }
        }
    }
    for (auto x : *toEraseSet) {
        auto it = std::find(instructions->begin(), instructions->end(), x);
        instructions->erase(it);
    }
}

void PeepHole::deleteRepeatLwSw() {
    auto toEraseSet = new std::set<Instruction*>();
    for (int i = 0; i < instructions->size(); i ++ ){
        auto x = (*instructions)[i];
        if (dynamic_cast<M*>(x)) {
            auto follow_x = (*instructions)[i + 1];
            if (dynamic_cast<M*>(follow_x)) {
                auto reg1 = dynamic_cast<M*>(x)->rd;
                auto reg2 = dynamic_cast<M*>(x)->offset;
                auto reg3 = dynamic_cast<M*>(x)->rs;
                auto reg1_ = dynamic_cast<M*>(follow_x)->rd;
                auto reg2_ = dynamic_cast<M*>(follow_x)->offset;
                auto reg3_ = dynamic_cast<M*>(follow_x)->rs;
                if (reg1 == reg1_ && reg2 == reg2_ && reg3 == reg3_) {
                    toEraseSet->insert(x);
                }
            }
        }
    }
    for (auto x : *toEraseSet) {
        auto it = std::find(instructions->begin(), instructions->end(), x);
        instructions->erase(it);
    }
}

void PeepHole::mergeMove() {
    auto toEraseSet = new std::set<Instruction*>();
    for (int i = 0; i < instructions->size(); i ++ ){
        auto x = (*instructions)[i];
        if (dynamic_cast<RR*>(x) && dynamic_cast<RR*>(x)->type == RR::move) {
            auto pre_x = (*instructions)[i - 1];
            if (dynamic_cast<RRI*>(pre_x) && dynamic_cast<RR*>(x)->rt == dynamic_cast<RRI*>(pre_x)->rd) {
                toEraseSet->insert(x);
                (*instructions)[i - 1] = new RRI(dynamic_cast<RRI*>(pre_x)->type, dynamic_cast<RR*>(x)->rs,
                                                 dynamic_cast<RRI*>(pre_x)->rs, dynamic_cast<RRI*>(pre_x)->immediate);
            }
            else if (dynamic_cast<RRR*>(pre_x) && dynamic_cast<RR*>(x)->rt == dynamic_cast<RRR*>(pre_x)->rd) {
                toEraseSet->insert(x);
                (*instructions)[i - 1] = new RRR(dynamic_cast<RRR*>(pre_x)->type, dynamic_cast<RR*>(x)->rs,
                                                 dynamic_cast<RRR*>(pre_x)->rs, dynamic_cast<RRR*>(pre_x)->rt);
            }
        }
    }
    for (auto x : *toEraseSet) {
        auto it = std::find(instructions->begin(), instructions->end(), x);
        instructions->erase(it);
    }
}

void PeepHole::optimSeq() {
    auto toEraseSet = new std::set<Instruction*>();
    for (int i = 0; i < instructions->size(); i ++ ){
        auto x = (*instructions)[i];
        if (dynamic_cast<RRR*>(x) && dynamic_cast<RRR*>(x)->type == RRR::seq) {
            auto follow_x = (*instructions)[i + 1];
            if (dynamic_cast<RL*>(follow_x) && dynamic_cast<RL*>(follow_x)->type == RL::beqz &&
                    dynamic_cast<RRR*>(x)->rd == dynamic_cast<RL*>(follow_x)->rd) {
                toEraseSet->insert(x);
                (*instructions)[i + 1] = new RRL(RRL::bne, dynamic_cast<RRR*>(x)->rs, dynamic_cast<RRR*>(x)->rt,
                                                 dynamic_cast<RL*>(follow_x)->target);
            }
        }
        else if (dynamic_cast<RRR*>(x) && dynamic_cast<RRR*>(x)->type == RRR::sne) {
            auto follow_x = (*instructions)[i + 1];
            if (dynamic_cast<RL*>(follow_x) && dynamic_cast<RL*>(follow_x)->type == RL::beqz &&
                dynamic_cast<RRR*>(x)->rd == dynamic_cast<RL*>(follow_x)->rd) {
                toEraseSet->insert(x);
                (*instructions)[i + 1] = new RRL(RRL::beq, dynamic_cast<RRR*>(x)->rs, dynamic_cast<RRR*>(x)->rt,
                                                 dynamic_cast<RL*>(follow_x)->target);
            }
        }
    }
    for (auto x : *toEraseSet) {
        auto it = std::find(instructions->begin(), instructions->end(), x);
        instructions->erase(it);
    }
}


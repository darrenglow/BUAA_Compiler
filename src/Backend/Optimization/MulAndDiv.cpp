//
// Created by 安达楷 on 2023/12/12.
//

#include "MulAndDiv.h"

int MulAndDiv::limit = 5;
std::vector<Instruction*>* MulAndDiv::mul(RegType rd, RegType rt, int num) {
    auto v = new std::vector<Instruction*>();
    bool neg = num < 0;
    int prevNum = num;
    num = num > 0 ? num : -num;
    int cnt = 0;
    bool first = true;
    while (num != 0) {
        if (num % 2 == 1) {
            if (first) {
                auto code = new RRI(RRI::sll, rd, rt, cnt);
                v->push_back(code);
                first = false;
            }
            else {
                v->push_back(new RRI(RRI::sll, $a1, rt, cnt));
                v->push_back(new RRR(RRR::addu, rd, rd, $a1));
            }
        }
        num >>= 1;
        cnt += 1;
    }
    if (neg) {
        v->push_back(new RR(RR::neg, rd, rd));
    }
    if (limit < v->size()) {
        v->clear();
        v->push_back(new RRR(RRR::mul, rd, rt, prevNum));
    }
    return v;
}
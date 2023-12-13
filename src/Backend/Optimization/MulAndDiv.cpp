//
// Created by 安达楷 on 2023/12/12.
//

#include "MulAndDiv.h"
#include <cmath>

int MulAndDiv::limit = 4;
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
                v->push_back(new RRI(RRI::sll, $a0, rt, cnt));
                v->push_back(new RRR(RRR::addu, rd, rd, $a0));
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

int MulAndDiv::divLimit = 20;
int MulAndDiv::N = 32;

long long* MulAndDiv::chooseMultiplier(int d, int p) {
    std::cout << "d:" << d << " p:" << p << std::endl;
    long long l = (long long)ceil(log(d) / log(2));
    long long sh = l;
    long long low = (long long)floor(pow(2, N + l) / d);
    long long high = (long long)floor((pow(2, N + l) + pow(2, N + l - p)) / d);
    printf("%lld %lld %lld %lld\n", l, sh, low, high);
    while ((low >> 1ll) < (high >> 1ll) && sh > 0) {
        low >>= 1ll;
        high >>= 1ll;
        sh -= 1;
        printf("high: %lld\n", high);
    }
    long long* result = new long long[3];
    result[0] = high;
    result[1] = sh;
    result[2] = l;
    printf("multiplier: m: %lld sh: %lld l: %lld\n", high, sh, l);
    return result;
}

std::vector<Instruction *> * MulAndDiv::div(RegType rd, RegType rt, int d, bool isDiv) {
    auto v = new std::vector<Instruction*>();
    auto multiplier = chooseMultiplier(abs(d), N - 1);
    auto m = multiplier[0];
    auto sh = multiplier[1];
    auto l = multiplier[2];
    printf("multiplier: m: %lld sh: %lld l: %lld\n", m, sh, l);
    if (abs(d) == 1) {
        v->push_back(new RR(RR::move, rd, rt));
    }
    else if (abs(d) == (1ll << l)) {
        v->push_back(new RRI(RRI::sra, $a0, rt, (int)l - 1));
        v->push_back(new RRI(RRI::srl, $a0, $a0, (int)(N - l)));
        v->push_back(new RRR(RRR::addu, $a0, $a0, rt));
        v->push_back(new RRI(RRI::sra, rd, $a0, (int)l));
    }
    else if (m < (1ll << (N - 1))) {
        v->push_back(new RI(RI::li, $a0, (int)m));
        v->push_back(new RR(RR::mult, $a0, rt));
        v->push_back(new R(R::mfhi, $a0));
        v->push_back(new RRI(RRI::sra, rd, $a0, (int)sh));
        v->push_back(new RRI(RRI::sra, $a0, rt, 31));
        v->push_back(new RRR(RRR::subu, rd, rd, $a0));
    }
    else {
        v->push_back(new RI(RI::li, $a0, (int)(m - (int)(1l << N))));
        v->push_back(new RR(RR::mult, $a0, rt));
        v->push_back(new R(R::mfhi, $a0));
        v->push_back(new RRR(RRR::addu, $a0, $a0, rt));
        v->push_back(new RRI(RRI::sra, rd, $a0, (int)sh));
        v->push_back(new RRI(RRI::sra, $a0, rt, 31));
        v->push_back(new RRR(RRR::subu, rd, rd, $a0));
    }
    if (d < 0) {
        v->push_back(new RR(RR::neg, rd, rd));
    }
    if (!isDiv) {
        auto v2 = mul($v0, rd, d);
        for (auto x : *v2) {
            v->push_back(x);
        }
        v->push_back(new RRR(RRR::subu, rd, rt, $v0));
    }
    return v;
}
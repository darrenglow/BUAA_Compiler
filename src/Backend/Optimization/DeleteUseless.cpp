//
// Created by 安达楷 on 2023/12/12.
//

#include "DeleteUseless.h"

void DeleteUseless::deleteUselessJump() {
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
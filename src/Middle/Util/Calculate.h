//
// Created by 安达楷 on 2023/10/25.
//

#ifndef BUAA_COMPILER_CALCULATE_H
#define BUAA_COMPILER_CALCULATE_H

#include "../../Fronted/Parser/AST.h"


class Calculate {
public:
    static int calcConstExp(ConstExp *constExp);

    static int calcExp(Exp *exp);

    static int calcAddExp(AddExp *addExp);

    static int calcMulExp(MulExp *mulExp);

    static int calcUnaryExp(UnaryExp *unaryExp);

    static int calcLVal(LVal *lVal);
};

#endif //BUAA_COMPILER_CALCULATE_H

//
// Created by 安达楷 on 2023/10/25.
//

#include "Calculate.h"
#include "../Visitor/Visitor.h"
#include "cassert"
#include "../../Util/Debug.h"


int Calculate::calcExp(Exp *exp) {
    return calcAddExp(exp->addExp);
}

int Calculate::calcConstExp(ConstExp *constExp) {
    return calcAddExp(constExp->addExp);
}

int Calculate::calcAddExp(AddExp *addExp) {
    auto mulExp = addExp->mulExps[0];
    int res = calcMulExp(mulExp);
    for (int i = 1; i < addExp->mulExps.size(); i ++ ) {
        auto op = addExp->ops[i - 1];
        auto mulExp2 = addExp->mulExps[i];
        int x2 = calcMulExp(mulExp2);

        switch (op->tokenType) {
            case Token::PLUS:
                res += x2;
                break;
            case Token::MINU:
                res -= x2;
                break;
            default:
                std::cout << "Error in calcAddExp" << std::endl;
        }
    }
    return res;
}

int Calculate::calcMulExp(MulExp *mulExp) {
    auto unaryExp = mulExp->unaryExps[0];
    int res = calcUnaryExp(unaryExp);
    for (int i = 1; i < mulExp->unaryExps.size(); i ++ ) {
        auto op = mulExp->ops[i - 1];
        auto unaryExp2 = mulExp->unaryExps[i];
        int x2 = calcUnaryExp(unaryExp2);

        switch (op->tokenType) {
            case Token::MULT:
                res *= x2;
                break;
            case Token::DIV:
                res /= x2;
                break;
            case Token::MOD:
                res %= x2;
                break;
            default:
                std::cout << "Error in calcMulExp" << std::endl;
        }
    }
    return res;
}

// 这里不考虑函数调用，只考虑表达式
int Calculate::calcUnaryExp(UnaryExp *unaryExp) {
    auto primaryExp = unaryExp->primaryExp;
    auto unaryOp = unaryExp->unaryOp;
    int res = 0;

    if (primaryExp != nullptr) {
        auto number = primaryExp->number;
        auto lVal = primaryExp->lVal;
        auto exp = primaryExp->exp;
        if (number != nullptr)
            res = number->intConst->val;
        else if (lVal != nullptr)
            res = calcLVal(lVal);
        else if (exp != nullptr)
            res = calcExp(exp);
        else
            std::cout << "Error in calcUnaryExp" << std::endl;
    }
    else if (unaryOp != nullptr) {
        switch (unaryOp->unaryOp->tokenType) {
            case Token::PLUS:
                res = calcUnaryExp(unaryExp->unaryExp);
                break;
            case Token::MINU:
                res = -calcUnaryExp(unaryExp->unaryExp);
                break;
            case Token::NOT:
                res = (calcUnaryExp(unaryExp->unaryExp) == 0);
                break;
            default:
                std::cout << "Error in calcUnaryExp" << std::endl;
        }
    }
    return res;
}

// LVal → Ident {'[' Exp ']'} //1.普通变量 2.一维数组 3.二维数组
int Calculate::calcLVal(LVal *lVal) {
    auto ident = lVal->ident;
    auto name = ident->content;
    auto symbol = Visitor::curTable->getSymbol(name, true);
    assert(symbol != nullptr);
    auto valueSymbol = dynamic_cast<ValueSymbol*>(symbol);
    assert(valueSymbol != nullptr);
    // TODO: 变量的值，和从数组中取值。
    return 0;
}







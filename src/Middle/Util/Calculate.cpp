//
// Created by 安达楷 on 2023/10/25.
//

#include "Calculate.h"
#include "../Visitor/Visitor.h"
#include "cassert"
#include "../../Util/Debug.h"
#include "../../Util/Exception.h"


int Calculate::calcExp(Exp *exp){
    try {
        return calcAddExp(exp->addExp);
    } catch(MyError&) {throw;};

}

int Calculate::calcConstExp(ConstExp *constExp) {
    try {
        return calcAddExp(constExp->addExp);
    } catch(MyError&) {throw;};
}

int Calculate::calcAddExp(AddExp *addExp) {
    try {
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
    }catch (MyError&) {throw;}
}

int Calculate::calcMulExp(MulExp *mulExp) {
    try {
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
    }catch (MyError&){throw;}
}

// 这里不考虑函数调用，只考虑表达式
int Calculate::calcUnaryExp(UnaryExp *unaryExp) {
    auto primaryExp = unaryExp->primaryExp;
    auto unaryOp = unaryExp->unaryOp;
    auto ident = unaryExp->ident;   // 函数调用
    int res = 0;

    if (primaryExp != nullptr) {
        auto number = primaryExp->number;
        auto lVal = primaryExp->lVal;
        auto exp = primaryExp->exp;
        if (number != nullptr)
            res = number->intConst->val;
        else if (lVal != nullptr)
            try {
                res = calcLVal(lVal);
            }catch(MyError&){throw;}
        else if (exp != nullptr)
            try {
                res = calcExp(exp);
            }catch(MyError&){throw;}
        else
            throw MyError();
        return res;
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
        return res;
    }
    else{
        throw MyError();
    }
}

// LVal → Ident {'[' Exp ']'} //1.普通变量 2.一维数组 3.二维数组
int Calculate::calcLVal(LVal *lVal) {
    auto ident = lVal->ident;
    auto name = ident->content;
    auto symbol = Visitor::curTable->getSymbol(name, true);
    assert(symbol != nullptr);
    auto valueSymbol = dynamic_cast<ValueSymbol*>(symbol);
    assert(valueSymbol != nullptr);

    if (!valueSymbol->isConst())
        throw MyError();

        // TODO: 变量的值，和从数组中取值。
    if (valueSymbol->valueType == ValueType::SINGLE) {
        return valueSymbol->initValue;
    }
    else if (valueSymbol->valueType == ValueType::ARRAY) {
        if (valueSymbol->dims.size() == 1) {    // 一维
            int x = calcExp(lVal->exps[0]);
            std::cout << "x" << x << " " << valueSymbol->initValues[x] << std::endl;
            return valueSymbol->initValues[x];
        }
        else if (valueSymbol->dims.size() == 2) {
            int x1 = calcExp(lVal->exps[0]);
            int x2 = calcExp(lVal->exps[1]);
            return valueSymbol->initValues[x1 * valueSymbol->dims[1] + x2];
        }
    }
    return -1;
}







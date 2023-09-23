//
// Created by 安达楷 on 2023/9/13.
//
#include "Token.h"
#include <iostream>

//与tokenType一一对应
std::string Token::type2str[50] = {
        "IDENFR", "INTCON", "STRCON", "MAINTK", "CONSTTK", "INTTK", "BREAKTK",
        "CONTINUETK", "IFTK", "ELSETK", "NOT", "AND", "OR", "FORTK", "GETINTTK",
        "PRINTFTK", "RETURNTK", "PLUS", "MINU", "VOIDTK", "MULT", "DIV", "MOD", "LSS",
        "LEQ", "GRE", "GEQ", "EQL", "NEQ", "ASSIGN", "SEMICN", "COMMA", "LPARENT", "RPARENT",
        "LBRACK", "RBRACK", "LBRACE", "RBRACE", "UNDEFINE"
//        "Ident", "IntConst", "FormatString", "main", "const", "int", "break",
//        "continue", "if", "else", "!", "&&", "||", "while", "getint",
//        "printf", "return", "+", "-", "void", "*", "/", "%", "<",
//        "<=", ">", ">=", "==", "!=", "=", ";", ",", "(", ")",
//        "[", "]", "{", "}"
};

std::ostream& operator<<(std::ostream &ostream, const Token &token) {
    ostream << Token::type2str[token.tokenType] << " " << token.content;
    return ostream;
}

//这个TokenType的<<重载主要是为了打印parser的输出结果
//其中IDENFR, STRCON, INTCON涉及到具体的值，会由Token的<<重载输出
std::ostream& operator<<(std::ostream &ostream, const Token::TokenType &type) {
    ostream << Token::type2str[type] << " ";
    switch(type) {
        case Token::COMMA:
            ostream << ",";
            break;
        case Token::IDENFR:
            break;
        case Token::INTCON:
            break;
        case Token::STRCON:
            break;
        case Token::MAINTK:
            ostream << "main";
            break;
        case Token::CONSTTK:
            ostream << "const";
            break;
        case Token::INTTK:
            ostream << "int";
            break;
        case Token::BREAKTK:
            ostream << "break";
            break;
        case Token::CONTINUETK:
            ostream << "continue";
            break;
        case Token::IFTK:
            ostream << "if";
            break;
        case Token::ELSETK:
            ostream << "else";
            break;
        case Token::NOT:
            ostream << "!";
            break;
        case Token::AND:
            ostream << "&&";
            break;
        case Token::OR:
            ostream << "||";
            break;
        case Token::FORTK:
            ostream << "for";
            break;
        case Token::GETINTTK:
            ostream << "getint";
            break;
        case Token::PRINTFTK:
            ostream << "printf";
            break;
        case Token::RETURNTK:
            ostream << "return";
            break;
        case Token::PLUS:
            ostream << "+";
            break;
        case Token::MINU:
            ostream << "-";
            break;
        case Token::VOIDTK:
            ostream << "void";
            break;
        case Token::MULT:
            ostream << "*";
            break;
        case Token::DIV:
            ostream << "/";
            break;
        case Token::MOD:
            ostream << "%";
            break;
        case Token::LSS:
            ostream << "<";
            break;
        case Token::LEQ:
            ostream << "<=";
            break;
        case Token::GRE:
            ostream << ">";
            break;
        case Token::GEQ:
            ostream << ">=";
            break;
        case Token::EQL:
            ostream << "==";
            break;
        case Token::NEQ:
            ostream << "!=";
            break;
        case Token::ASSIGN:
            ostream << "=";
            break;
        case Token::SEMICN:
            ostream << ";";
            break;
        case Token::LPARENT:
            ostream << "(";
            break;
        case Token::RPARENT:
            ostream << ")";
            break;
        case Token::LBRACK:
            ostream << "[";
            break;
        case Token::RBRACK:
            ostream << "]";
            break;
        case Token::LBRACE:
            ostream << "{";
            break;
        case Token::RBRACE:
            ostream << "}";
            break;
        case Token::UNDEFINE:
            break;
    }
    return ostream;
}

std::vector<Token *> TokenList::getTokens() {
    return tokens;
}
bool TokenList::hasNext() {
    return pos < tokens.size();
}
void TokenList::addToken(Token *token) {
    tokens.push_back(token);
}
Token * TokenList::get() {
    return tokens[pos];
}
Token * TokenList::getNext(int i) {
    return tokens[pos + i];
}
Token* TokenList::pop() {
    return tokens[pos ++ ];
}
Token* TokenList::popExpect(Token::TokenType type) {
    Token* token = tokens[pos];
    if (token->tokenType == type) {
        pos ++ ;
        return token;
    }
    std::cout << "Error in Line " << token->line << "Expect Token is {" << Token::type2str[type]
    << "} but get Token is {" << *token << "}" << std::endl;
    exit(1);
}

void TokenList::backTo(int x) {
    pos = x;
}

int TokenList::getPos() {
    return pos;
}



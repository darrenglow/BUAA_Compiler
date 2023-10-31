//
// Created by 安达楷 on 2023/9/13.
//
#include <fstream>
#include <iostream>
#include "Lexer.h"
#include "../Error/Error.h"
#include "../Error/ErrorTable.h"


extern std::ifstream input;

std::map<std::string, Token::TokenType> Lexer::str2type = {
        {"main"    , Token::MAINTK    },
        {"const"   , Token::CONSTTK   },
        {"int"     , Token::INTTK     },
        {"break"   , Token::BREAKTK   },
        {"continue", Token::CONTINUETK},
        {"if"      , Token::IFTK      },
        {"else"    , Token::ELSETK    },
        {"for"   , Token::FORTK   },
        {"getint"  , Token::GETINTTK  },
        {"printf"  , Token::PRINTFTK  },
        {"return"  , Token::RETURNTK  },
        {"void"    , Token::VOIDTK    },
        {"+"  , Token::PLUS    },
        {"-"  , Token::MINU    },
        {"*"  , Token::MULT    },
        {"/"  , Token::DIV     },
        {"%"  , Token::MOD     },
        {";"  , Token::SEMICN  },
        {","  , Token::COMMA   },
        {"("  , Token::LPARENT},
        {")"  , Token::RPARENT},
        {"["  , Token::LBRACK },
        {"]"  , Token::RBRACK },
        {"{"  , Token::LBRACE },
        {"}"  , Token::RBRACE },
        {"&&" , Token::AND     },
        {"||" , Token::OR      },
        {"<"  , Token::LSS     },
        {"<=" , Token::LEQ     },
        {">"  , Token::GRE     },
        {">=" , Token::GEQ     },
        {"="  , Token::ASSIGN  },
        {"==" , Token::EQL     },
        {"!"  , Token::NOT     },
        {"!=" , Token::NEQ     }
};

char Lexer::getChar() {
    char c = '\0';
    if (pos < fileStr.size())
        c = fileStr[pos ++ ];
    if (c == '\n')
        ++ curLine;
    return c;
}

void Lexer::lex() {
    ch = getChar();
    while (ch != '\0') {
        nextToken();
    }
}


void Lexer::nextToken() {
    std::string tokenStr;

    if (ch == '\0'){
        return;
    }
    if (ch == '\n' || ch == '\r' || ch == ' ' || ch == '\t') {
        ch = getChar();
        return;
    }

    if (isalpha(ch) || ch == '_') {
        int line = curLine;
        std::string str;
        while (isalpha(ch) || isdigit(ch) || ch == '_') {
            str.push_back(ch);
            ch = getChar();
        }
        auto iter = str2type.find(str);
        tokenList.addToken(new Token(iter == str2type.end() ? Token::IDENFR : iter->second, str, line));
    }
    else if (isdigit(ch)) {
        int line = curLine;
        std::string str;
        while (isdigit(ch)){
            str.push_back(ch);
            ch = getChar();
        }
        tokenList.addToken(new Token(Token::INTCON, str, line, atoi(str.c_str())));
    }
    else if (std::string("()[]{},;%*-+").find(ch) != std::string::npos) {
        int line = curLine;
        tokenStr.push_back(ch);
        tokenList.addToken(new Token(str2type.find(tokenStr)->second, tokenStr, line));
        ch = getChar();
    }
    else if (std::string("<>!=").find(ch) != std::string::npos) {
        int line = curLine;
        tokenStr.push_back(ch);
        ch = getChar();
        if (ch == '=') {
            tokenStr.push_back(ch);
            ch = getChar();
        }
        tokenList.addToken(new Token(str2type.find(tokenStr)->second, tokenStr, line));
    }
    else if (ch == '|') {
        int line = curLine;
        ch = getChar();
        if (ch == '|')
            tokenList.addToken(new Token(str2type.find("||")->second, "||", line));
        ch = getChar();
    }
    else if (ch == '&') {
        int line = curLine;
        ch = getChar();
        if (ch == '&')
            tokenList.addToken(new Token(str2type.find("&&")->second, "&&", line));
        ch = getChar();
    }
    else if (ch == '/') {
        int line = curLine;
        ch = getChar();
        if (ch == '/' || ch == '*') {
            if (ch == '/') {
                while (ch != '\n' && ch != '\0')
                    ch = getChar();
            }
            else if (ch == '*') {
                char preCh = '\0';
                ch = getChar();
                while (!(preCh == '*' && ch == '/')) {
                    preCh = ch;
                    ch = getChar();
                }
            }
            ch = getChar();
        }
        else
            tokenList.addToken(new Token(Token::DIV, "/", line));
    }

    else if (ch == '"') {
        int line = curLine;
        std::string str("\"");
        ch = getChar();
        while (ch != '"') {
            str.push_back(ch);
            ch = getChar();
        }
        str.push_back('"');

        for (int i = 1; i < str.size() - 1; ++ i) {
            char c = str[i];
            bool x = (c == 32 || c == 33 || (40 <= c && c <= 126 && c != 92));
            bool y = (c == 92 && i + 1 < str.size() - 1 && str[i + 1] == 'n');
            bool z = (c == '%' && i + 1 < str.size() - 1 && str[i + 1] == 'd');
            if (!x && !y && !z) {
                ErrorTable::getInstance().addError(new Error(Error::ILLEGAL_FORMATSTRING, line));
                break;
            }
        }

        tokenList.addToken(new Token(Token::STRCON, str, line));
        ch = getChar();
    }

    else {
        int line = curLine;
        tokenList.addToken(new Token(Token::UNDEFINE, "", line));
        ch = getChar();
    }
}


extern std::ofstream output;
void Lexer::printTokens() {
    for (auto token : tokenList.getTokens()) {
        output << Token::type2str[token->tokenType] << " " << token->content << std::endl;
    }
}

TokenList& Lexer::getTokens() {
    return tokenList;
}
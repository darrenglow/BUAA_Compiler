//
// Created by 安达楷 on 2023/9/13.
//
#include "Lexer.h"
#include <fstream>
#include <iostream>
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
        std::string str;
        while (isalpha(ch) || isdigit(ch) || ch == '_') {
            str.push_back(ch);
            ch = getChar();
        }
        auto iter = str2type.find(str);
        tokens.addToken(new Token(iter == str2type.end() ? Token::IDENFR : iter->second, str, curLine));
    }
    else if (isdigit(ch)) {
        std::string str;
        while (isdigit(ch)){
            str.push_back(ch);
            ch = getChar();
        }
        tokens.addToken(new Token(Token::INTCON, str, curLine, atoi(str.c_str())));
    }
    else if (std::string("()[]{},;%*-+").find(ch) != std::string::npos) {
        tokenStr.push_back(ch);
        tokens.addToken(new Token(str2type.find(tokenStr)->second, tokenStr, curLine));
        ch = getChar();
    }
    else if (std::string("<>!=").find(ch) != std::string::npos) {
        tokenStr.push_back(ch);
        ch = getChar();
        if (ch == '=') {
            tokenStr.push_back(ch);
            ch = getChar();
        }
        tokens.addToken(new Token(str2type.find(tokenStr)->second, tokenStr, curLine));
    }
    else if (ch == '|') {
        ch = getChar();
        if (ch == '|')
            tokens.addToken(new Token(str2type.find("||")->second, "||", curLine));
        ch = getChar();
    }
    else if (ch == '&') {
        ch = getChar();
        if (ch == '&')
            tokens.addToken(new Token(str2type.find("&&")->second, "&&", curLine));
        ch = getChar();
    }
    else if (ch == '/') {
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
            tokens.addToken(new Token(Token::DIV, "/", curLine));
    }

    else if (ch == '"') {
        std::string str("\"");
        ch = getChar();
        while (ch != '"') {
            str.push_back(ch);
            ch = getChar();
        }
        str.push_back('"');
        tokens.addToken(new Token(Token::STRCON, str, curLine));
        ch = getChar();
    }

    else {
        tokens.addToken(new Token(Token::UNDEFINE, "", curLine));
        ch = getChar();
    }
}


extern std::ofstream output;
void Lexer::printTokens() const {
    output << tokens;
}
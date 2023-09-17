//
// Created by 安达楷 on 2023/9/13.
//
#include "Lexer.h"
#include <fstream>
#include <iostream>
extern std::ifstream input;

std::map<std::string, Token::TokenType> Lexer::str2type = {
        {"main"    , Token::TokenType::MAINTK    },
        {"const"   , Token::TokenType::CONSTTK   },
        {"int"     , Token::TokenType::INTTK     },
        {"break"   , Token::TokenType::BREAKTK   },
        {"continue", Token::TokenType::CONTINUETK},
        {"if"      , Token::TokenType::IFTK      },
        {"else"    , Token::TokenType::ELSETK    },
        {"while"   , Token::TokenType::WHILETK   },
        {"getint"  , Token::TokenType::GETINTTK  },
        {"printf"  , Token::TokenType::PRINTFTK  },
        {"return"  , Token::TokenType::RETURNTK  },
        {"void"    , Token::TokenType::VOIDTK    },
        {"+"  , Token::TokenType::PLUS    },
        {"-"  , Token::TokenType::MINU    },
        {"*"  , Token::TokenType::MULT    },
        {"/"  , Token::TokenType::DIV     },
        {"%"  , Token::TokenType::MOD     },
        {";"  , Token::TokenType::SEMICN  },
        {","  , Token::TokenType::COMMA   },
        {"("  , Token::TokenType::LPARENT},
        {")"  , Token::TokenType::RPARENT},
        {"["  , Token::TokenType::LBRACK },
        {"]"  , Token::TokenType::RBRACK },
        {"{"  , Token::TokenType::LBRACE },
        {"}"  , Token::TokenType::RBRACE },
        {"&&" , Token::TokenType::AND     },
        {"||" , Token::TokenType::OR      },
        {"<"  , Token::TokenType::LSS     },
        {"<=" , Token::TokenType::LEQ     },
        {">"  , Token::TokenType::GRE     },
        {">=" , Token::TokenType::GEQ     },
        {"="  , Token::TokenType::ASSIGN  },
        {"==" , Token::TokenType::EQL     },
        {"!"  , Token::TokenType::NOT     },
        {"!=" , Token::TokenType::NEQ     }
};

std::vector<Token*> Lexer::getTokens() const {
    return tokens;
}

std::string Lexer::getFileStr() const {
    return fileStr;
}

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
    if (ch == '\n' || ch == '\r' || ch == ' ' || ch == '\r') {
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
        tokens.push_back(new Token(iter == str2type.end() ? Token::TokenType::IDENFR : iter->second, str, curLine));
    }
    else if (isdigit(ch)) {
        std::string str;
        while (isdigit(ch)){
            str.push_back(ch);
            ch = getChar();
        }
        tokens.push_back(new Token(Token::TokenType::INTCON, str, curLine, atoi(str.c_str())));
    }
    else if (std::string("()[]{},;%*-+").find(ch) != std::string::npos) {
        tokenStr.push_back(ch);
        tokens.push_back(new Token(str2type.find(tokenStr)->second, tokenStr, curLine));
        ch = getChar();
    }
    else if (std::string("<>!=").find(ch) != std::string::npos) {
        tokenStr.push_back(ch);
        ch = getChar();
        if (ch == '=') {
            tokenStr.push_back(ch);
            ch = getChar();
        }
        tokens.push_back(new Token(str2type.find(tokenStr)->second, tokenStr, curLine));
    }
    else if (ch == '|') {
        ch = getChar();
        if (ch == '|')
            tokens.push_back(new Token(str2type.find("||")->second, "||", curLine));
        ch = getChar();
    }
    else if (ch == '&') {
        ch = getChar();
        if (ch == '&')
            tokens.push_back(new Token(str2type.find("&&")->second, "&&", curLine));
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
            tokens.push_back(new Token(Token::TokenType::DIV, "/", curLine));
    }

    else if (ch == '"') {
        std::string str("\"");
        ch = getChar();
        while (ch != '"') {
            str.push_back(ch);
            ch = getChar();
        }
        str.push_back('"');
        tokens.push_back(new Token(Token::TokenType::STRCON, str, curLine));
        ch = getChar();
    }

    else {
        tokens.push_back(new Token(Token::TokenType::UNDEFINE, "", curLine));
        ch = getChar();
    }
}


extern std::ofstream output;
void Lexer::printTokens() const {
    for (auto& token : tokens) {
        output << *token << std::endl;
    }
}
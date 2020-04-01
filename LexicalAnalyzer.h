//
// Created by 黄继辉 on 2019-09-29.
//

#ifndef GRAMMER_ANALYSE_LEXICALANALYZER_H
#define GRAMMER_ANALYSE_LEXICALANALYZER_H
#define MAXTXTLENGTH 1000

void catToken();

void retract();

int reserve();

void getsym();

void clearToken();

int getChar();

#define IDENFR 0
#define INTCON 1
#define CHARCON 2
#define STRCON 3
#define CONSTTK 4
#define INTTK 5
#define CHARTK 6
#define VOIDTK 7
#define MAINTK 8
#define IFTK 9
#define ELSETK 10
#define DOTK 11
#define WHILETK 12
#define FORTK 13
#define SCANFTK 14
#define PRINTFTK 15
#define RETURNTK 16
#define PLUS 17
#define MINU 18
#define MULT 19
#define DIV 20
#define LSS 21
#define LEQ 22
#define GRE 23
#define GEQ 24
#define EQL 25
#define NEQ 26
#define ASSIGN 27
#define SEMICN 28
#define COMMA 29
#define LPARENT 30
#define RPARENT 31
#define LBRACK 32
#define RBRACK 33
#define LBRACE 34
#define RBRACE 35

static char num2typename[40][20] = {
        "IDENFR",
        "INTCON",
        "CHARCON",
        "STRCON",
        "CONSTTK",
        "INTTK",
        "CHARTK",
        "VOIDTK",
        "MAINTK",
        "IFTK",
        "ELSETK",
        "DOTK",
        "WHILETK",
        "FORTK",
        "SCANFTK",
        "PRINTFTK",
        "RETURNTK",
        "PLUS",
        "MINU",
        "MULT",
        "DIV",
        "LSS",
        "LEQ",
        "GRE",
        "GEQ",
        "EQL",
        "NEQ",
        "ASSIGN",
        "SEMICN",
        "COMMA",
        "LPARENT",
        "RPARENT",
        "LBRACK",
        "RBRACK",
        "LBRACE",
        "RBRACE"
};

int isSpace();

int isLetter();

int isDigit();

int isDouble();

int isEqu();

int isStringToken();

int isStringLetter();

int isCharToken();

int isCharLetter();

void writeSymbolType();
#endif //GRAMMER_ANALYSE_LEXICALANALYZER_H

//
// Created by 黄继辉 on 2019-09-29.
//

#include <cstring>
#include <cctype>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include "LexicalAnalyzer.h"
#include "Printer.h"
#include "ErrorHandle.h"

using namespace std;
extern int symbol;
extern int inputtxt_top;
extern int token_top;
extern char tempchar;
extern char token[MAXTXTLENGTH];
extern int inputlength;
extern char inputtxt[MAXTXTLENGTH];
extern ofstream outfile;

int countLine = 1;
char num2name[MAXTXTLENGTH][20] = {
        "IDENFR",
        "INTCON",
        "CHARCON",
        "STRCON",
        "const",
        "int",
        "char",
        "void",
        "main",
        "if",
        "else",
        "do",
        "while",
        "for",
        "scanf",
        "printf",
        "return",
        "+",
        "-",
        "*",
        "/",
        "<",
        "<=",
        ">",
        ">=",
        "==",
        "!=",
        "=",
        ";",
        ",",
        "(",
        ")",
        "[",
        "]",
        "{",
        "}"
};

void getsym() {
    inputToken();
    getChar();
    clearToken();
    while (isSpace()) {
        getChar();
    }
    if (isLetter()) {
        while (isLetter() | isDigit()) {
            catToken();
            getChar();
        }
        retract();
        symbol = reserve();
        return;
    }
    else if(tempchar == '0') {
        getChar();
        if (!isDigit()) {
            catToken();
            //num = transNum(token);
            retract();
            symbol = INTCON;
            return;
        }
        while (isDigit()) {
            getChar();
        }
        retract();
        symbol = INTCON;
        checkError(ERROR_TYPE_A, 0);
        return;
    }
    else if(tempchar != '0' && isDigit()) {
        while (isDigit()) {
            catToken();
            getChar();
        }
        retract();
        //num = transNum(token);
        symbol = INTCON;
        return;
    }
    else if (isDouble()) {
        catToken();
        getChar();
        if (isEqu()) {
            catToken();
            symbol = reserve();
        }
        else {
            retract();
            symbol = reserve();
        }
        return;
    }
    else if (isStringToken()) {
        getChar();
        while (!isStringToken()) {
            if (isStringLetter()) {
                catToken();
                getChar();
            }
            else {
                checkError(ERROR_TYPE_A,0);
                if (tempchar == '\n') {
                    strRetract();
                    symbol = STRCON;
                    return;
                }
            }
        }
        symbol = STRCON;
        return;
    }
    else if (isCharToken()) {
        getChar();
        if (isCharLetter()) {
            catToken();
            getChar();
        }
        else {
            checkError(ERROR_TYPE_A,1);
        }
        if (isCharToken()) {
            symbol = CHARCON;
        }
        else {
            checkError(ERROR_TYPE_A, 0);
            symbol = CHARCON;
            retract();
        }
        return;
    }
    else {
        catToken();
        int r = reserve();
        if (r != 0) {
            symbol = r;
            return;
        }
        else {
            checkError(ERROR_TYPE_A,0);
            getsym();
        }
    }
}

int reserve() {
    int i;
    for (i = 0;i < 40;i++) {
        if (strcmp(token,num2name[i]) == 0) {
            return i;
        }
    }
    return IDENFR;
}

void retract() {
    inputtxt_top--;
}

void catToken() {
    token[token_top] = tempchar;
    token[token_top + 1] = '\0';
    token_top++;
}

void clearToken() {
    for (int i = 0; i <= token_top;i++) {
        token[i] = 0;
    }
    token_top = 0;
}

int getChar() {
    if (inputtxt_top > inputlength) {
        exit(0);
    }
    tempchar = inputtxt[inputtxt_top++];
    return 0;
}

extern char tempchar;

int isCharLetter() {
    return isLetter() || isDigit() || tempchar == '+' || tempchar == '-' || tempchar == '*' || tempchar == '/';
}

int isCharToken() {
    return tempchar == '\'';
}

int isStringLetter() {
    return tempchar >= 32 && tempchar <= 126 && tempchar != 34;
}

int isStringToken() {
    return tempchar == '"';
}

int isEqu() {
    return tempchar == '=';
}

int isDouble() {
    return tempchar == '=' || tempchar == '>' || tempchar == '<' || tempchar == '!';
}


int isDigit() {
    return isdigit(tempchar);
}

int isLetter() {
    return tempchar == '_' || isupper(tempchar) | islower(tempchar);
}

int isSpace() {
    //return tempchar == ' ' || tempchar == '\n' || tempchar == '\t' ||  tempchar == '\r';
    if (tempchar == '\n') {
        countLine++;
    }
    return isspace(tempchar);
}

void writeSymbolType(){
    if (token[0] != 0) {
        printf("%s %s\n", num2typename[symbol], token);
        //fprintf(OUT, "%s %s\n", num2typename[symbol], token);
        outfile << num2typename[symbol] << " " << token << endl;
    }
}
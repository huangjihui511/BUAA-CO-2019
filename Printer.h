//
// Created by 黄继辉 on 2019-09-30.
//

#ifndef GRAMMER_ANALYSE_PRINTER_H
#define GRAMMER_ANALYSE_PRINTER_H

#include "LexicalAnalyzer.h"
#include "MiddleCode.h"
#include <string.h>

typedef struct struct1 {
    int symbol;
    char name[MAXTXTLENGTH];
    int line;
} Token;


void printToken();

void printTokenRetract(int a);

void inputToken();

void printTag(int tag);

void printError(int errorType);

char *getMcode(int i);

void printMcode();

void printMips(char *str);

char *mcode2string(Mcode *mcodeptr);

int selectType(int type);

void strRetract();

#endif //GRAMMER_ANALYSE_PRINTER_H

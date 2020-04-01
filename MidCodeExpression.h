//
// Created by 黄继辉 on 2019-11-13.
//

#ifndef MIDDLE_CODE_MIDCODEEXPRESSION_H
#define MIDDLE_CODE_MIDCODEEXPRESSION_H


#include "LexicalAnalyzer.h"


typedef struct struct6 {
    int type;
    int value;
    int op;
    char name[20];
    int index;
} MidCodeFactor;

typedef struct struct5{
    int length;
    int op;
    // 0 -> pos, 1->neg
    MidCodeFactor factors[20];
} MidCodeItem;


typedef struct struct4{
    int length;
    // 0 -> pos, 1->neg
    MidCodeItem items[20];
} MidCodeExpression;

void createMidCodeExpression();

void setNegMidCodeItem(int op);

void addMidCodeExpression();

void deleteMidCodeExpression();

#define FACTOR_BZF 0
#define FACTOR_BZF_ARRAY 1
#define FACTOR_BDS 2
#define FACTOR_ZS 3
#define FACTOR_ZF 4
#define FACTOR_HS 5

MidCodeFactor createMidCodeFactor(int type, int value, char name[MAXTXTLENGTH]);

void addMidCodeItem(MidCodeFactor mcf);

void setNegMidCodeFactor(MidCodeFactor *mcf, int op);

#endif //MIDDLE_CODE_MIDCODEEXPRESSION_H

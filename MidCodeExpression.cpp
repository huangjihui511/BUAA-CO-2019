//
// Created by 黄继辉 on 2019-11-13.
//

#include "MidCodeExpression.h"
#include "MiddleCode.h"
#include "GenerateMips.h"
#include <string.h>
#include <stdio.h>

MidCodeExpression mceList[20];

int mceListTop = -1;

int  preExpressionName;
void createMidCodeExpression() {
    mceListTop++;
    MidCodeExpression *mce = &mceList[mceListTop];
    mce->length = 0;
    MidCodeItem *mci = &mce->items[0];
    mci->length = 0;
    //printf("add%d\n",mceListTop);
    mceList[mceListTop].length = 0;

}


void addMidCodeExpression() {
    MidCodeExpression *mce = &mceList[mceListTop];
    mce->length++;
    if (mce->length == 2) {
        int newname;
        newname = expressionMcodeTable(mce);
        mce->length = 1;
        mce->items[0].factors[0] = createMidCodeFactor(FACTOR_BDS, 0, NULL);
        mce->items[0].factors[0].index = newname;
    }
    MidCodeItem *mci = &mce->items[mce->length];
    mci->length = 0;
}

void deleteMidCodeExpression() {
    preExpressionName = expressionMcodeTable( &mceList[mceListTop]);
    mceListTop--;
    //printf("minus%d\n",mceListTop);
}


void setNegMidCodeItem(int op) {
    MidCodeExpression *mce = &mceList[mceListTop];
    int length = mce->length;
    MidCodeItem *mci = &mce->items[length];
    mci->op = op;
}

void addMidCodeItem(MidCodeFactor mcf) {
    MidCodeExpression *mce = &mceList[mceListTop];
    int length = mce->length;
    MidCodeItem *mci = &mce->items[length];
    mci->factors[mci->length] = mcf;
    mci->length++;
    if (mcf.type == FACTOR_BDS) {
        mci->factors[mci->length - 1].index = preExpressionName;
    }
    if (mcf.type == FACTOR_BZF_ARRAY) {
        mci->factors[mci->length - 1].index = preExpressionName;
    }
    if (mcf.type == FACTOR_HS) {
        int name1 = allocTempVar();
        addMcodeTable(MID_MOVE_V0, NULL, NULL, NULL, 0, 0, 0, name1, 0, 0, 0, 0);
        mci->factors[mci->length - 1].index = name1;
    }
    if (mci->length == 2) {
        int newname = itemMcodeTable(mci);
        mci->length = 1;
        mci->factors[0] = createMidCodeFactor(FACTOR_BDS, 0, NULL);
        mci->factors[0].index = newname;
    }
}


MidCodeFactor createMidCodeFactor(int type, int value, char name[MAXTXTLENGTH]) {
    MidCodeFactor mcf;
    mcf.type = type;
    mcf.value = value;
    mcf.op = 0;
    if (name != NULL) {
        strcpy(mcf.name, name);
    }
    return mcf;
}

void setNegMidCodeFactor(MidCodeFactor *mcf, int op) {
    mcf->op = op;
}



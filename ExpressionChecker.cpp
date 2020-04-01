//
// Created by 黄继辉 on 2019-10-31.
//

#include <stdlib.h>
#include "ExpressionChecker.h"
#include "SymbleTable.h"
#include "Printer.h"

extern int tokenstop;

extern Token tokens[MAXTXTLENGTH];

int BDSTypes[500];

int BDSTypesTop = -1;

int preBDSType;

int returnType;

void createBDSType() {
    BDSTypesTop ++;
    BDSTypes[BDSTypesTop] = EXP_TYPE_NULL;
}

void getBDStype(int type2) {
    if (type2 != EXP_TYPE_NULL) {
        BDSTypes[BDSTypesTop] = EXP_TYPE_INT;
    }
}

void getXtype(int type2) {
    if (type2 != EXP_TYPE_NULL) {
        BDSTypes[BDSTypesTop] = EXP_TYPE_INT;
    }
}

void getYZTyoe(int type1) {
    if (BDSTypes[BDSTypesTop] != EXP_TYPE_NULL) {
        BDSTypes[BDSTypesTop] = EXP_TYPE_INT;
        return;
    }
    else if (type1 != EXP_TYPE_NULL) {
        BDSTypes[BDSTypesTop] = type1;
    }
    else {
        char *name = tokens[tokenstop - 1].name;
        int index = hasSymble(name);
        int type = getType(index);
        switch (type) {
            case DATATYPE_INT_CONST:
            case DATATYPE_INT_VALUE:
            case DATATYPE_INT_ARRAY:
            case DATATYPE_INT_RETURN: {
                BDSTypes[BDSTypesTop] = EXP_TYPE_INT;
                break;
            }
            case DATATYPE_CHAR_CONST:
            case DATATYPE_CHAR_VALUE:
            case DATATYPE_CHAR_ARRAY:
            case DATATYPE_CHAR_RETURN: {
                BDSTypes[BDSTypesTop] = EXP_TYPE_CHAR;
                break;
            }
        }
    }
}

void endBDSType() {
    preBDSType = BDSTypes[BDSTypesTop];
    BDSTypesTop --;
}

int getpreBDSType() {
    return preBDSType;
}

void initReturnType() {
    returnType = EXP_TYPE_NULL;
}

void addReturnType(int isBDS) {
    if (isBDS) {
        returnType = getpreBDSType();
    }
    else {
        returnType = EXP_TYPE_NULL;
    }
}

int checkReturnType() {
    return returnType;
}
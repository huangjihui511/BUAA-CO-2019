#include "LexicalAnalyzer.h"
#include "MiddleCode.h"
#include "SymbleTable.h"
#include "Printer.h"
#include "MidCodeExpression.h"
#include "ExpressionChecker.h"
#include "GenerateMips.h"
#include <string.h>
#include <cstdio>
//
// Created by 黄继辉 on 2019-11-12.
//

extern Token tokens[MAXTXTLENGTH * 100];

extern int recentHSDYYJ;

extern int preExpressionName;

extern int ZS_value;

extern int tokenstop;

extern SubFHB *subFHB;

Mcode mcodeTable[MAXTXTLENGTH * 10];

int mcodeTableTop = -1;

int labelID = 0;

int endlabelID = 0;

int tempVarID = 1;

int isNotGloble = 1;

int tempUsed[MAX_NUM_OF_BZF] = {0};

Mcode tempMcode;

void addMcodeTable(int midType, char nameLeft[], char nameRight1[]
        , char nameRight2[], int numLeft, int numRight1
        , int numRight2, int tempLeft, int tempRight1
        , int tempRight2, int indexLeft, int indexRight) {
    mcodeTableTop++;
    mcodeTable[mcodeTableTop].midType = midType;
    if (nameLeft != NULL) {
        strcpy(mcodeTable[mcodeTableTop].nameLeft,nameLeft);
    }
    if (nameRight1 != NULL) {
        strcpy(mcodeTable[mcodeTableTop].nameRight1,nameRight1);
    }
    if (nameRight2 != NULL) {
        strcpy(mcodeTable[mcodeTableTop].nameRight2,nameRight2);
    }
    mcodeTable[mcodeTableTop].numLeft = numLeft;
    mcodeTable[mcodeTableTop].numRight1 = numRight1;
    mcodeTable[mcodeTableTop].numRight2 = numRight2;
    mcodeTable[mcodeTableTop].tempLeft = tempLeft;
    mcodeTable[mcodeTableTop].tempRight1 = tempRight1;
    mcodeTable[mcodeTableTop].tempRight2 = tempRight2;
    mcodeTable[mcodeTableTop].indexLeft = indexLeft;
    mcodeTable[mcodeTableTop].indexRight = indexRight;
}

void insertMcodeTable(int midType, char name[]) {
    if (midType == MID_FUNC_INT
    || midType == MID_FUNC_CHAR
    || midType == MID_FUNC_VOID) {
        isNotGloble = 0;
        tempVarID = 1;
    }
    addMcodeTable(midType, name, NULL, NULL, 0, 0, isNotGloble, 0, 0, 0, 0, 0);
    if ((midType == MID_CON_INT
    || midType == MID_CON_CHAR)
    && !isNotGloble) {
        int t = allocTempVar();
        addMcodeTable(MID_CONST_ASSIGH,name,NULL,NULL, 0, 0, isNotGloble, t, t, 0, 0, 0);
        recycleTempVar1step();
    }
}

void insertArrayLength(int v) {
    int i = mcodeTableTop;
    mcodeTable[i].numLeft = v;
}

void setMcodeTableValue(int value) {
    mcodeTable[mcodeTableTop].numRight1 = value;
    if (!mcodeTable[mcodeTableTop].numRight2) {
        mcodeTable[mcodeTableTop - 1].numRight1 = value;
    }
}

void pushMcodeTable() {
    addMcodeTable(MID_PUSH, NULL, NULL, NULL, 0, 0, 0, 0, preExpressionName, 0, 0, 0);
    recycleTempVar2int(0);
}

void startpushMcodeTable() {
    addMcodeTable(MID_START_PUSH, NULL, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0);
}

void endpushMcodeTable() {
    addMcodeTable(MID_END_PUSH, NULL, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0);
}

void scanfPushMcodeTable(char name[]) {
    addMcodeTable(MID_SCANF_PUSH, name, NULL, NULL, 0, 0, 0, allocTempVar(), 0, 0, 0, 0);
    recycleTempVar1step();
}

void printfPushVarMcodeTable() {
    addMcodeTable(MID_PRINTF_PUSH_VAR, NULL, NULL, NULL, (getpreBDSType()== EXP_TYPE_CHAR)? 0 : 1, 0, 0, 0, preExpressionName, 0, NULL, NULL);
    recycleTempVar2int(0);
}

void printfPushStrMcodeTable(char str[]) {
    addMcodeTable(MID_PRINTF_PUSH_STR, str, NULL, NULL, 0, 0, 0, 0, 0, 0, NULL, NULL);
    recycleTempVar2int(0);
}

void callMcodeTable() {
    char *name = tokens[recentHSDYYJ].name;
    addMcodeTable(MID_CALL, name, NULL, NULL, 0, 0, 0, 0, 0, 0, NULL, NULL);
    FHB *fhb = getSymbleTableByName(name);
    int n = fhb->csbLength;
    for (int i = 0;i < n;i++) {
        recycleTempVar1step();
    }
}

void printfCallMcodeTable() {
    addMcodeTable(MID_PRINTF_CALL, "printf", NULL, NULL, 0, 0, 0, 0, 0, 0, NULL, NULL);
}

void scanfCallMcodeTable() {
    addMcodeTable(MID_SCANF_CALL, "scanf", NULL, NULL, 0, 0, 0, 0, 0, 0, NULL, NULL);
}

void initAssignMcodeTable() {
    char nameVar[MAX_NAME_LENGTH];
    int retract = 2;
    int type = MID_STORE_VAR;

    if (tokens[tokenstop - 1].symbol == LBRACK) {
        type = MID_STORE_ARRAY;
    }
    strcpy(nameVar,tokens[tokenstop - retract].name);
    printf("%s\n",nameVar);
    tempMcode.midType = type;
    strcpy(tempMcode.nameLeft, nameVar);
}

void setIndexAssignMcodeTable() {
    tempMcode.indexLeft = preExpressionName;
}

void setValueAssignMcodeTable() {
    addMcodeTable(tempMcode.midType, tempMcode.nameLeft
            , NULL, NULL
            , 0, 0, 0, 0, preExpressionName, 0, tempMcode.indexLeft, 0);
    recycleTempVar1step();
    if (tempMcode.midType == MID_STORE_ARRAY) {
        recycleTempVar1step();
    }
    recycleTempVar2int(0);

}

void initEqualMcodeTable() {
    int symbol = tokens[tokenstop - 1].symbol;
    int type = 0;
    switch (symbol) {
        case LSS: {
            type = MID_LESS;
            break;
        }
        case LEQ: {
            type = MID_EQUAL_LESS;
            break;
        }
        case GRE: {
            type = MID_GREATER;
            break;
        }
        case GEQ: {
            type = MID_EQUAL_GREATER;
            break;
        }
        case EQL: {
            type = MID_EQUAL;
            break;
        }
        case NEQ: {
            type = MID_NOT_EQUAL;
            break;
        }
    }
    tempMcode.midType = type;
    tempMcode.tempRight1 = preExpressionName;
}

void setEqualMcodeTable() {
    addMcodeTable(tempMcode.midType, NULL, NULL, NULL, 0, 0, 0, 0, tempMcode.tempRight1, preExpressionName, 0, 0);
    recycleTempVar1step();
    recycleTempVar1step();
}
void newPrintfMcodeTable() {
    addMcodeTable(MID_NEW_PRINTF, NULL, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0);
}
void setEqualZeroMcodeTable() {
    int temp = allocTempVar();
    addMcodeTable(MID_ASSIGH_INT, NULL, NULL, NULL, 0, 0, 0, temp, 0, 0, 0, 0);
    addMcodeTable(MID_NOT_EQUAL, NULL, NULL, NULL, 0, 0, 0, 0, preExpressionName, temp, 0, 0);
    recycleTempVar1step();
    recycleTempVar1step();
}

void returnValueMcodeTable() {
    addMcodeTable(MID_RET_VALUE, NULL, NULL, NULL, endlabelID, 0, 0, 0, preExpressionName, 0, 0, 0);
    recycleTempVar2int(0);
    //tempVarID = 1;
}

void returnNOValueMcodeTable() {
    addMcodeTable(MID_RET_NOVALUE, NULL, NULL, NULL, endlabelID, 0, 0, 0, 0, 0, 0, 0);
    recycleTempVar2int(0);
    //tempVarID = 1;
}

void addStartLoop() {
    addMcodeTable(MID_START_LOOP, NULL, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0);
}

void addEndLoop() {
    addMcodeTable(MID_END_LOOP, NULL, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0);
}

int jumpMcodeTable(int jumpType) {
    switch (jumpType) {
        case JUMP_IF:
        case JUMP_FOR:
        case JUMP_WHILE: {
            addMcodeTable(MID_BNZ, NULL, NULL, NULL, labelID, 0, 0, 0, 0, 0, 0, 0);
            break;
        }
        case JUMP_DO: {
            addMcodeTable(MID_BZ, NULL, NULL, NULL, labelID, 0, 0, 0, 0, 0, 0, 0);
            break;
        }
        case JUMP: {
            addMcodeTable(MID_GOTO, NULL, NULL, NULL, labelID, 0, 0, 0, 0, 0, 0, 0);
            break;
        }
    }
    return mcodeTableTop;
}

int labelMcodeTable() {
    labelID ++;
    addMcodeTable(MID_LABEL, NULL, NULL, NULL, labelID, 0, 0, 0, 0, 0, 0, 0);
    return labelID;
}

void fillInJumpMcodeTable(int index, int target) {
    mcodeTable[index].numRight1 = target;
}

void addNewline() {
    insertMcodeTable(MID_NEWLINE, NULL);
}

void forAdd(char name1[], char name2[], int value) {
    char tempname1[MAX_NAME_LENGTH] = {0};
    char tempname2[MAX_NAME_LENGTH] = {0};
    int tempname3;
    int tempname4;
    int tempname5;
    strcpy(tempname1, name1);
    strcpy(tempname2, name2);
    tempname3 = allocTempVar();
    tempname4 = allocTempVar();
    tempname5 = allocTempVar();
    addMcodeTable(MID_ASSIGH_VAR, NULL, tempname2, NULL, 0, 0, 0, tempname3, 0, 0, 0, 0);
    addMcodeTable(MID_ASSIGH_INT, NULL, NULL, NULL, 0, value, 0, tempname4, 0, 0, 0, 0);
    addMcodeTable(MID_ADD, NULL, NULL, NULL, 0, 0, 0, tempname5, tempname3, tempname4, 0, 0);
    addMcodeTable(MID_STORE_VAR, tempname1, NULL, NULL, 0, 0, 0, 0, tempname5, tempname4, 0, 0);
    recycleTempVar1step();
    recycleTempVar1step();
}

int allocTempVar() {
    if (subFHB != NULL) {
        //printf("allocError\n");
        insertSubSymbleTable(NULL, DATATYPE_INT_VALUE, 0, tempVarID);
    }
    return tempVarID++;
}

void recycleTempVar2int(int i) {
    //tempVarID = i;
}

void recycleTempVar1step() {
    //tempVarID --;
}

void addEndFunction() {
    addMcodeTable(MID_END_FUNCTION, NULL, NULL, NULL, endlabelID, 0, 0, 0, 0, 0, NULL, NULL);
    endlabelID++;
}

int expressionMcodeTable(MidCodeExpression *mce) {
    if (mce->length == 0) {
        return NULL;
    }
    int name1;
    int name2;
    int name3;
    int tempVarPos = tempVarID;
    name1 = itemMcodeTable(&mce->items[0]);
    if (mce->items[0].op != 0) {
        name3 = allocTempVar();
        addMcodeTable(MID_ASSIGH_OPPOSE, NULL, NULL, NULL, 0, 0, 0, name3, name1, 0, 0, 0);
        mce->items[0].op = 0;
    }
    else {
        name3 = name1;
    }
    if (mce->length == 1) {
        return name3;
    }
    else if (mce->length == 2) {
        name2 = itemMcodeTable(&mce->items[1]);
        recycleTempVar2int(tempVarPos);
        if (mce->items[1].factors[0].type == FACTOR_BDS) {
            recycleTempVar1step();
        }
        name1 = allocTempVar();
        switch (mce->items[1].op) {
            case 0: {
                addMcodeTable(MID_ADD, NULL, NULL, NULL, 0, 0, 0, name1, name3, name2, 0, 0);
                break;
            }
            case 1: {
                addMcodeTable(MID_MINUS, NULL, NULL, NULL, 0, 0, 0, name1, name3, name2, 0, 0);
                break;
            }
        }
        name3 = name1;
        return name3;
    }
}

int itemMcodeTable(MidCodeItem *mci) {
    int name1;
    int name2;
    int name3;
    int tempVarPos = tempVarID;
    name1 = factorMcodeTable(&mci->factors[0]);
    if (mci->length == 1) {
        return name1;
    }
    else if (mci->length == 2){
        name2 = factorMcodeTable(&mci->factors[1]);
        recycleTempVar2int(tempVarPos);
        if (mci->factors[1].type == FACTOR_BDS) {
            recycleTempVar1step();
        }
        name3 = allocTempVar();
        switch (mci->factors[1].op) {
            case 0: {
                addMcodeTable(MID_MULT, NULL, NULL, NULL, 0, 0, 0, name3, name1, name2, 0, 0);
                break;
            }
            case 1: {
                addMcodeTable(MID_DIVI, NULL, NULL, NULL, 0, 0, 0, name3, name1, name2, 0, 0);
                break;
            }
        }
        return name3;
    }
}

int factorMcodeTable(MidCodeFactor *mcf) {
    int name1;
    switch (mcf->type) {
        case FACTOR_BZF: {
            name1 = allocTempVar();
            addMcodeTable(MID_ASSIGH_VAR, NULL, mcf->name, NULL, 0, mcf->value, 0, name1, 0, 0, 0, 0);
            break;
        }
        case FACTOR_BZF_ARRAY: {
            name1 = allocTempVar();
            addMcodeTable(MID_READ_ARRAY, NULL, mcf->name, NULL, 0, 0, 0, name1, 0, 0, 0, mcf->index);
            break;
        }
        case FACTOR_BDS: {
            name1 = mcf->index;
            break;
        }
        case FACTOR_ZS: {
            name1 = allocTempVar();
            addMcodeTable(MID_ASSIGH_INT, NULL, NULL, NULL, 0, mcf->value, 0, name1, 0, 0, 0, 0);
            break;
        }
        case FACTOR_ZF: {
            name1 = allocTempVar();
            addMcodeTable(MID_ASSIGH_CHAR, NULL, NULL, NULL, 0, mcf->value, 0, name1, 0, 0, 0, 0);
            break;
        }
        case FACTOR_HS: {
            name1 = mcf->index;
            break;
        }
    }
    return name1;
}


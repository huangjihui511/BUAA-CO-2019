#include <cstdio>
#include <cstring>
#include <fstream>
#include "MiddleCode.h"
#include "GenerateMips.h"
#include "Printer.h"
#include "SymbleTable.h"
#include "Optimal.h"
#include "MemoAlloc.h"
//
// Created by 黄继辉 on 2019-11-15.
//
using namespace std;

extern int mcodeTableTop;

extern Mcode mcodeTable[MAXTXTLENGTH];

extern FHB symbleTable[MAX_NUM_OF_BZF];

extern McodeList *mcodeListHead;

int istext = 1;

FHB *presentFHB = NULL;

int numpara = 0;

int jumptype;

char strsTable[MAX_NUM_OF_BZF][MAXTXTLENGTH] = {0};

int strsTableTop = 0;

int variableType = 0;

McodeList *pushlist = NULL;

Printlink *printlink = NULL;

//McodeList *paralist = NULL;

char *scanMidCode() {
    char *text = (char *)malloc(MAXTXTLENGTH * sizeof(char));
    McodeList *mcodeListptr = mcodeListHead;
    strcpy(text, ".data\n");
    printMips(text);
    while (mcodeListptr != nullptr) {
        Mcode *mcodeptr = &mcodeListptr->mcode;
        printf("\n# %s\n",mcode2string(mcodeptr));
        char *temptext = Mcode2Mips(mcodeListptr);
        //strcat(text, temptext);
        printMips(temptext);
        mcodeListptr = mcodeListptr->nextnode;
    }
    return text;
}

char *initStr() {
    char *inittext = (char *)malloc(MAXTXTLENGTH * sizeof(char));
    strcpy(inittext, "NL:\t.asciiz\t\"\\n\"\n");
    for (int i = 0;i <= mcodeTableTop;i++) {
        if (mcodeTable[i].midType == MID_PRINTF_PUSH_STR) {
            char temptext[MAXTXTLENGTH] = {0};
            int j;
            for (j = 0;j < strsTableTop;j++) {
                if (strcmp(strsTable[j],mcodeTable[i].nameLeft) == 0) {
                    break;
                }
            }
            if (j == strsTableTop) {
                sprintf(temptext, "STR%d:\t.asciiz\t\"%s\"\n", strsTableTop, handleTransfore(mcodeTable[i].nameLeft));
                strcpy(strsTable[strsTableTop], mcodeTable[i].nameLeft);
                strsTableTop++;
                strcat(inittext, temptext);
            }
        }
    }
    return inittext;
}

char *Mcode2Mips(McodeList *mcodeListptr) {
    Mcode *mcodeptr = &mcodeListptr->mcode;
    int type = mcodeptr->midType;
    char *temptext = (char *)malloc(MAXTXTLENGTH * sizeof(char));
    // init
    if (mcodeptr->numRight2) {
        switch (type) {
            case MID_CON_CHAR:
            case MID_CON_INT: {
                sprintf(temptext,"%s:\t.word\t%d\n", mcodeptr->nameLeft, mcodeptr->numRight1);
                return temptext;
            }
            case MID_VAR_CHAR:
            case MID_VAR_INT: {
                sprintf(temptext,"%s:\t.word\t0\n", mcodeptr->nameLeft);
                return temptext;
            }
            case MID_VAR_ARRAY_INT:
            case MID_VAR_ARRAY_CHAR: {
                sprintf(temptext,"%s:\t.space\t%d\n", mcodeptr->nameLeft, mcodeptr->numLeft * 4);
                return temptext;
            }
        }
    }
    if (istext) {
        sprintf(temptext, "%s\n.text\njal main\n", initStr());
        istext = 0;
    }
    switch (type) {
        case MID_FUNC_VOID:
        case MID_FUNC_CHAR:
        case MID_FUNC_INT: {
            presentFHB = getSymbleTableByName(mcodeptr->nameLeft);
            //allocVariableLength(presentFHB);
            sprintf(temptext, "%s\n%s:\n",temptext, mcodeptr->nameLeft);
            break;
        }
        case MID_CONST_ASSIGH: {
            sprintf(temptext,"%s%s\n",temptext, initConst(mcodeptr->nameLeft,mcodeptr->numRight1));
            break;
        }
        case MID_PUSH: {
            addPushList(mcodeListptr);
            break;
        }
        case MID_CALL: {
            sprintf(temptext,"%s\n%s\n",temptext, call(mcodeptr->nameLeft, mcodeListptr->regs));
            break;
        }
        case MID_EQUAL:
        case MID_LESS:
        case MID_GREATER:
        case MID_EQUAL_GREATER:
        case MID_EQUAL_LESS:
        case MID_NOT_EQUAL: {
            sprintf(temptext, "%s%s",temptext
                    , compare(type
                    , getOffset2Rindex(mcodeptr->tempRight1, presentFHB) + T0_OFFSET
                    , getOffset2Rindex(mcodeptr->tempRight2, presentFHB) + T0_OFFSET));
            break;
        }
        case MID_BZ0:
        case MID_BNZ0: {
            char labelname[20];
            sprintf(labelname, "%s%s_LABEL_%d",temptext
                    , mcodeptr->nameLeft, mcodeptr->numRight1);
            sprintf(temptext,"%s", jump2Labelbz0(type
                    ,getOffset2Rindex(mcodeptr->tempRight1, presentFHB) + T0_OFFSET
                    ,labelname));
            break;
        }
        case MID_BNZ: {
            char labelname[20];
            sprintf(labelname, "%s%s_LABEL_%d",temptext
                    , mcodeptr->nameLeft, mcodeptr->numRight1);
            sprintf(temptext,"%s", jump2Labelbnz(labelname));
            break;
        }
        case MID_BZ: {
            char labelname[20];
            sprintf(labelname, "%s%s_LABEL_%d", temptext, mcodeptr->nameLeft, mcodeptr->numRight1);
            sprintf(temptext,"%s", jump2Labelbz(labelname));
            break;
        }
        case MID_GOTO: {
            char labelname[20];
            sprintf(labelname, "%s%s_LABEL_%d", temptext, mcodeptr->nameLeft, mcodeptr->numRight1);
            sprintf(temptext,"j %s\n", labelname);
            break;
        }
        case MID_LABEL: {
            char labelname[20];
            sprintf(labelname, "%s%s_LABEL_%d", temptext,mcodeptr->nameLeft, mcodeptr->numLeft);
            sprintf(temptext,"\n%s:\n\n",labelname);
            break;
        }
        case MID_READ_ARRAY: {
            sprintf(temptext,"%s%s", temptext, loadArray2Register(mcodeptr->nameRight1, getOffset2Rindex(mcodeptr->tempLeft, presentFHB) + T0_OFFSET, getOffset2Rindex(mcodeptr->indexRight, presentFHB) + T0_OFFSET));
            break;
        }
        case MID_ASSIGH_INT:
        case MID_ASSIGH_CHAR: {
            sprintf(temptext, "%s%s", temptext, assignIntChar(getOffset2Rindex(mcodeptr->tempLeft, presentFHB) + T0_OFFSET, mcodeptr->numRight1));

            break;
        }
        case MID_DIVI:
        case MID_MULT:
        case MID_ADD:
        case MID_MINUS: {
            sprintf(temptext, "%s%s", temptext, fourCompute(type, getOffset2Rindex(mcodeptr->tempLeft, presentFHB) + T0_OFFSET, getOffset2Rindex(mcodeptr->tempRight1, presentFHB) + T0_OFFSET, getOffset2Rindex(mcodeptr->tempRight2, presentFHB) + T0_OFFSET));
            break;
        }
        case MID_ASSIGH_OPPOSE: {
            sprintf(temptext, "%s%s", temptext, assignOppose(getOffset2Rindex(mcodeptr->tempLeft, presentFHB) + T0_OFFSET, getOffset2Rindex(mcodeptr->tempRight1, presentFHB) + T0_OFFSET));
            break;
        }
        case MID_STORE_VAR: {
            sprintf(temptext, "%s%s", temptext
                    , storeRegister2Variable(mcodeptr->nameLeft
                    , getOffset2Rindex(mcodeptr->tempRight1, presentFHB) + T0_OFFSET));
            break;
        }
        case MID_STORE_ARRAY: {
            sprintf(temptext, "%s%s", temptext, storeRegister2Array(getOffset2Rindex(mcodeptr->tempRight1, presentFHB) + T0_OFFSET
                    , mcodeptr->nameLeft
                    , getOffset2Rindex(mcodeptr->indexLeft, presentFHB) + T0_OFFSET));
            break;
        }
        case MID_ASSIGH_VAR: {
            sprintf(temptext, "%s%s", temptext, loadVariable2Register(mcodeptr->nameRight1
                    , getOffset2Rindex(mcodeptr->tempLeft, presentFHB) + T0_OFFSET));

            break;
        }
        case MID_END_FUNCTION: {
            sprintf(temptext, "%s\nEND_FUNCTION_%d :\n\n",temptext, mcodeptr->numLeft);
            if (strcmp(presentFHB->name, "main")) {
                sprintf(temptext, "%s%s", temptext, endFunction());
            }
            else {
                sprintf(temptext,"%s%s\n", temptext, syscallCommand(10, 0, NULL));
            }
            break;
        }
        case MID_INLINE_END: {
            sprintf(temptext, "%s\nEND_FUNCTION_%d :\n\n",temptext, mcodeptr->numLeft);
            break;
        }
        case MID_INLINE_RET_VALUE: {
            sprintf(temptext, "%s%s",temptext, move2V0(getOffset2Rindex(mcodeptr->tempRight1, presentFHB) + T0_OFFSET));
            break;
        }
        case MID_RET_VALUE: {
            sprintf(temptext, "%s%s",temptext, move2V0(getOffset2Rindex(mcodeptr->tempRight1, presentFHB) + T0_OFFSET));
            char labelname[20];
            sprintf(labelname, "END_FUNCTION_%d", mcodeptr->numLeft);
            //sprintf(temptext, "%s%s", temptext, jCommand(labelname));
            sprintf(temptext, "%s%s", temptext, endFunction());
            break;
        }
        case MID_INLINE_RET_NO_VALUE: {
            break;
        }
        case MID_RET_NOVALUE: {
            char labelname[20];
            sprintf(labelname, "%sEND_FUNCTION_%d", temptext, mcodeptr->numLeft);
            sprintf(temptext, "%s", jCommand(labelname));
            break;
        }
        case MID_SCANF_PUSH: {
            int sysType = 0;
            setVariableType(mcodeptr->nameLeft);
            switch (variableType) {
                case MIPS_VARIBLE_TYPE_char: {
                    sysType = 12;
                    break;
                }
                case MIPS_VARIBLE_TYPE_INT: {
                    sysType = 5;
                    break;
                }
            }
            sprintf(temptext, "%s%s",temptext, syscallCommand(sysType, getOffset2Rindex(mcodeptr->tempLeft, presentFHB) + T0_OFFSET, mcodeptr->nameLeft));
            break;
        }
        case MID_PRINTF_PUSH_VAR: {
            printlink->is_value = 1;
            printlink->value = mcodeptr->tempRight1;
            int sysType = 0;
            switch (mcodeptr->numLeft) {
                case 0: {
                    sysType = 11;
                    break;
                }
                case 1: {
                    sysType = 1;
                    break;
                }
            }
            sprintf(temptext, "%s%s", temptext, syscallCommand(sysType, getOffset2Rindex(mcodeptr->tempRight1, presentFHB) + T0_OFFSET, NULL));
            break;
        }
        case MID_PRINTF_PUSH_STR: {
            printlink->str = mcodeptr->nameLeft;
            sprintf(temptext, "%s%s",temptext , syscallCommand(4, 0, mcodeptr->nameLeft));
            break;
        }
        case MID_NEWLINE: {
            sprintf(temptext, "%s%s",temptext, newLine());
            break;
        }
        case MID_PARA_CHAR:
        case MID_PARA_INT: {
            sprintf(temptext,"%s%s", temptext,loadPara());
            break;
        }
        case MID_MOVE_V0: {
            sprintf(temptext, "%s%s", temptext,moveFromV0(getOffset2Rindex(mcodeptr->tempLeft, presentFHB) + T0_OFFSET));
            break;
        }
        case MID_INLINE_STORE:
        case MID_INLINE_MOVE:
        case MID_INLINE_LOAD: {
            sprintf(temptext, "%s%s", temptext,inlineLoad(getOffset2Rindex(mcodeptr->tempLeft, presentFHB) + T0_OFFSET, getOffset2Rindex(mcodeptr->tempRight1, presentFHB) + T0_OFFSET));
            break;
        }
        case MID_ADDI: {
            sprintf(temptext, "%s%s", temptext,addi(getOffset2Rindex(mcodeptr->tempLeft, presentFHB) + T0_OFFSET, getOffset2Rindex(mcodeptr->tempRight1, presentFHB) + T0_OFFSET, mcodeptr->numRight1));
            break;
        }
        case MID_NEW_PRINTF: {
            Printlink *tempPrintflink = (Printlink *)malloc(sizeof(Printlink));
            tempPrintflink->nextnode = printlink;
            printlink = tempPrintflink;
            break;
        }
        case MID_PRINTF_CALL: {
            /*
            if (printlink->str != NULL) {
                sprintf(temptext, "%s%s", temptext, syscallCommand(4, 0, printlink->str));
            }
            if (printlink->is_value) {
                int sysType = 0;
                switch (printlink->type_value) {
                    case 0: {
                        sysType = 11;
                        break;
                    }
                    case 1: {
                        sysType = 1;
                        break;
                    }
                }
                sprintf(temptext, "%s%s", temptext, syscallCommand(sysType, getOffset2Rindex(printlink->value, presentFHB) + T0_OFFSET, NULL));

            }
            printlink = printlink->nextnode;
            break;
             */
             break;
        }
    }
    return temptext;
}


char *swCommand(int r1, int offset, int r2) {
    char *text = (char *)malloc(MAXTXTLENGTH * sizeof(char));

    sprintf(text, "sw $%s, %d($%s)\n", tempNum2RegisterName(r1), offset, tempNum2RegisterName(r2));
    return text;
}

char *lwCommand(int r1, int offset, int r2) {
    char *text = (char *)malloc(MAXTXTLENGTH * sizeof(char));
    sprintf(text, "lw $%s, %d($%s)\n", tempNum2RegisterName(r1), offset, tempNum2RegisterName(r2));
    return text;
}

char *liCommand(int r1, int value) {
    char *text = (char *)malloc(MAXTXTLENGTH * sizeof(char));
    sprintf(text, "li $%s, %d\n", tempNum2RegisterName(r1), value);
    return text;
}

char *jalCommand(char name[]) {
    char *text = (char *)malloc(MAXTXTLENGTH * sizeof(char));
    sprintf(text,"jal %s\n",name);
    return text;
}

char *sllCommand(int r1, int r2, int offset) {
    char *text = (char *)malloc(MAXTXTLENGTH * sizeof(char));
    sprintf(text,"sll $%s, $%s, %d\n", tempNum2RegisterName(r1), tempNum2RegisterName(r2), offset);
    return text;
}

char *addCommand(int r1, int r2, int r3) {
    char *text = (char *)malloc(MAXTXTLENGTH * sizeof(char));
    sprintf(text,"add $%s, $%s, $%s\n", tempNum2RegisterName(r1), tempNum2RegisterName(r2), tempNum2RegisterName(r3));
    return text;
}

char *addiCommand(int r1, int r2, int value) {
    char *text = (char *)malloc(MAXTXTLENGTH * sizeof(char));
    sprintf(text,"addi $%s, $%s, %d\n", tempNum2RegisterName(r1), tempNum2RegisterName(r2), value);
    return text;
}

char *subCommand(int r1, int r2, int r3) {
    char *text = (char *)malloc(MAXTXTLENGTH * sizeof(char));
    sprintf(text,"sub $%s, $%s, $%s\n", tempNum2RegisterName(r1), tempNum2RegisterName(r2), tempNum2RegisterName(r3));
    return text;
}

char *laCommand(int r1, char variableName[]) {
    char *text = (char *)malloc(MAXTXTLENGTH * sizeof(char));
    sprintf(text, "la $%s, %s\n", tempNum2RegisterName(r1), variableName);
    return text;
}

char *multCommand(int r1, int r2, int r3) {
    char *text = (char *)malloc(MAXTXTLENGTH * sizeof(char));
    sprintf(text,"mul $%s, $%s, $%s\n", tempNum2RegisterName(r1), tempNum2RegisterName(r2), tempNum2RegisterName(r3));
    //sprintf(text,"%smflo $%s\n", text, tempNum2RegisterName(r1));
    return text;
}

char *divCommand(int r1, int r2, int r3) {
    char *text = (char *)malloc(MAXTXTLENGTH * sizeof(char));
    sprintf(text,"div $%s, $%s\n", tempNum2RegisterName(r2), tempNum2RegisterName(r3));
    sprintf(text,"%smflo $%s\n", text, tempNum2RegisterName(r1));
    return text;
}

char *moveCommand(int r1, int r2) {
    char *text = (char *)malloc(MAXTXTLENGTH * sizeof(char));
    sprintf(text,"move $%s, $%s\n", tempNum2RegisterName(r1), tempNum2RegisterName(r2));
    return text;
}

char *jCommand(char labelName[]) {
    char *text = (char *)malloc(MAXTXTLENGTH * sizeof(char));
    sprintf(text,"j %s\n", labelName);
    return text;
}

char *syscallCommand(int type, int r1, char str[]) {
    char *text = (char *)malloc(MAXTXTLENGTH * sizeof(char));
    if (!isTempInRegister(r1 - T0_OFFSET)) {
        strcat(text, loadLargeTemp(r1 - T0_OFFSET, 0));
        r1 = T8_OFFSET;
    }
    if (r1 < 0) {
        r1 = -r1;
    }
    switch (type) {
        case 11:
        case 1: {
            strcat(text, moveCommand(A0_OFFSET, r1));
            strcat(text, liCommand(V0_OFFSET, type));
            strcat(text, "syscall\n");
            break;
        }
        case 4: {
            int i;
            for (i = 0;i < strsTableTop;i++) {
                if (strcmp(str, strsTable[i]) == 0) {
                    break;
                }
            }
            char id[MAX_NAME_LENGTH] = {0};
            sprintf(id, "STR%d",i);
            strcat(text, laCommand(A0_OFFSET, id));
            strcat(text, liCommand(V0_OFFSET, type));
            strcat(text, "syscall\n");
            break;
        }
        case 12:
        case 5: {
            strcat(text, liCommand(V0_OFFSET, type));
            strcat(text, "syscall\n");
            strcat(text, storeRegister2Variable(str, V0_OFFSET));
            break;
        }
        case 10: {
            strcat(text, liCommand(V0_OFFSET, type));
            strcat(text, "syscall\n");
        }
    }
    return text;
}


char *tempNum2RegisterName(int num) {
    char *name = (char*)malloc(10*sizeof(char));
    if (num >=T0_OFFSET and num < T0_OFFSET + 8) {
        sprintf(name, "t%d", num - T0_OFFSET);
        return name;
    }
    else if (num >=S0_OFFSET and num < S0_OFFSET + 8) {
        sprintf(name, "s%d", num - S0_OFFSET);
        return name;
    }
    else if (num >=A0_OFFSET and num < A0_OFFSET + 4) {
        sprintf(name, "a%d", num - A0_OFFSET);
        return name;
    }
    else if (num >= T8_OFFSET and num < T8_OFFSET + 2) {
        sprintf(name, "t%d", num - T8_OFFSET + 8);
        return name;
    }
    else if (num == V0_OFFSET) {
        return "v0";
    }
    else if (num == SP_OFFSET) {
        return "sp";
    }
    else if (num == RA_OFFSET) {
        return "ra";
    }
    else if (num == 0) {
        return "0";
    }
    printf("invalide tempnum\n");
    return "null";
}

char *storeRegister2Variable(char *variableName, int rsrc) {
    char *text = (char *)malloc(MAXTXTLENGTH * sizeof(char));
    if (!isTempInRegister(rsrc - T0_OFFSET)) {
        strcat(text, loadLargeTemp(rsrc - T0_OFFSET, 0));
        rsrc = T8_OFFSET;
    }
    int i = findVarible(presentFHB, variableName);
    if (rsrc < 0) {
        rsrc = -rsrc;
    }
    if (i >= 0) {
        SubFHB *tempSubFHB = presentFHB->subFHB + i;
        if (tempSubFHB->isInRegister) {
            return moveCommand(tempSubFHB->rName, rsrc);
        }
        return swCommand(rsrc, - (VAR2SP + tempSubFHB->offset) * byte2word, SP_OFFSET);
    }
    int rposition = T8_OFFSET + 1;
    strcat(text, laCommand(rposition, variableName));
    strcat(text, swCommand(rsrc, 0, rposition));
    return text;
}

char *loadVariable2Register(char *variableName, int rtarget) {
    char *text = (char *)malloc(MAXTXTLENGTH * sizeof(char));
    int prertarget = rtarget;
    if (!isTempInRegister(rtarget - T0_OFFSET)) {
        rtarget = T8_OFFSET;
    }
    if (rtarget < 0) {
        rtarget = -rtarget;
    }
    int i = findVarible(presentFHB, variableName);
    if (i >= 0) {
        SubFHB *tempSubFHB = presentFHB->subFHB + i;
        if (tempSubFHB->isInRegister) {
            strcat(text, moveCommand(rtarget, tempSubFHB->rName));
        }
        else {
            strcat(text, lwCommand(rtarget, - (VAR2SP + tempSubFHB->offset) * byte2word, SP_OFFSET));
        }
    }
    else {
        strcat(text, laCommand(rtarget, variableName));
        strcat(text, lwCommand(rtarget, 0, rtarget));
    }
    if (!isTempInRegister(prertarget - T0_OFFSET)) {
        strcat(text, storeLargeTemp(prertarget - T0_OFFSET));
    }
    return text;
}

char *initConst(char *variableName, int value) {
    char *text = (char *)malloc(MAXTXTLENGTH * sizeof(char));
    SubFHB *tempSubFHB = presentFHB->subFHB;
    int i = findStatedVarible(presentFHB, variableName);
    tempSubFHB = tempSubFHB + i;
    sprintf(text, "%s", liCommand(T0_OFFSET,value));
    if (tempSubFHB->isInRegister) {
        strcat(text, moveCommand(tempSubFHB->rName, T0_OFFSET));
    }
    else {
        sprintf(text, "%s%s\n", text, swCommand(T0_OFFSET, -(tempSubFHB->offset + VAR2SP) * byte2word, SP_OFFSET));
    }
    return text;
}

char *loadPara() {
    char *text = (char *)malloc(MAXTXTLENGTH * sizeof(char));
    SubFHB *tempSubFHB = presentFHB->subFHB;
    tempSubFHB = tempSubFHB + numpara;
    if (numpara < S_NUM) {
        //strcat(text, moveCommand(tempSubFHB->rName, A0_OFFSET + numpara));
    }
    else {
        strcpy(text, lwCommand(tempSubFHB->rName, -(VAR2SP + numpara) * byte2word, SP_OFFSET));
    }
    numpara++;
    return text;
}

void addPushList(McodeList *mcodelistptr) {
    McodeList *tempPushList = (McodeList *)malloc(sizeof(McodeList));
    *tempPushList = *mcodelistptr;
    tempPushList->nextnode = pushlist;
    pushlist = tempPushList;
}


McodeList *getPushListTop() {
    McodeList *mcodeListptr = pushlist;
    pushlist = pushlist->nextnode;
    return mcodeListptr;
}


/*

char *computeParameter() {
    char *text = (char *)malloc(MAXTXTLENGTH * sizeof(char));
    McodeList *pushList = getPushListTop();
    int rsrc = pushList->mcode.tempLeft + T0_OFFSET;
    McodeList *tempPushList = pushList->pushcontent;
    while (tempPushList != NULL) {
        strcat(text, Mcode2Mips(tempPushList));
        printf("\n## %s",mcode2string(&tempPushList->mcode));
        tempPushList = tempPushList->nextnode;
    }
    printf("\n## Push\n");
    if (!isTempInRegister(rsrc - T0_OFFSET)) {
        strcat(text, loadLargeTemp(rsrc - T0_OFFSET, 0));
    }
    pushList->nextnode = paralist;
    paralist = pushList;
    return text;
}

*/


char *compare(int type, int r1, int r2) {
    char *text = (char *)malloc(MAXTXTLENGTH * sizeof(char));
    if (!isTempInRegister(r1 - T0_OFFSET)) {
        strcat(text, loadLargeTemp(r1 - T0_OFFSET, 0));
        r1 = T8_OFFSET;
    }
    if (!isTempInRegister(r2 - T0_OFFSET)) {
        strcat(text, loadLargeTemp(r2 - T0_OFFSET, 1));
        r2 = T8_OFFSET + 1;
    }
    if (r1 < 0) {
        r1 = -r1;
    }
    if (r2 < 0) {
        r2 = -r2;
    }
    switch (type) {
        case MID_EQUAL_GREATER: {
            strcat(text, subCommand(T8_OFFSET, r1, r2));
            jumptype = GREATER_EQUAL_JUMP;
            break;
        }
        case MID_LESS: {
            strcat(text, subCommand(T8_OFFSET, r2, r1));
            jumptype = GREATER_JUMP;
            break;
        }
        case MID_EQUAL: {
            strcat(text, subCommand(T8_OFFSET, r1, r2));
            jumptype = EQUAL_ZERO_JUMP;
            break;
        }
        case MID_NOT_EQUAL: {
            strcat(text, subCommand(T8_OFFSET, r1, r2));
            jumptype = NOT_EQUAL_ZERO_JUMP;
            break;
        }
        case MID_EQUAL_LESS: {
            strcat(text, subCommand(T8_OFFSET, r2, r1));
            jumptype = GREATER_EQUAL_JUMP;
            break;
        }
        case MID_GREATER: {
            jumptype = GREATER_JUMP;
            strcat(text, subCommand(T8_OFFSET, r1, r2));
            break;
        }
    }
    return text;
}

char *jump2Labelbz(char *labelname) {
    char *text = (char *)malloc(MAXTXTLENGTH * sizeof(char));
    switch (jumptype) {
        case GREATER_JUMP: {
            sprintf(text, "bgtz $t8, %s\n", labelname);
            break;
        }
        case GREATER_EQUAL_JUMP: {
            sprintf(text, "bgez $t8, %s\n", labelname);
            break;
        }
        case EQUAL_ZERO_JUMP: {
            sprintf(text, "beq $t8, $0, %s\n", labelname);
            break;
        }
        case NOT_EQUAL_ZERO_JUMP: {
            sprintf(text, "bne $t8, $0, %s\n", labelname);
            break;
        }
    }
    return text;
}

char *jump2Labelbz0(int type, int rsrc, char *labelname) {
    char *text = (char *)malloc(MAXTXTLENGTH * sizeof(char));
    if (!isTempInRegister(rsrc - T0_OFFSET)) {
        strcat(text, loadLargeTemp(rsrc - T0_OFFSET, 0));
        rsrc = T8_OFFSET;
    }
    if (rsrc < 0) {
        rsrc = - rsrc;
    }
    switch (type) {
        case MID_BNZ0: {
            sprintf(text, "bne $%s, $0, %s\n", tempNum2RegisterName(rsrc), labelname);
            break;
        }
        case MID_BZ0: {
            sprintf(text, "beq $%s, $0, %s\n", tempNum2RegisterName(rsrc), labelname);
            break;
        }
    }
    return text;
}

char *jump2Labelbnz(char *labelname) {
    char *text = (char *)malloc(MAXTXTLENGTH * sizeof(char));
    switch (jumptype) {
        case GREATER_JUMP: {
            sprintf(text, "blez $t8, %s\n", labelname);
            break;
        }
        case GREATER_EQUAL_JUMP: {
            sprintf(text, "bltz $t8, %s\n", labelname);
            break;
            case NOT_EQUAL_ZERO_JUMP: {
            }
            sprintf(text, "beq $t8, $0, %s\n", labelname);
            break;
        }
        case EQUAL_ZERO_JUMP: {
            sprintf(text, "bne $t8, $0, %s\n", labelname);
            break;
        }
    }
    return text;
}

char *loadArray2Register(char *arrayName, int rtarget, int rindex) {
    char *text = (char *)malloc(MAXTXTLENGTH * sizeof(char));
    int prertarget = rtarget;
    if (!isTempInRegister(rtarget - T0_OFFSET)) {
        rtarget = T8_OFFSET;
    }
    if (!isTempInRegister(rindex - T0_OFFSET)) {
        strcat(text, loadLargeTemp(rindex - T0_OFFSET, 1));
        rindex = T8_OFFSET + 1;
    }
    if (rindex < 0) {
        rindex = -rindex;
    }
    if (rtarget < 0) {
        rtarget = -rtarget;
    }
    int i = findVarible(presentFHB, arrayName);
    if (i >= 0) {
        SubFHB *tempSubFHB = presentFHB->subFHB + i;
        int offset = tempSubFHB->offset + VAR2SP;
        strcat(text, sllCommand(T8_OFFSET + 1, rindex, 2));
        strcat(text, subCommand(T8_OFFSET + 1, SP_OFFSET, T8_OFFSET + 1));
        strcat(text, lwCommand(rtarget, -offset * byte2word, T8_OFFSET + 1));
    }
    else {
        strcat(text, laCommand(rtarget, arrayName));
        //strcat(text, sllCommand(rtarget, rtarget, 2));
        strcat(text, sllCommand(T8_OFFSET + 1, rindex, 2));
        strcat(text, addCommand(rtarget, rtarget, T8_OFFSET + 1));
        strcat(text, lwCommand(rtarget, 0, rtarget));
    }
    if (!isTempInRegister(prertarget - T0_OFFSET)) {
        strcat(text, storeLargeTemp(prertarget - T0_OFFSET));
    }
    return text;
}

char *assignIntChar(char r1, int value) {
    char *text = (char *)malloc(MAXTXTLENGTH * sizeof(char));
    int prer1 = r1;
    if (!isTempInRegister(r1 - T0_OFFSET)) {
        r1 = T8_OFFSET;
    }
    if (r1 < 0) {
        r1 = -r1;
    }
    strcpy(text,liCommand(r1,value));
    if (!isTempInRegister(prer1 - T0_OFFSET)) {
        strcat(text, storeLargeTemp(prer1 - T0_OFFSET));
    }
    return text;
}

char *fourCompute(int type, int r1, int r2, int r3) {
    char *text = (char *)malloc(MAXTXTLENGTH * sizeof(char));
    if (!isTempInRegister(r2 - T0_OFFSET)) {
        strcat(text, loadLargeTemp(r2 - T0_OFFSET, 0));
        r2 = T8_OFFSET;
    }
    if (!isTempInRegister(r3 - T0_OFFSET)) {
        strcat(text, loadLargeTemp(r3 - T0_OFFSET, 1));
        r3 = T8_OFFSET + 1;
    }
    int prer1 = r1;
    if (!isTempInRegister(r1 - T0_OFFSET)) {
        r1 = T8_OFFSET;
    }
    if (r2 < 0) {
        r2 = -r2;
    }
    if (r3 < 0) {
        r3 = -r3;
    }
    if (r1 < 0) {
        r1 = -r1;
    }
    switch (type) {
        case MID_MULT: {
            strcat(text, multCommand(r1, r2, r3));
            break;
        }
        case MID_DIVI: {
            strcat(text, divCommand(r1, r2, r3));
            break;
        }
        case MID_ADD: {
            strcat(text, addCommand(r1, r2, r3));
            break;
        }
        case MID_MINUS: {
            strcat(text, subCommand(r1, r2, r3));
            break;
        }
    }
    if (!isTempInRegister(prer1 - T0_OFFSET)) {
        strcat(text, storeLargeTemp(prer1 - T0_OFFSET));
    }
    return text;
}

char *assignOppose(int rtarget, int rsrc) {
    char *text = (char *)malloc(MAXTXTLENGTH * sizeof(char));
    //strcpy(text, loadVariable2register(name, rname));
    if (!isTempInRegister(rsrc - T0_OFFSET)) {
        strcat(text, loadLargeTemp(rsrc - T0_OFFSET, 0));
        rsrc = T8_OFFSET;
    }
    if (rsrc < 0) {
        rsrc = -rsrc;
    }
    int prertarget = rtarget;
    if (!isTempInRegister(rtarget - T0_OFFSET)) {
        rtarget = T8_OFFSET;
    }
    if (rtarget < 0) {
        rtarget = -rtarget;
    }
    strcat(text, subCommand(rtarget, 0, rsrc));
    if (!isTempInRegister(prertarget - T0_OFFSET)) {
        strcat(text, storeLargeTemp(prertarget - T0_OFFSET));
    }
    return text;
}

char *storeRegister2Array(int rsrc, char *arrayName, int rindex) {
    char *text = (char *)malloc(MAXTXTLENGTH * sizeof(char));
    if (!isTempInRegister(rsrc - T0_OFFSET)) {
        strcat(text, loadLargeTemp(rsrc - T0_OFFSET, 0));
        rsrc = T8_OFFSET;
    }
    if (!isTempInRegister(rindex - T0_OFFSET)) {
        strcat(text, loadLargeTemp(rindex - T0_OFFSET, 1));
        rindex = T8_OFFSET + 1;
    }
    if (rindex < 0) {
        rindex = -rindex;
    }
    if (rsrc < 0) {
        rsrc = -rsrc;
    }
    int i = findVarible(presentFHB, arrayName);
    if (i >= 0) {
        SubFHB *tempSubFHB = presentFHB->subFHB + i;
        int offset = tempSubFHB->offset + VAR2SP;
        strcat(text, sllCommand(T8_OFFSET + 1, rindex, 2));
        strcat(text, subCommand(T8_OFFSET + 1, SP_OFFSET, T8_OFFSET + 1));
        strcat(text, swCommand(rsrc, -offset * byte2word, T8_OFFSET + 1));
    }
    else {
        int rposition = A0_OFFSET;
        strcat(text, laCommand(rposition, arrayName));
        strcat(text, sllCommand(T8_OFFSET + 1, rindex, 2));
        strcat(text, addCommand(rposition, rposition, T8_OFFSET + 1));
        strcat(text, swCommand(rsrc, 0, rposition));
    }
    return text;
}

char *endFunction() {
    char *text = (char *)malloc(MAXTXTLENGTH * sizeof(char));
    numpara = 0;
    strcat(text, "jr $ra\n");
    return text;
}

char *call(char *fname, int *regs) {
    char *text = (char *)malloc(MAXTXTLENGTH * sizeof(char));
    int stacklength = presentFHB->stacklength;
    int csblengh = getSymbleTableByName(fname)->csbLength;
    int num = csblengh - 1;
    /*
    for (int i = 0;i < csblengh;i++) {
        strcat(text, computeParameter());
    }
     */
    for (int i = 0;i < ((presentFHB->slength < S_NUM)? presentFHB->slength : S_NUM);i++) {
        strcat(text,swCommand(i + S0_OFFSET, -(i + S02SP) * byte2word, SP_OFFSET));
    }
    for (int i = 0;i < T_NUM;i++) {
        if (regs[i]) {
            strcat(text, swCommand(i + T0_OFFSET, -(i + T02SP) * byte2word, SP_OFFSET));
        }
    }
    for (int i = 0;i < csblengh;i++) {
        strcat(text, pushParameter(num));
        num--;
    }
    strcat(text, addiCommand(SP_OFFSET, SP_OFFSET, -stacklength * byte2word));
    strcat(text, swCommand(RA_OFFSET, 0, SP_OFFSET));
    strcat(text, "\n");
    strcat(text, jalCommand(fname));
    strcat(text, "\n");
    strcat(text, lwCommand(RA_OFFSET, 0, SP_OFFSET));
    strcat(text, addiCommand(SP_OFFSET, SP_OFFSET, stacklength * byte2word));
    for (int i = 0; i < ((presentFHB->slength < S_NUM)? presentFHB->slength : S_NUM); i++) {
        strcat(text, lwCommand(i + S0_OFFSET, -(i + S02SP) * byte2word, SP_OFFSET));
    }
    for (int i = 0; i < T_NUM; i++) {
        if (regs[i]) {
            strcat(text, lwCommand(i + T0_OFFSET, -(i + T02SP) * byte2word, SP_OFFSET));
        }
    }
    return text;
}

void setVariableType(char variableName[]) {
    variableType = MIPS_VARIBLE_TYPE_INT;
    SubFHB *subFhbptr = presentFHB->subFHB;
    int length = presentFHB->numVars;
    for (int i = 0;i < length;i++) {
        if (strcmp((subFhbptr + i)->name, variableName) == 0) {
            if ((subFhbptr + i)->dataType == DATATYPE_CHAR_VALUE
                || (subFhbptr + i)->dataType == DATATYPE_CHAR_ARRAY
                || (subFhbptr + i)->dataType == DATATYPE_CHAR_CONST) {
                variableType = MIPS_VARIBLE_TYPE_char;
                return;
            }
        }
    }
    FHB *fhbptr = getSymbleTableByName(variableName);
    if (fhbptr->dataType == DATATYPE_CHAR_VALUE
        || fhbptr->dataType == DATATYPE_CHAR_ARRAY
        || fhbptr->dataType == DATATYPE_CHAR_CONST) {
        variableType = MIPS_VARIBLE_TYPE_char;
        return;
    }
}

char *newLine() {
    char *text = (char *)malloc(MAXTXTLENGTH * sizeof(char));
    strcpy(text, laCommand(A0_OFFSET, "NL"));
    strcat(text, liCommand(V0_OFFSET, 4));
    strcat(text, "syscall\n");
    return text;
}

char *moveFromV0(int rtarget) {
    char *text = (char *)malloc(MAXTXTLENGTH * sizeof(char));
    //strcpy(text, loadVariable2register(name, rname));
    int rsrc = V0_OFFSET;
    int prertarget = rtarget;
    if (!isTempInRegister(rtarget - T0_OFFSET)) {
        rtarget = T8_OFFSET;
    }
    if (rtarget < 0) {
        rtarget = -rtarget;
    }
    strcat(text, moveCommand(rtarget, rsrc));
    if (!isTempInRegister(prertarget - T0_OFFSET)) {
        strcat(text, storeLargeTemp(prertarget - T0_OFFSET));
    }
    return text;
}

char *move2V0(int rsrc) {
    char *text = (char *)malloc(MAXTXTLENGTH * sizeof(char));
    //strcpy(text, loadVariable2register(name, rname));
    int rtarget = V0_OFFSET;
    int prersrc = rsrc;
    if (!isTempInRegister(rsrc - T0_OFFSET)) {
        rsrc = T8_OFFSET;
    }
    if (rsrc < 0) {
        rsrc = -rsrc;
    }
    strcat(text, moveCommand(rtarget, rsrc));
    return text;
}

char *inlineLoad(int rtarget, int rsrc) {

    char *text = (char *)malloc(MAXTXTLENGTH * sizeof(char));
    //strcpy(text, loadVariable2register(name, rname));
    int prertarget = rtarget;
    if (!isTempInRegister(rtarget - T0_OFFSET)) {
        rtarget = T8_OFFSET;
    }
    if (!isTempInRegister(rsrc - T0_OFFSET)) {

        strcat(text, loadLargeTemp(rsrc - T0_OFFSET, 1));
        rsrc = T8_OFFSET  + 1;
    }
    if (rsrc < 0) {
        rsrc = -rsrc;
    }
    if (rtarget < 0) {
        rtarget = -rtarget;
    }
    strcat(text, moveCommand(rtarget, rsrc));
    if (!isTempInRegister(prertarget - T0_OFFSET)) {
        strcat(text, storeLargeTemp(prertarget - T0_OFFSET));
    }
    return text;
}

char *addi(int rtarget, int rsrc, int value) {

    char *text = (char *)malloc(MAXTXTLENGTH * sizeof(char));
    //strcpy(text, loadVariable2register(name, rname));
    int prertarget = rtarget;
    if (!isTempInRegister(rtarget - T0_OFFSET)) {
        rtarget = T8_OFFSET;
    }
    if (!isTempInRegister(rsrc - T0_OFFSET)) {

        strcat(text, loadLargeTemp(rsrc - T0_OFFSET, 1));
        rsrc = T8_OFFSET  + 1;
    }
    if (rsrc < 0) {
        rsrc = -rsrc;
    }
    if (rtarget < 0) {
        rtarget = -rtarget;
    }
    strcat(text, addiCommand(rtarget, rsrc,value));
    if (!isTempInRegister(prertarget - T0_OFFSET)) {
        strcat(text, storeLargeTemp(prertarget - T0_OFFSET));
    }
    return text;
}

int isTempInRegister(int offset) {
    if (offset == V0_OFFSET - T0_OFFSET
     || offset == - T0_OFFSET || offset < 0) {
        return 1;
    }
    return offset < 8;
}

char *loadLargeTemp(int offset, int is2nd) {
    char *text = (char *)malloc(MAXTXTLENGTH * sizeof(char));
    SubFHB *subFhb = presentFHB->subTempFHB + offset;
    strcat(text,lwCommand(T8_OFFSET + is2nd, - (VAR2SP + subFhb->offset) * byte2word, SP_OFFSET));
    return text;
}

char *storeLargeTemp(int offset) {
    char *text = (char *)malloc(MAXTXTLENGTH * sizeof(char));
    SubFHB *subFhb = presentFHB->subTempFHB + offset;
    strcat(text,swCommand(T8_OFFSET, - (VAR2SP + subFhb->offset) * byte2word, SP_OFFSET));
    return text;
}

char *handleTransfore(char *rawstr) {
    char *text = (char *)malloc(MAXTXTLENGTH * sizeof(char));
    for (int i = 0,j = 0;rawstr[i] != '\0';i++, j++) {
        if (rawstr[i] == '\\') {
            text[j] = '\\';
            j++;
        }
        text[j] = rawstr[i];
        text[j + 1] = '\0';
    }
    return text;
}

char *pushParameter(int num) {
    char *text = (char *)malloc(MAXTXTLENGTH * sizeof(char));
    int stacklength = presentFHB->stacklength;
    int offset = stacklength + VAR2SP + num;
    int rsrc = getOffset2Rindex(pushlist->mcode.tempRight1, presentFHB) + T0_OFFSET;
    if (!isTempInRegister(rsrc - T0_OFFSET)) {
        strcat(text, loadLargeTemp(rsrc - T0_OFFSET, 0));
        rsrc = T8_OFFSET;
    }
    if (rsrc < 0) {
        rsrc = -rsrc;
    }
    pushlist = pushlist->nextnode;
    if (num < S_NUM) {
        strcat(text, moveCommand(S0_OFFSET + num, rsrc));
    }
    else {
        strcat(text, swCommand(rsrc, -offset * byte2word, SP_OFFSET));
    }
    return text;
}

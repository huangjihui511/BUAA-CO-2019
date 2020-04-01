//
// Created by 黄继辉 on 2019-11-22.
//

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include "Optimal.h"
#include "SymbleTable.h"
#include "GenerateMips.h"
#include "Printer.h"
#include "MemoAlloc.h"

McodeList *mcodeListHead;

extern int mcodeTableTop;

extern Mcode mcodeTable[MAXTXTLENGTH];

extern FHB symbleTable[MAX_NUM_OF_BZF];

extern int symbleTableIndexTable[MAX_NUM_OF_BZF];

extern int endlabelID;

extern int labelID;

int inlineCount = 0;

void linkAllMcode() {
    McodeList *premcodeListptr = NULL;
    McodeList *mcodeListptr;
    mcodeListHead = (McodeList*)malloc(sizeof(McodeList));
    mcodeListptr = mcodeListHead;
    for (int i = 0; i <= mcodeTableTop;i++) {
        mcodeListptr->mcode = mcodeTable[i];
        mcodeListptr->nextnode = NULL;
        if (premcodeListptr != NULL) {
            premcodeListptr->nextnode = mcodeListptr;
        }
        premcodeListptr = mcodeListptr;
        mcodeListptr = (McodeList*)malloc(sizeof(McodeList));
    }
    free(mcodeListptr);
}

/*
int linkPushMcode() {
    McodeList *mcodeListptr;
    mcodeListptr = mcodeListHead;
    while (mcodeListptr->nextnode != NULL) {
        if (mcodeListptr->nextnode->mcode.midType == MID_START_PUSH) {
            McodeList *mcodeListptr2 = mcodeListptr->nextnode->nextnode;
            while (mcodeListptr2 != NULL) {
                if (mcodeListptr2->mcode.midType == MID_START_PUSH) {
                    break;
                }
                if (mcodeListptr2->mcode.midType == MID_END_PUSH) {
                    McodeList *mcodeListptr3 = mcodeListptr->nextnode;
                    McodeList *mcodeListptr4;
                    while (mcodeListptr3->mcode.midType != MID_END_PUSH) {
                        if (mcodeListptr3->mcode.midType == MID_PUSH) {
                            if (mcodeListptr3->pushcontent == NULL) {
                                mcodeListptr3->pushcontent = mcodeListptr->nextnode;
                                mcodeListptr->nextnode = mcodeListptr3;
                                mcodeListptr = mcodeListptr3;
                                mcodeListptr4->nextnode = NULL;
                            }
                            else {
                                mcodeListptr = mcodeListptr3;
                            }
                        }
                        mcodeListptr4 = mcodeListptr3;
                        mcodeListptr3 = mcodeListptr3->nextnode;
                    }
                    mcodeListptr4->nextnode = mcodeListptr3->nextnode;
                    return 1;
                }
                mcodeListptr2 = mcodeListptr2->nextnode;
            }
        }
        mcodeListptr = mcodeListptr->nextnode;
    }
    return 0;
}


int isLocal(FHB *presentFHB, char *vname) {
    if (presentFHB == NULL) {
        return 0;
    }
    int i = findVarible(presentFHB, vname);
    return (i >= 0);
}
void makeNameUnique() {
    McodeList *mcodeListptr = NULL;
    mcodeListptr = mcodeListHead;
    char functionName[MAX_NAME_LENGTH] = {0};
    FHB *presentFHB = NULL;
    while (mcodeListptr != NULL) {
        int type = mcodeListptr->mcode.midType;
        if (type == MID_FUNC_VOID
            || type == MID_FUNC_CHAR
            || type == MID_FUNC_INT) {
            strcpy(functionName, mcodeListptr->mcode.nameLeft);
            presentFHB = getSymbleTableByName(functionName);
        }
        switch (type) {
            case MID_CON_CHAR:
            case MID_CON_INT:
            case MID_PARA_CHAR:
            case MID_PARA_INT:
            case MID_STORE_VAR:
            case MID_STORE_ARRAY:
            case MID_SCANF_PUSH: {
                char vname[MAX_NAME_LENGTH] = {0};
                strcpy(vname, mcodeListptr->mcode.nameLeft);
                if (isLocal(presentFHB,vname)) {
                    sprintf(mcodeListptr->mcode.nameLeft, "%s_%s",functionName, vname);
                }
                break;
            }
            case MID_READ_ARRAY:
            case MID_ASSIGH_VAR: {
                char vname[MAX_NAME_LENGTH] = {0};
                strcpy(vname, mcodeListptr->mcode.nameRight1);
                if (isLocal(presentFHB, vname)) {
                    sprintf(mcodeListptr->mcode.nameRight1, "%s_%s",functionName, vname);
                }
                break;
            }
            case MID_BNZ:
            case MID_BZ:
            case MID_GOTO:
            case MID_LABEL:
                {
                strcpy(mcodeListptr->mcode.nameLeft, functionName);
            }
        }
        mcodeListptr = mcodeListptr->nextnode;
    }
    mcodeListptr = mcodeListHead;
    while (mcodeListptr != NULL) {
        int type = mcodeListptr->mcode.midType;
        if (type == MID_FUNC_VOID
        || type == MID_FUNC_CHAR
        || type == MID_FUNC_INT) {
            strcpy(functionName, mcodeListptr->mcode.nameLeft);
            presentFHB = getSymbleTableByName(functionName);
            SubFHB *tempSubFhb = presentFHB->subFHB;
            int num = presentFHB->numVars;
            for (int j = 0;j<num;j++) {
                char tempName[MAX_NAME_LENGTH] = {0};
                strcpy(tempName, (tempSubFhb + j)->name);
                sprintf((tempSubFhb + j)->name, "%s_%s", functionName, tempName);
                //printf("%s\n",(tempSubFhb + j)->name);
            }
        }
        mcodeListptr = mcodeListptr->nextnode;
    }
}


McodeList *push2store(int r, char *variableName) {
    McodeList *mcodeListptr = (McodeList*)malloc(sizeof(McodeList));
    mcodeListptr->mcode.midType = MID_STORE_VAR;
    strcpy(mcodeListptr->mcode.nameLeft, variableName);
    mcodeListptr->mcode.tempRight1 = r;
    mcodeListptr->mcode.tempRight2 = allocTempVar();
    mcodeListptr->nextnode = NULL;
    recycleTempVar1step();
    return mcodeListptr;
}


McodeList *copyFunction(char *functionName, int offset) {
    McodeList *mcodeListptr1 = mcodeListHead;
    while (mcodeListptr1 != NULL) {
        if ((mcodeListptr1->mcode.midType == MID_FUNC_INT
             || mcodeListptr1->mcode.midType == MID_FUNC_CHAR
             || mcodeListptr1->mcode.midType == MID_FUNC_VOID)
            && (strcmp(mcodeListptr1->mcode.nameLeft,functionName) == 0)) {
            break;
        }
        mcodeListptr1 = mcodeListptr1->nextnode;
    }
    mcodeListptr1 = mcodeListptr1->nextnode;
    while (mcodeListptr1->mcode.midType == MID_PARA_CHAR
    || mcodeListptr1->mcode.midType == MID_PARA_INT) {
        mcodeListptr1 = mcodeListptr1->nextnode;
    }
    McodeList *mcodeListptrhead = (McodeList*)malloc(sizeof(McodeList));
    McodeList *mcodeListptrtemp = mcodeListptrhead;
    McodeList *mcodeListptrpre = NULL;
    while(mcodeListptr1->mcode.midType != MID_RET_VALUE
    && mcodeListptr1->mcode.midType != MID_RET_NOVALUE) {
        *mcodeListptrtemp = *mcodeListptr1;
        mcodeListptrtemp->mcode.tempLeft += offset;
        mcodeListptrtemp->mcode.tempRight1 += offset;
        mcodeListptrtemp->mcode.tempRight2 += offset;
        mcodeListptrtemp->mcode.indexRight += offset;
        mcodeListptrtemp->mcode.indexLeft += offset;
        mcodeListptrtemp->nextnode = NULL;
        if (mcodeListptrpre != NULL) {
            mcodeListptrpre->nextnode = mcodeListptrtemp;
        }
        mcodeListptrpre = mcodeListptrtemp;
        mcodeListptrtemp = (McodeList*)malloc(sizeof(McodeList));
        mcodeListptr1 = mcodeListptr1->nextnode;
    }
    mcodeListptrtemp = (McodeList*)malloc(sizeof(McodeList));
    mcodeListptrtemp->nextnode = NULL;
    mcodeListptrtemp->mcode.tempRight1 = mcodeListptrpre->mcode.tempLeft;
    mcodeListptrtemp->mcode.tempLeft = V0_OFFSET - T0_OFFSET;
    mcodeListptrtemp->mcode.midType = MID_MOVE;
    mcodeListptrpre->nextnode = mcodeListptrtemp;
    return mcodeListptrhead;
}

void mergeSymbleTable(FHB *parent, FHB *son, int offset) {
    SubFHB *sonStated = son->subFHB;
    int numSonStated = son->numVars;
    for (int i = 0;i < numSonStated;i++) {
        SubFHB *parentStated = parent->subFHB;
        int numParentStated = parent->numVars;
        int j;
        for (j = 0;j < numParentStated;j++) {
            if (strcmp((parentStated + j)->name, (sonStated + i)->name) == 0) {
                break;
            }
        }
        if (j == numParentStated) {
            *(parentStated + numParentStated) = *(sonStated + i);
            parent->numVars++;
        }
    }
    int numSonTemp = son->numTempVars + offset;
    int numParentTemp = parent->numTempVars;
    parent->numTempVars = (numSonTemp > numParentTemp) ? numSonTemp : numParentTemp;
}

void inlinefunction() {
    McodeList *mcodeListptr1 = mcodeListHead;
    McodeList *mcodeListptr2;
    FHB *parentFHB;
    while (mcodeListptr1->nextnode != NULL) {
        if ((mcodeListptr1->mcode.midType == MID_FUNC_INT
             || mcodeListptr1->mcode.midType == MID_FUNC_CHAR
             || mcodeListptr1->mcode.midType == MID_FUNC_VOID)
            && (strcmp(mcodeListptr1->mcode.nameLeft,"main") == 0)) {
            parentFHB = getSymbleTableByName("main");
        }
        if (mcodeListptr1->nextnode->mcode.midType == MID_PUSH) {
            mcodeListptr2 = mcodeListptr1;
            int numpush = 0;
            while (mcodeListptr2->mcode.midType != MID_CALL) {
                if (mcodeListptr2->mcode.midType == MID_PUSH) {
                    numpush++;
                }
                mcodeListptr2 = mcodeListptr2->nextnode;
            }
            if (isInline(mcodeListptr2->mcode.nameLeft)) {
                FHB *presentFHB = getSymbleTableByName(mcodeListptr2->mcode.nameLeft);
                int num = presentFHB->csbLength;
                if (num != numpush) {
                    mcodeListptr1 = mcodeListptr1->nextnode;
                    continue;
                }
                int offset = mcodeListptr2->mcode.tempLeft;
                mergeSymbleTable(parentFHB, presentFHB, offset);
                SubFHB *subFhb = presentFHB->subFHB;
                McodeList *mcodeListptr3 = mcodeListptr1->nextnode;
                for (int i = 0; i < num; i++) {
                    while (mcodeListptr3->mcode.midType != MID_PUSH) {
                        mcodeListptr1->nextnode = mcodeListptr3;
                        mcodeListptr1 = mcodeListptr1->nextnode;
                        mcodeListptr3 = mcodeListptr3->nextnode;
                    }
                    mcodeListptr1->nextnode = push2store(mcodeListptr3->mcode.tempLeft, subFhb->name);
                    subFhb++;
                    mcodeListptr1 = mcodeListptr1->nextnode;
                    mcodeListptr3 = mcodeListptr3->nextnode;
                }
                mcodeListptr1->nextnode = copyFunction(mcodeListptr2->mcode.nameLeft, offset);
                while (mcodeListptr1->nextnode != NULL) {
                    mcodeListptr1 = mcodeListptr1->nextnode;
                }
                mcodeListptr1->nextnode = mcodeListptr3->nextnode;
                continue;
            }
        }
        mcodeListptr1 = mcodeListptr1->nextnode;
    }
    return;
}

int checkDupicatedMove(char *str1, char *str2) {
    if (strncmp(str1, "move", 4) == 0 && strncmp(str2, "move", 4) == 0) {
        char t1[MAX_NAME_LENGTH];
        char t2[MAX_NAME_LENGTH];
        char t3[MAX_NAME_LENGTH];
        char t4[MAX_NAME_LENGTH];
        sscanf(str1, "move $%s $%s", t1, t2);
        sscanf(str2, "move $%s $%s", t3, t4);
        if (strncmp(t1, t4, 2) == 0) {
            if (t1[0] == 's' && strncmp(t2, t3, 2) == 0) {
                sprintf(str2, "");
                return 1;
            }
            if (t1[0] != 's' && strncmp(t2, t3, 2)) {
                sprintf(str1, "move $%s $%s",t3,t2);
                sprintf(str2, "");
                return 1;
            }
            if (t1[0] != 's' && !strncmp(t2, t3, 2)) {
                sprintf(str1, "");
                sprintf(str2, "");
                return 1;
            }
        }
    }
    return 0;
}

char *deleteDupicated(char *input) {
    char *token;
    char *pretoken;
    int isrevise = 1;
    while (isrevise) {
        char output[MAXTXTLENGTH] = {0};
        isrevise = 0;
        token = strtok(input, "\n");
        while (token != NULL) {
            pretoken = token;
            token = strtok(NULL, "\n");
            if (checkDupicatedMove(pretoken, token)) {
                isrevise = 1;
            }
            sprintf(output, "%s%s\n", output, pretoken);
        }
        token = (char *)malloc(MAXTXTLENGTH * sizeof(char));
        strcpy(token, output);
        strcpy(output, input);
        strcpy(input, token);
    }
    return input;
}
*/

int isInline(char *name) {
    McodeList *mcodeListptr1 = mcodeListHead;
    int statedlengh;
    int templengh;
    int csblength;
    int isjump = 0;
    int isglobal = 0;
    int isscanf = 0;
    while (mcodeListptr1->nextnode != NULL) {
        if ((mcodeListptr1->mcode.midType == MID_FUNC_INT
             || mcodeListptr1->mcode.midType == MID_FUNC_CHAR
             || mcodeListptr1->mcode.midType == MID_FUNC_VOID)
             && (strcmp(mcodeListptr1->mcode.nameLeft, name) == 0)) {
            FHB *presentFHB = getSymbleTableByName(mcodeListptr1->mcode.nameLeft);
            statedlengh = presentFHB->numVars;
            templengh = presentFHB->numTempVars;
            csblength = presentFHB->csbLength;
            McodeList *mcodeListptr2 = mcodeListptr1->nextnode;
            while (!(mcodeListptr2->mcode.midType == MID_FUNC_INT
                    || mcodeListptr2->mcode.midType == MID_FUNC_CHAR
                    || mcodeListptr2->mcode.midType == MID_FUNC_VOID)) {
                if (mcodeListptr2->mcode.midType == MID_CALL
                || mcodeListptr2->mcode.midType == MID_GOTO
                || mcodeListptr2->mcode.midType == MID_BZ
                   || mcodeListptr2->mcode.midType == MID_BNZ
                      || mcodeListptr2->mcode.midType == MID_INLINE_CALL
                   || mcodeListptr2->mcode.midType == MID_INLINE_CALL) {
                    isjump = 1;
                }
                if (mcodeListptr2->mcode.midType == MID_ASSIGH_VAR
                || mcodeListptr2->mcode.midType == MID_ASSIGH_ARRAY) {
                    char *varName = mcodeListptr2->mcode.nameRight1;
                    if (findVarible(presentFHB, varName) < 0) {
                        isglobal = 1;
                    }
                }
                if (mcodeListptr2->mcode.midType == MID_STORE_VAR
                    || mcodeListptr2->mcode.midType == MID_STORE_ARRAY) {
                    char *varName = mcodeListptr2->mcode.nameLeft;
                    if (findVarible(presentFHB, varName) < 0) {
                        isglobal = 1;
                    }
                }
                if (mcodeListptr2->mcode.midType == MID_SCANF_CALL) {
                    isscanf = 1;
                }
                mcodeListptr2 = mcodeListptr2->nextnode;
            }
            break;
        }
        mcodeListptr1 = mcodeListptr1->nextnode;
    }
    if (isglobal == 0
    && isjump == 0
    && csblength == statedlengh
    && isscanf == 0) {
        return 1;
        /*
        if (inlineCount < 100) {
            inlineCount++;
            if (csblength != 4) {
                return 1;
            }
        }
         */
    }
    return 0;
}

McodeList *copyFunction(char *functionName, int *newid, int *paraids) {
    McodeList *mcodeListptr = mcodeListHead;
    McodeList *copyHeadPtr = NULL;
    FHB *fhb = getSymbleTableByName(functionName);
    int label2newLabel[MAX_NUM_OF_BZF] = {0};
    while (mcodeListptr != NULL) {
        int type = mcodeListptr->mcode.midType;
        char *tempFunctionName = mcodeListptr->mcode.nameLeft;
        if ((type == MID_FUNC_INT
            ||type == MID_FUNC_CHAR
            || type == MID_FUNC_VOID)
            && strcmp(tempFunctionName, functionName) == 0) {
            mcodeListptr = mcodeListptr->nextnode;
            while (mcodeListptr->mcode.midType == MID_PARA_INT
            || mcodeListptr->mcode.midType == MID_PARA_CHAR) {
                mcodeListptr = mcodeListptr->nextnode;
            }
            copyHeadPtr = (McodeList*)malloc(sizeof(McodeList));
            *copyHeadPtr = *mcodeListptr;
            copyHeadPtr->nextnode = NULL;
            McodeList *tempPtr;
            McodeList *prePtr = copyHeadPtr;
            while (1) {
                if (mcodeListptr->mcode.midType == MID_END_FUNCTION) {
                    break;
                }
                mcodeListptr = mcodeListptr->nextnode;
                tempPtr = (McodeList*)malloc(sizeof(McodeList));
                *tempPtr = *mcodeListptr;
                prePtr->nextnode = tempPtr;
                prePtr = tempPtr;
                tempPtr->nextnode = NULL;
            }
            tempPtr = copyHeadPtr;
            while (tempPtr != NULL) {
                Mcode *mcode = &tempPtr->mcode;
                mcode->tempLeft = newid[mcode->tempLeft];
                mcode->tempRight1 = newid[mcode->tempRight1];
                mcode->tempRight2 = newid[mcode->tempRight2];
                mcode->indexLeft = newid[mcode->indexLeft];
                mcode->indexRight = newid[mcode->indexRight];
                mcode->isInline = 1;
                if (tempPtr->mcode.midType == MID_LABEL) {
                    labelID ++;
                    int oldLabel = tempPtr->mcode.numLeft;
                    tempPtr->mcode.numLeft = labelID;
                    label2newLabel[oldLabel] = labelID;
                }
                tempPtr = tempPtr->nextnode;
            }
            tempPtr = copyHeadPtr;
            while (tempPtr != NULL) {
                int type = tempPtr->mcode.midType;
                if (type == MID_ASSIGH_VAR) {
                    char *varName = tempPtr->mcode.nameRight1;
                    int paraOffset = findVarible(fhb, varName);
                    if (paraOffset >= 0) {
                        tempPtr->mcode.midType = MID_INLINE_LOAD;
                        tempPtr->mcode.tempRight1 = paraids[paraOffset];
                    }
                }
                if (type == MID_STORE_VAR) {
                    char *varName = tempPtr->mcode.nameLeft;
                    int paraOffset = findVarible(fhb, varName);
                    if (paraOffset >= 0) {
                        tempPtr->mcode.midType = MID_INLINE_STORE;
                        tempPtr->mcode.tempLeft = paraids[paraOffset];
                    }
                }
                if (type == MID_RET_VALUE) {
                    tempPtr->mcode.numLeft = endlabelID;
                    tempPtr->mcode.midType = MID_INLINE_RET_VALUE;
                }
                if (type == MID_RET_NOVALUE) {
                    tempPtr->mcode.midType = MID_INLINE_RET_NO_VALUE;
                }
                if (type == MID_END_FUNCTION) {
                    tempPtr->mcode.midType = MID_INLINE_END;
                    tempPtr->mcode.numLeft = endlabelID;
                    endlabelID++;
                }
                if (type == MID_BNZ
                || type == MID_BZ
                || type == MID_GOTO) {
                    int oldLabel = tempPtr->mcode.numRight1;
                    tempPtr->mcode.numRight1 = label2newLabel[oldLabel];
                }
                tempPtr = tempPtr->nextnode;
            }

            return copyHeadPtr;

        }
        mcodeListptr = mcodeListptr->nextnode;
    }
}
void inlinefunction() {
    McodeList *mcodeListptr = mcodeListHead;
    FHB *presentFHB = NULL;
    McodeList *pushPtr = NULL;
    McodeList *premcodeListptr = NULL;
    while (mcodeListptr != NULL) {
        int type = mcodeListptr->mcode.midType;
        if (type == MID_FUNC_VOID
            || type == MID_FUNC_CHAR
            || type == MID_FUNC_INT) {
            presentFHB = getSymbleTableByName(mcodeListptr->mcode.nameLeft);
        }
        if (type == MID_PUSH) {
            McodeList *tempPtr = (McodeList *)malloc(sizeof(McodeList));
            *tempPtr = *mcodeListptr;
            tempPtr->nextnode = pushPtr;
            pushPtr = tempPtr;
        }
        if (type == MID_CALL) {
            char *functionName = mcodeListptr->mcode.nameLeft;
            if (isInline(functionName)) {
                FHB *fhb = getSymbleTableByName(functionName);
                int templength = fhb->numTempVars;
                int csblength = fhb->csbLength;
                int *tempids = (int *)malloc(templength*sizeof(int));
                int *paraids = (int *)malloc(csblength*sizeof(int));
                for (int i = 0;i < csblength;i++) {
                    paraids[csblength - i - 1] = pushPtr->mcode.tempRight1;
                    pushPtr = pushPtr->nextnode;
                }
                for (int i = 0;i < templength;i++) {
                    tempids[i] = i + 1;
                }
                int *newid = addInlineTemp(presentFHB, templength, tempids);
                McodeList *copyHeadPtr = copyFunction(functionName,  newid, paraids);
                McodeList *tailPtr = copyHeadPtr;
                McodeList *pretailPtr = NULL;
                while (tailPtr->nextnode != NULL) {
                    pretailPtr = tailPtr;
                    tailPtr = tailPtr->nextnode;
                }
                tailPtr->inlineParaNum = csblength;
                tailPtr->inlineParaIndex = paraids;
                mcodeListptr->mcode.midType = MID_INLINE_CALL;
                if (pretailPtr->mcode.midType == MID_INLINE_RET_VALUE) {
                    McodeList *tailPtr2 = copyHeadPtr;
                    int index = mcodeListptr->nextnode->mcode.tempLeft;
                    while (tailPtr2->mcode.midType != MID_INLINE_END) {
                        if (tailPtr2->mcode.midType == MID_INLINE_RET_VALUE) {
                            tailPtr2->mcode.midType = MID_INLINE_MOVE;
                            tailPtr2->mcode.tempLeft = index;
                        }
                        tailPtr2 = tailPtr2->nextnode;
                    }
                    tailPtr->nextnode = mcodeListptr->nextnode->nextnode;
                } else {
                    tailPtr->nextnode = mcodeListptr->nextnode;
                }
                mcodeListptr->nextnode = copyHeadPtr;
                //printf("inline %s\n", functionName);
                mcodeListptr = tailPtr;
            }
            else {
                FHB *fhb = getSymbleTableByName(functionName);
                int csblength = fhb->csbLength;
                for (int i = 0;i < csblength;i++) {
                    pushPtr = pushPtr->nextnode;
                }
            }
        }
        premcodeListptr = mcodeListptr;
        mcodeListptr = mcodeListptr->nextnode;
    }
}

void allocVariableLength(FHB *symbleptr, int *countalloc) {
    int l = 0;
    int s = 0;
    for (int i = 0;i < symbleptr->numVars;i++) {
        SubFHB *subFhb = symbleptr->subFHB + i;
        if ((subFhb->dataType == DATATYPE_CHAR_CONST
             || subFhb->dataType == DATATYPE_CHAR_VALUE
             || subFhb->dataType == DATATYPE_INT_VALUE
             || subFhb->dataType == DATATYPE_INT_CONST) ) {
            /*
            if (s < S_NUM) {
                subFhb->isInRegister = 1;
                subFhb->rName = s + S0_OFFSET;
                s++;
            }
            */
            if (countalloc[i] != -1) {
                subFhb->isInRegister = 1;
                subFhb->rName = countalloc[i] + S0_OFFSET;
                s++;
            }

        }
        subFhb->offset = l;
        l += subFhb->length;
    }
    symbleptr->slength = s;
    s = 0;
    for (int i = 0;i < symbleptr->numTempVars;i++) {
        SubFHB *subFhb = symbleptr->subTempFHB + i;
        subFhb->offset = l;
        s = symbleptr->offset2Reg[i + 1];
        if (s < T_NUM) {
            subFhb->isInRegister = 1;
            subFhb->rName = s + T0_OFFSET;
            s++;
        }
        l += subFhb->length;
    }
    symbleptr->stacklength = l + VAR2SP;
    symbleptr->tlength = symbleptr->numTempVars;
}

int selectType(int type) {
    switch (type) {
        case MID_END_FUNCTION:
        case MID_RET_NOVALUE:
        case MID_RET_VALUE:
        case MID_PARA_CHAR:
        case MID_PARA_INT:
        case MID_ASSIGH_VAR:
        case MID_LESS:
        case MID_EQUAL_LESS:
        case MID_GREATER:

        case MID_EQUAL_GREATER:
        case MID_EQUAL:
        case MID_NOT_EQUAL:

        case MID_SCANF_PUSH:
        case MID_PRINTF_PUSH_VAR:
        case MID_NEWLINE:
        case MID_INLINE_END:
        case MID_MOVE_V0:
        case MID_INLINE_LOAD:
        case MID_INLINE_CALL:
        case MID_INLINE_STORE:
        case MID_SCANF_CALL:
        case MID_PRINTF_CALL:
        case MID_CALL:
        case MID_LABEL:
        case MID_PRINTF_PUSH_STR:
        case MID_ASSIGH_CHAR:
        case MID_ASSIGH_INT:
        case MID_MINUS:
        case MID_ASSIGH_OPPOSE:
        case MID_PUSH:
        case MID_DIVI:
        case MID_MULT:
        case MID_STORE_VAR:
        case MID_ADD: {
            return 1;
        }

        default: {
            return 1;
        }
    }
}

void removeInlineLoad() {
    McodeList *mcodeListptr = mcodeListHead;
    McodeList *premcodeListptr = NULL;
    FHB *presentFHB = NULL;
    while (mcodeListptr != NULL) {
        int type = mcodeListptr->mcode.midType;
        switch (type) {
            case MID_INLINE_LOAD: {
                int tsrc = mcodeListptr->mcode.tempRight1;
                int ttarget = mcodeListptr->mcode.tempLeft;
                McodeList *tempmcodeListptr = mcodeListptr->nextnode;
                while (tempmcodeListptr->mcode.midType != MID_INLINE_END) {
                    int flag = 0;
                    if (tempmcodeListptr->mcode.tempRight1 == ttarget) {
                        tempmcodeListptr->mcode.tempRight1 = tsrc;
                        flag = 1;
                    }
                    if (tempmcodeListptr->mcode.tempRight2 == ttarget) {
                        tempmcodeListptr->mcode.tempRight2 = tsrc;
                        flag = 1;
                    }
                    if (flag) {
                        premcodeListptr->nextnode = mcodeListptr->nextnode;
                        mcodeListptr = premcodeListptr;
                        break;
                    }
                    tempmcodeListptr = tempmcodeListptr->nextnode;
                }
            }
        }
        premcodeListptr = mcodeListptr;
        mcodeListptr = mcodeListptr->nextnode;
    } // must
}

void removeLi() {
    McodeList *mcodeListptr = mcodeListHead;
    McodeList *premcodeListptr = NULL;
    FHB *presentFHB = NULL;
    while (mcodeListptr != NULL) {
        int type = mcodeListptr->mcode.midType;
        switch (type) {
            //case MID_ASSIGH_CHAR:
            case MID_ASSIGH_INT: {
                int value = mcodeListptr->mcode.numRight1;
                int ttarget = mcodeListptr->mcode.tempLeft;
                McodeList *tempmcodeListptr = mcodeListptr->nextnode;
                McodeList *temppremcodeListptr = mcodeListptr;
                while (tempmcodeListptr->mcode.midType != MID_END_FUNCTION) {
                    int temptype = tempmcodeListptr->mcode.midType;
                    int src1 = tempmcodeListptr->mcode.tempRight1;
                    int src2 = tempmcodeListptr->mcode.tempRight2;
                    int flag = 0;

                    switch (temptype) {

                        case MID_ADD:{

                            if (src1 == ttarget || src2 == ttarget) {
                                if (src1 == ttarget) {
                                    tempmcodeListptr->mcode.tempRight1 = tempmcodeListptr->mcode.tempRight2;
                                }
                                tempmcodeListptr->mcode.numRight1 = value;
                                tempmcodeListptr->mcode.midType = MID_ADDI;
                                premcodeListptr->nextnode = mcodeListptr->nextnode;
                                mcodeListptr = premcodeListptr;
                                flag = 1;
                            }
                            break;
                        }
                        case MID_MINUS:{

                            if (src2 == ttarget) {

                                value = -value;
                                tempmcodeListptr->mcode.numRight1 = value;
                                tempmcodeListptr->mcode.midType = MID_ADDI;
                                premcodeListptr->nextnode = mcodeListptr->nextnode;
                                mcodeListptr = premcodeListptr;
                                flag = 1;
                            }
                            break;
                        }



                        case MID_EQUAL: {
                            if (value == 0) {

                            }
                            if (value == 0 && src2 == ttarget) {
                                temppremcodeListptr->nextnode = tempmcodeListptr->nextnode;
                                //tempmcodeListptr->mcode.tempRight2 = 0 - T0_OFFSET;
                                //tempmcodeListptr->mcode.midType = MID_MINUS;
                                tempmcodeListptr = tempmcodeListptr->nextnode;
                                int temptype2 = tempmcodeListptr->mcode.midType;
                                if (temptype2 == MID_BNZ) {
                                    tempmcodeListptr->mcode.midType = MID_BNZ0;
                                    tempmcodeListptr->mcode.tempRight1 = src1;
                                }
                                if (temptype2 == MID_BZ) {
                                    tempmcodeListptr->mcode.midType = MID_BZ0;
                                    tempmcodeListptr->mcode.tempRight1 = src1;
                                }
                                premcodeListptr->nextnode = mcodeListptr->nextnode;
                                mcodeListptr = premcodeListptr;
                                flag = 1;
                            }
                            break;
                        }
                        default: {

                            if (tempmcodeListptr->mcode.tempLeft == ttarget) {
                                flag = 1;
                            }
                        }

                    }
                    if (flag) {
                        break;
                    }
                    temppremcodeListptr = tempmcodeListptr;
                    tempmcodeListptr = tempmcodeListptr->nextnode;
                }
                break;
            }
        }
        premcodeListptr = mcodeListptr;
        mcodeListptr = mcodeListptr->nextnode;
    } // must
}

void removeSTOREmove() {
    McodeList *mcodeListptr = mcodeListHead;
    McodeList *premcodeListptr = NULL;
    FHB *presentFHB = NULL;
    while (mcodeListptr != NULL) {
        int type = mcodeListptr->mcode.midType;
        switch (type) {
            case MID_FUNC_VOID:
            case MID_FUNC_CHAR:
            case MID_FUNC_INT: {
                presentFHB = getSymbleTableByName(mcodeListptr->mcode.nameLeft);
                break;
            }
            case MID_STORE_VAR: {
                char *varName = mcodeListptr->mcode.nameLeft;
                int i = findVarible(presentFHB, varName);
                if (i >= 0) {
                    SubFHB *tempSubFHB = presentFHB->subFHB + i;
                    if (tempSubFHB->isInRegister) {
                        int sname = tempSubFHB->rName;
                        int tname1 = premcodeListptr->mcode.tempLeft;
                        int tname2 = mcodeListptr->mcode.tempRight1;
                        if (tname1 == tname2) {
                            premcodeListptr->mcode.tempLeft = -sname;
                            premcodeListptr->nextnode = mcodeListptr->nextnode;
                            mcodeListptr = premcodeListptr;
                            continue;
                        }
                    }
                }
                break;
            }
        }
        premcodeListptr = mcodeListptr;
        mcodeListptr = mcodeListptr->nextnode;
    } // must
}

void removeJump() {
    McodeList *mcodeListptr = mcodeListHead;
    McodeList *premcodeListptr = NULL;
    while (mcodeListptr != NULL) {
        int type = mcodeListptr->mcode.midType;
        switch (type) {
            case MID_GOTO: {
                int gotoId = mcodeListptr->mcode.numRight1;
                McodeList *tempmcodeListptr = mcodeListHead;
                McodeList *startposition = NULL;
                McodeList *endposition = NULL;
                while (tempmcodeListptr != mcodeListptr) {
                    if (tempmcodeListptr->mcode.midType == MID_START_LOOP) {
                        while (tempmcodeListptr->nextnode->mcode.midType != MID_LABEL) {
                            tempmcodeListptr = tempmcodeListptr->nextnode;
                        }
                        if (tempmcodeListptr->nextnode->mcode.numLeft == gotoId) {
                            startposition = tempmcodeListptr->nextnode->nextnode;
                            endposition = startposition;
                            while (endposition->mcode.midType != MID_BNZ) {
                                endposition = endposition->nextnode;
                            }
                            tempmcodeListptr->nextnode->nextnode = endposition->nextnode;
                            endposition->nextnode = tempmcodeListptr->nextnode;
                            tempmcodeListptr->nextnode = startposition;
                            McodeList *tempmcodeListptr2 = NULL;

                            while (startposition != endposition->nextnode) {
                                tempmcodeListptr2 = (McodeList*)malloc(sizeof(McodeList));
                                *tempmcodeListptr2 = *startposition;
                                premcodeListptr->nextnode = tempmcodeListptr2;
                                premcodeListptr = premcodeListptr->nextnode;
                                startposition = startposition->nextnode;
                            }
                            tempmcodeListptr2->nextnode = mcodeListptr->nextnode;
                            tempmcodeListptr2->mcode.midType = MID_BZ;
                            tempmcodeListptr2->mcode.numRight1 = gotoId;
                            mcodeListptr = tempmcodeListptr2;
                            break;
                        }
                    }
                    tempmcodeListptr = tempmcodeListptr->nextnode;
                }
                break;
            }
        }
        premcodeListptr = mcodeListptr;
        mcodeListptr = mcodeListptr->nextnode;
    } // must
}

void optimalMain() {
    linkAllMcode(); // must
    //while(linkPushMcode()) {} // must
    FHB *presentFHB = NULL;
    //makeNameUnique();
    printMcode();
    removeJump();
    inlinefunction();
    McodeList *mcodeListptr = mcodeListHead;
    while (mcodeListptr != NULL) {
        int type = mcodeListptr->mcode.midType;
        if (type == MID_FUNC_VOID
            || type == MID_FUNC_CHAR
            || type == MID_FUNC_INT) {
            presentFHB = getSymbleTableByName(mcodeListptr->mcode.nameLeft);
            int *countalloc = countAlloc(presentFHB, mcodeListptr);
            allocTempVariable(mcodeListptr, presentFHB);
            allocVariableLength(presentFHB, countalloc);
            removeUnnecessaryMove(presentFHB, mcodeListptr);
        }
        mcodeListptr = mcodeListptr->nextnode;
    } // must
    removeInlineLoad();


    removeLi();
    removeSTOREmove();
    char temptext[MAXTXTLENGTH] = {0};
    scanMidCode();
    //strcpy(temptext, scanMidCode());
    //strcpy(temptext, deleteDupicated(temptext));
    //printMips(temptext);
}

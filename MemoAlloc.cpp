//
// Created by 黄继辉 on 2019-11-27.
//

#include <cstdio>
#include <cstdlib>
#include "Optimal.h"
#include "MemoAlloc.h"
#include "Printer.h"
#include "GenerateMips.h"

extern McodeList *mcodeListHead;

int regs[MAX_NUM_OF_BZF] = {0};

McodeList *pushMcodeHead = NULL;

void allocTempVariable(McodeList *funcHeadPtr, FHB *presentFHB) {
    McodeList *mcodeListptr = funcHeadPtr->nextnode;
    McodeList *premcodeListptr = mcodeListptr;
    int *offset2Reg = (int *)malloc(MAX_NUM_OF_BZF * sizeof(int));
    while (mcodeListptr != NULL) {
        int type = mcodeListptr->mcode.midType;
        switch (type) {
            case MID_BNZ:
            case MID_BZ:
            case MID_GOTO:
            case MID_LABEL:
            case MID_RET_VALUE:
            case MID_END_FUNCTION:
            case MID_RET_NOVALUE: {
                McodeList *headMcodePtr = premcodeListptr;
                McodeList *tailMcodePtr = mcodeListptr->nextnode;
                //printf("start block\n");
                while (headMcodePtr != tailMcodePtr) {
                    Mcode *mcodeptr = &headMcodePtr->mcode;
                    switch (mcodeptr->midType) {
                        case MID_INLINE_CALL: {
                            allocInlineCallMcode(headMcodePtr, offset2Reg);
                            break;
                        }
                        case MID_CALL: {
                            allocCallMcode(headMcodePtr, offset2Reg);
                            break;
                        }
                        case MID_PUSH: {
                            allocPushMcode(headMcodePtr, offset2Reg);
                            break;
                        }
                        case MID_INLINE_LOAD: {
                            //printf("%s", mcode2string(mcodeptr));
                            allocReg(headMcodePtr->mcode.tempLeft, offset2Reg);
                            break;
                        }

                        case MID_INLINE_STORE: {
                            printf("%s", mcode2string(mcodeptr));
                            recycleReg(headMcodePtr->mcode.tempRight1, offset2Reg);
                            break;
                        }
                        case MID_INLINE_END: {
                            int inlineParaNum = headMcodePtr->inlineParaNum;
                            int *inlineParaIndex = headMcodePtr->inlineParaIndex;
                            for (int i = 0;i < inlineParaNum;i++) {
                                recycleReg(*(inlineParaIndex + i), offset2Reg);
                            }
                            break;
                        }
                        default: {
                            allocMcode(mcodeptr, offset2Reg);
                        }
                    }
                    headMcodePtr = headMcodePtr->nextnode;
                }
                for (int i = 0;i < MAX_NUM_OF_BZF;i++) {
                    regs[i] = 0;
                }
                premcodeListptr = mcodeListptr;
            }
        }
        mcodeListptr = mcodeListptr->nextnode;
        if (type == MID_END_FUNCTION) {
            break;
        }
    }
    for (int i = 0;i <= presentFHB->numTempVars;i++) {
        presentFHB->offset2Reg[i] = offset2Reg[i];
    }
}

void allocReg(int index, int *offset2Reg) {
    if (index > 0) {
        for (int i = 0;i < MAX_NUM_OF_BZF;i++) {
            if (regs[i] == 0) {
                offset2Reg[index] = i;
                regs[i] = 1;
                //printf("alloc T%d to t%d\n", offset2Reg[index], index);
                return;
            }
        }
    }
}

void allocInlineReg(int index, int *offset2Reg) {
    if (index > 0) {
        for (int i = 0;i < MAX_NUM_OF_BZF;i++) {
            if (regs[i] == 0) {
                offset2Reg[index] = i;
                regs[i] = 2; // not recycle
                //printf("alloc T%d to t%d\n", offset2Reg[index], index);
                return;
            }
        }
    }
}

void recycleReg(int index, int *offset2Reg) {
    if (index > 0) {
        if (regs[offset2Reg[index]] == 1) {
            regs[offset2Reg[index]] = 0;
            //printf("recycle T%d from t%d\n", offset2Reg[index], index);
        }
    }
}

int getOffset2Rindex(int offset, FHB *presentFHB) {
    if (offset <= - S0_OFFSET) {
        return offset - T0_OFFSET;
    }
    if (offset == - T0_OFFSET) {
        return offset;
    }
    if (offset > 0) {
        return presentFHB->offset2Reg[offset];
    }
}

void allocMcode(Mcode *mcodeptr, int *offset2Reg) {
    //printf("%s", mcode2string(mcodeptr));
    allocReg(mcodeptr->tempLeft, offset2Reg);
    recycleReg(mcodeptr->tempRight1, offset2Reg);
    recycleReg(mcodeptr->tempRight2, offset2Reg);
    recycleReg(mcodeptr->indexRight, offset2Reg);
    recycleReg(mcodeptr->indexLeft, offset2Reg);
}

void allocPushMcode(McodeList *mcodeListPtr, int *offset2Reg) {
    McodeList *tempPtr = (McodeList *)malloc(sizeof(McodeList));
    *tempPtr = *mcodeListPtr;
    tempPtr->nextnode = pushMcodeHead;
    pushMcodeHead = tempPtr;
}

void allocCallMcode(McodeList *mcodeListPtr, int *offset2Reg) {
    char *name = mcodeListPtr->mcode.nameLeft;
    FHB *fhbPtr = getSymbleTableByName(name);
    int length = fhbPtr->csbLength;
    for (int i = 0;i < length;i++) {
        allocMcode(&pushMcodeHead->mcode, offset2Reg);
        pushMcodeHead = pushMcodeHead->nextnode;
    }
    for (int i = 0;i < T_NUM;i++) {
        mcodeListPtr->regs[i] = regs[i];
    }
}

void allocInlineCallMcode(McodeList *mcodeListPtr, int *offset2Reg) {
    char *name = mcodeListPtr->mcode.nameLeft;
    FHB *fhbPtr = getSymbleTableByName(name);
    int length = fhbPtr->csbLength;
    for (int i = 0;i < length;i++) {
        //allocMcode(&pushMcodeHead->mcode, offset2Reg);
        pushMcodeHead = pushMcodeHead->nextnode;
    }
}

void removeUnnecessaryMove(FHB *presentFHB, McodeList *mcodeListPtr) {
    McodeList *tempPtr = mcodeListPtr;
    while (tempPtr->nextnode->mcode.midType != MID_END_FUNCTION) {
        McodeList *tempNextPtr = tempPtr->nextnode;
        if (tempNextPtr->mcode.midType == MID_ASSIGH_VAR) {
            char *varName = tempNextPtr->mcode.nameRight1;
            int i = findVarible(presentFHB, varName);
            if (i >= 0) {
                SubFHB *tempSubFHB = presentFHB->subFHB + i;
                if (tempSubFHB->isInRegister) {
                    int sname = tempSubFHB->rName;
                    int tname = tempNextPtr->mcode.tempLeft;
                    if (changeTname2Sname(presentFHB, tempNextPtr, sname, tname)) {
                        tempPtr->nextnode = tempNextPtr->nextnode;
                        continue;
                    }
                }
            }
        }
        if (0) {
        //if (tempNextPtr->mcode.midType == MID_STORE_VAR) {
            char *varName = tempNextPtr->mcode.nameLeft;
            int i = findVarible(presentFHB, varName);
            if (i >= 0) {
                SubFHB *tempSubFHB = presentFHB->subFHB + i;
                if (tempSubFHB->isInRegister) {
                    int sname = tempSubFHB->rName;
                    int tname1 = tempPtr->mcode.tempLeft;
                    int tname2 = tempNextPtr->mcode.tempRight1;
                    if (tname1 == tname2) {
                        tempPtr->mcode.tempLeft = -sname;
                        tempPtr->nextnode = tempNextPtr->nextnode;
                        continue;
                    }
                }
            }
        }
        tempPtr = tempPtr->nextnode;
    }
}

int changeTname2Sname(FHB *presentFHB, McodeList *mcodeListPtr, int sname, int tname) {
    McodeList *tempPtr = mcodeListPtr;
    while (tempPtr->nextnode != NULL){
        tempPtr = tempPtr->nextnode;
        int temp1 = tempPtr->mcode.tempRight1;
        int temp2 = tempPtr->mcode.tempRight2;
        int indexRight = tempPtr->mcode.indexRight;
        int indexLeft = tempPtr->mcode.indexLeft;
        int type = tempPtr->mcode.midType;
        switch (type) {
            case MID_ADD:
            case MID_MINUS:
            case MID_MULT:
            case MID_DIVI:
            case MID_EQUAL:
            case MID_LESS:
            case MID_GREATER:
            case MID_EQUAL_GREATER:
            case MID_EQUAL_LESS:
            case MID_NOT_EQUAL: {
                if (temp1 == tname) {
                    tempPtr->mcode.tempRight1 = -sname;
                    return 1;
                }
                if (temp2 == tname) {
                    tempPtr->mcode.tempRight2 = -sname;
                    return 1;
                }
            }
            case MID_RET_VALUE:
            case MID_PRINTF_PUSH_VAR:
            case MID_STORE_VAR:
            case MID_MOVE_V0:
            //case MID_PUSH:
            case MID_ASSIGH_OPPOSE: {
                if (temp1 == tname) {
                    tempPtr->mcode.tempRight1 = -sname;
                    return 1;
                }
            }
            case MID_READ_ARRAY: {
                if (indexRight == tname) {
                    tempPtr->mcode.indexRight = -sname;
                    return 1;
                }
            }
            //case MID_INLINE_LOAD:
            case MID_STORE_ARRAY: {
                if (indexLeft == tname) {
                    tempPtr->mcode.indexLeft = -sname;
                    return 1;
                }
                if (temp1 == tname) {
                    tempPtr->mcode.tempRight1 = -sname;
                    return 1;
                }
            }
            case MID_INLINE_CALL: {
                return 0;
            }

        }
    }
    printf("not find\n");
    return 0;
}
/*
McodeList *allocPushMcode(McodeList *mcodeListPtr, int *offset2Reg) {
    McodeList *headmcodeListPtr = mcodeListPtr;
    McodeList *stack = NULL;
    while (headmcodeListPtr->mcode.midType == MID_PUSH) {
        McodeList *tempPtr = (McodeList *)malloc(sizeof(McodeList));
        *tempPtr = *headmcodeListPtr;
        tempPtr->nextnode = stack;
        stack = tempPtr;
        headmcodeListPtr = headmcodeListPtr->nextnode;
    }
    McodeList *stack2 = NULL;
    while (stack != NULL) {
        McodeList *tempPushPtr = stack->pushcontent;
        while (tempPushPtr != NULL) {
            allocMcode(&tempPushPtr->mcode, offset2Reg);
            tempPushPtr = tempPushPtr->nextnode;
        }
        McodeList *tempPtr = stack;
        stack = stack->nextnode;
        tempPtr->nextnode = stack2;
        stack2 = tempPtr;
    }
    while (stack2!=NULL) {
        recycleReg(stack2->mcode.tempRight1, offset2Reg);
        stack2 = stack2->nextnode;
    }
    return headmcodeListPtr;
}
*/

int *countAlloc(FHB *fhb, McodeList *mptr) {
    int inloop = 1;
    int num = fhb->numVars;
    int csnum = fhb->csbLength;
    int weight[num];
    int assignPosi[num];
    int storePosi[num];
    int startPosi[num];
    int endPosi[num];
    int *super = (int *)malloc(num * sizeof(int));
    for (int i = 0;i < num;i++) {
        weight[i] = 0;
        super[i] = -1;
        assignPosi[i] = -1;
        storePosi[i] = -1;
        startPosi[i] = 0;
        endPosi[i] = 0;
    }
    int counter = 0;
    while (mptr->mcode.midType != MID_END_FUNCTION) {
        int type = mptr->mcode.midType;
        switch (type) {
            case MID_START_LOOP: {
                inloop *= 100;
                break;
            }
            case MID_END_LOOP: {
                inloop /= 100;
                break;
            }

            case MID_ASSIGH_VAR: {
                char *name = mptr->mcode.nameRight1;
                int index = findVarible(fhb, name);
                if (index >= 0) {
                    weight[index] += inloop;
                    if (assignPosi[index] == -1) {
                        assignPosi[index] = counter;
                    }
                    storePosi[index] = counter;
                }
                break;
            }
            case MID_CONST_ASSIGH:
            case MID_PARA_INT:
            case MID_PARA_CHAR:
            case MID_STORE_VAR: {
                char *name = mptr->mcode.nameLeft;
                int index = findVarible(fhb, name);
                if (index >= 0) {
                    weight[index] += inloop;
                    storePosi[index] = counter;
                    if (assignPosi[index] == -1) {
                        assignPosi[index] = counter;
                    }
                }
                break;
            }
        }
        mptr = mptr->nextnode;
        counter++;
    }
    for (int i = 0;i < csnum;i++) {
        super[i] = i;
        startPosi[i] = assignPosi[i];
        endPosi[i] = startPosi[i];
    }
    for (int i = csnum;i < num && i < S_NUM;i++) {
        int maxvalue = -1;
        int maxindex = 0;
        for (int j = 0;j < num;j++) {
            if (weight[j] > maxvalue
            && super[j] == -1) {
                maxvalue = weight[j];
                maxindex = j;
            }
        }
        if (maxvalue <= 0) {
            break;
        }
        super[maxindex] = i;
        startPosi[i] = assignPosi[maxindex];
        //printf("%d %d\n",startPosi[i], storePosi[maxindex]);
        endPosi[i] = storePosi[maxindex];
    }

    for (int i = S_NUM;i < num;i++) {
        int maxvalue = -1;
        int maxindex = 0;
        for (int j = 0;j < num;j++) {
            if (weight[j] > maxvalue
                && super[j] == -1) {
                maxvalue = weight[j];
                maxindex = j;
            }
        }
        if (maxvalue <= 0) {
            break;
        }
        for (int j = 0;j < S_NUM;j++) {
            if (startPosi[j] > storePosi[maxindex]) {
                printf("1 %d %d\n",startPosi[j], storePosi[maxindex]);
                startPosi[j] = assignPosi[maxindex];
                super[maxindex] = j;
                break;
            }
            if (endPosi[j] < assignPosi[maxindex]) {
                printf("2 %d %d\n",endPosi[j], assignPosi[maxindex]);
                endPosi[j] = storePosi[maxindex];
                super[maxindex] = j;
                break;
            }
        }
    }

    for (int i = 0;i < num;i++) {
        printf("alloc %s s%d : %d\n", (fhb->subFHB + i)->name, super[i], weight[i]);
    }
    return super;
}
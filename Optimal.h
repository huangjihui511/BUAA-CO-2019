//
// Created by 黄继辉 on 2019-11-22.
//

#ifndef OPTIMAL_OPTIMAL_H
#define OPTIMAL_OPTIMAL_H

#include "MiddleCode.h"
#include "SymbleTable.h"


typedef struct struct8 {
    Mcode mcode;
    struct8 *nextnode = nullptr;
    int regs[8];
    int inlineParaNum = 0;
    int *inlineParaIndex;
} McodeList;

typedef struct struct9 {
    char parentname[MAX_NAME_LENGTH];
    char sonname[MAX_NAME_LENGTH];
} InlineTable;

int isLocal(FHB *presentFHB, char *vname);

void optimalMain();

void linkAllMcode();

int linkPushMcode();

void allocVariableLength(FHB *symbleptr, int *countalloc);

void makeNameUnique();

void inlinefunction();

void mergeSymbleTable(FHB *parent, FHB *son, int offset);

McodeList *copyFunction(char *functionName, int offset);

int checkDupicatedMove(char *str1, char *str2);

char *deleteDupicated(char *input);

int isInline(char *name);

void removeInlineLoad();

void removeLi();

#endif //OPTIMAL_OPTIMAL_H

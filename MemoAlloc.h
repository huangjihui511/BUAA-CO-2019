//
// Created by 黄继辉 on 2019-11-27.
//

#ifndef OPTIMAL_MEMOALLOC_H
#define OPTIMAL_MEMOALLOC_H

#include "Optimal.h"


void allocTempVariable(McodeList *funcHeadPtr, FHB *presentFHB);

void allocReg(int index, int *offset2Reg);

void allocInlineReg(int index, int *offset2Reg);

void recycleReg(int index, int *offset2Reg);

int getOffset2Rindex(int offset, FHB *presentFHB);

void allocMcode(Mcode *mcodeptr, int *offset2Reg);

void allocPushMcode(McodeList *mcodeListPtr, int *offset2Reg);

void allocCallMcode(McodeList *mcodeListPtr, int *offset2Reg);

void removeUnnecessaryMove(FHB *presentFHB, McodeList *mcodeListPtr);

int changeTname2Sname(FHB *presentFHB, McodeList *mcodeListPtr, int sname, int tname);

void allocInlineCallMcode(McodeList *mcodeListPtr, int *offset2Reg);

void removeSTOREmove();

int *countAlloc(FHB *fhb, McodeList *mptr);

#endif //OPTIMAL_MEMOALLOC_H

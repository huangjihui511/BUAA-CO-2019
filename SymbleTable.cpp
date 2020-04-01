//
// Created by 黄继辉 on 2019-10-30.
//

#include "LexicalAnalyzer.h"
#include "SymbleTable.h"
#include "Printer.h"
#include "ErrorHandle.h"
#include "MiddleCode.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

extern Token tokens[MAXTXTLENGTH];
extern int tokenstop;
extern int symbol;
extern int isCSB;

int hashPositionTable[HASH_LENGTH + 1];

int symbleTableIndexTable[MAX_NUM_OF_BZF];

FHB symbleTable[MAX_NUM_OF_BZF];

int symbleTableIndexTop = 0;

SubFHB *subFHB = NULL;
SubFHB *subTempFHB = NULL;

void insertSymbleTable(char n[MAXTXTLENGTH], int dt, int cl) {
    checkError(ERROR_TYPE_B,0);
    int hash = hashSymbleTable(n);
    int prePos = hashPositionTable[hash];
    symbleTableIndexTable[symbleTableIndexTop] += 1;
    int pos = symbleTableIndexTable[symbleTableIndexTop];
    hashPositionTable[hash] = pos;
    strcpy(symbleTable[pos].name,n);
    symbleTable[pos].dataType = dt;
    symbleTable[pos].csbLength = cl;
    symbleTable[pos].nextnode = prePos;
    //printf("INSERT %s %d AT %d\n",n,dt,pos);
    if (isCSB) {
        symbleTable[symbleTableIndexTable[symbleTableIndexTop-1]].paraType[symbleTable[symbleTableIndexTable[symbleTableIndexTop-1]].csbLength] = dt;
        symbleTable[symbleTableIndexTable[symbleTableIndexTop-1]].csbLength += 1;
        //printf("ADD CSBL\n");
    }
    insertSubSymbleTable(n, dt, cl, 0);
    if (dt == DATATYPE_VOID_NORETURN
    || dt == DATATYPE_INT_RETURN
    || dt == DATATYPE_CHAR_RETURN) {
        subFHB = (SubFHB *)malloc(sizeof(SubFHB) * MAX_NUM_OF_BZF);
        symbleTable[pos].subFHB = subFHB;
        subTempFHB = (SubFHB *)malloc(sizeof(SubFHB) * MAX_NUM_OF_BZF);
        symbleTable[pos].subTempFHB = subTempFHB;
    }
    //printf("add :%s\n",n);
}

void insertArrayLengthSymbleTable(int v) {
    int pos = symbleTableIndexTable[symbleTableIndexTop];
    symbleTable[pos].csbLength = v;
    if (subFHB == NULL) {
        return;
    }
    int indexFunct = symbleTableIndexTable[symbleTableIndexTop - 1];
    SubFHB *subFhbPtr = symbleTable[indexFunct].subFHB + symbleTable[indexFunct].numVars - 1;
    subFhbPtr->length = v;
}

int hashSymbleTable(char n[MAXTXTLENGTH]) {
    int hash = 0;
    for(int i = 0; n[i] != '\0'; ++i)
    {
        hash = hash << 7 ^ n[i];
        //hash = hash*7 + key[i];
    }
    return (hash & 0x7FFFFFFF) % HASH_LENGTH + 1;
}

void deleteSymbleTable() {
    symbleTableIndexTop -= 1;
    for (int i = 1; i <= HASH_LENGTH;i++) {
        int pos = hashPositionTable[i];
        while (pos > symbleTableIndexTable[symbleTableIndexTop]) {
            pos = symbleTable[pos].nextnode;
        }
        hashPositionTable[i] = pos;
    }
    int pos = symbleTableIndexTable[symbleTableIndexTop];
    int dt = symbleTable[pos].dataType;
    if (dt == DATATYPE_VOID_NORETURN
        || dt == DATATYPE_INT_RETURN
        || dt == DATATYPE_CHAR_RETURN) {
        subFHB = NULL;
        subTempFHB = NULL;
    }
    //printf("DELECT BACK TO %d\n",symbleTableIndexTable[symbleTableIndexTop]);
}

void addSymbleTableBlock() {
    symbleTableIndexTop += 1;
    symbleTableIndexTable[symbleTableIndexTop] = symbleTableIndexTable[symbleTableIndexTop - 1];
}

int hasSymble(char n[MAXTXTLENGTH]) {
    int hash = hashSymbleTable(n);
    int pos = hashPositionTable[hash];
    int bound = symbleTableIndexTable[symbleTableIndexTop - 1];
    for (;pos != 0;pos = symbleTable[pos].nextnode) {
        if (pos > bound && strcmp(symbleTable[pos].name,n) == 0) {
            return pos;
        }
        if (pos <= symbleTableIndexTable[0] && strcmp(symbleTable[pos].name,n) == 0) {
            return pos;
        }
    }
    return 0;
}

int dupicateSymble(char n[MAXTXTLENGTH]) {
    int hash = hashSymbleTable(n);
    int pos = hashPositionTable[hash];
    int bound = symbleTableIndexTable[symbleTableIndexTop - 1];
    for (;pos != 0;pos = symbleTable[pos].nextnode) {
        if (pos > bound && strcmp(symbleTable[pos].name,n) == 0) {
            return pos;
        }
        if (pos <= symbleTableIndexTable[0] && strcmp(symbleTable[pos].name,n) == 0) {
            return 0;
        }
    }
    return 0;
}

int getDefaultType() {
    return symbleTable[symbleTableIndexTable[symbleTableIndexTop]].dataType;
}

int updateSymbleTable() {
    char *name;
    int nextRawType;
    int preRawType;
    int thisRawType;
    int prePreRawType;
    name = tokens[tokenstop - 1].name;
    nextRawType = symbol;
    preRawType = tokens[tokenstop - 2].symbol;
    thisRawType = tokens[tokenstop - 1].symbol;
    prePreRawType = tokens[tokenstop - 3].symbol;
    if (thisRawType == RBRACE) {
        deleteSymbleTable();
        addEndFunction();
    }
    else if (thisRawType == IDENFR || thisRawType == MAINTK) {
        int type;
        if (nextRawType == LPARENT) {
            switch (preRawType) {
                case INTTK: {
                    type = DATATYPE_INT_RETURN;
                    break;
                }
                case CHARTK: {
                    type = DATATYPE_CHAR_RETURN;
                    break;
                }
                case VOIDTK: {
                    type = DATATYPE_VOID_NORETURN;
                }
            }
            insertMcodeTable(symbleType2McodeType(type), name);
            insertSymbleTable(name,type,0);
            addSymbleTableBlock();
        }
        else if (nextRawType == LBRACK) {
            switch (preRawType) {
                case INTTK: {
                    type = DATATYPE_INT_ARRAY;
                    break;
                }
                case CHARTK: {
                    type = DATATYPE_CHAR_ARRAY;
                    break;
                }
                default: {
                    int pretype = getDefaultType();
                    switch (pretype) {
                        case DATATYPE_INT_VALUE:
                        case DATATYPE_INT_ARRAY: {
                            type = DATATYPE_INT_ARRAY;
                            break;
                        }
                        case DATATYPE_CHAR_VALUE:
                        case DATATYPE_CHAR_ARRAY: {
                            type = DATATYPE_CHAR_ARRAY;
                            break;
                        }
                    }
                }
            }
            insertMcodeTable(symbleType2McodeType(type), name);
            insertSymbleTable(name,type,0);
        }
        else if (prePreRawType == CONSTTK) {
            switch (preRawType) {
                case INTTK: {
                    type = DATATYPE_INT_CONST;
                    break;
                }
                case CHARTK: {
                    type = DATATYPE_CHAR_CONST;
                    break;
                }
            }
            insertMcodeTable(symbleType2McodeType(type), name);
            insertSymbleTable(name,type,0);
        }
        else {
            switch (preRawType) {
                case INTTK: {
                    type = DATATYPE_INT_VALUE;
                    break;
                }
                case CHARTK: {
                    type = DATATYPE_CHAR_VALUE;
                    break;
                }
                default: {
                    int pretype = getDefaultType();
                    switch (pretype) {
                        case DATATYPE_INT_CONST:
                        {
                            type = DATATYPE_INT_CONST;
                            break;
                        }
                        case DATATYPE_INT_VALUE:
                        case DATATYPE_INT_ARRAY: {
                            type = DATATYPE_INT_VALUE;
                            break;
                        }
                        case DATATYPE_CHAR_CONST:{
                            type = DATATYPE_CHAR_CONST;
                            break;
                        }
                        case DATATYPE_CHAR_VALUE:
                        case DATATYPE_CHAR_ARRAY: {
                            type = DATATYPE_CHAR_VALUE;
                            break;
                        }
                    }
                }
            }
            insertMcodeTable(symbleType2McodeType(type), name);
            insertSymbleTable(name,type,0);
        }
    }
    return 0;
}

int symbleType2McodeType(int symbletype) {
    switch (symbletype) {
        case DATATYPE_INT_CONST:
            return MID_CON_INT;
        case DATATYPE_CHAR_CONST:
            return MID_CON_CHAR;
        case DATATYPE_INT_VALUE: {
            if (isCSB) {
                return MID_PARA_INT;
            }
            return MID_VAR_INT;
        }
        case DATATYPE_CHAR_VALUE: {
            if (isCSB) {
                return MID_PARA_CHAR;
            }
            return MID_VAR_CHAR;
        }
        case DATATYPE_INT_RETURN:
            return MID_FUNC_INT;
        case DATATYPE_CHAR_RETURN:
            return MID_FUNC_CHAR;
        case DATATYPE_VOID_NORETURN:
            return MID_FUNC_VOID;
        case DATATYPE_INT_ARRAY:
            return MID_VAR_ARRAY_INT;
        case DATATYPE_CHAR_ARRAY:
            return MID_VAR_ARRAY_CHAR;
    }
}

int getType(int index) {
    return symbleTable[index].dataType;
}

int getCSL(int index) {
    return symbleTable[index].csbLength;
}

int getCSType(int index,int pos) {
    return symbleTable[index].paraType[pos];
}

int getFuncType() {
    int index = symbleTableIndexTable[symbleTableIndexTop - 1];
    return symbleTable[index].dataType;
}

void insertSubSymbleTable(char n[MAXTXTLENGTH], int dt, int cl, int istemp) {
    if (subFHB == NULL) {
        return;
    }
    int indexFunct = symbleTableIndexTable[symbleTableIndexTop - 1];
    int nums;
    if (n == NULL) {
        if (istemp < 0) {
            return;
        }
        nums = symbleTable[indexFunct].numTempVars;
        SubFHB *subFhbPtr = symbleTable[indexFunct].subTempFHB + istemp - 1;
        if (istemp > nums) {
            symbleTable[indexFunct].numTempVars = istemp;
            //strcpy(subFhbPtr->name,n);
            subFhbPtr->length = 1;
            //printf("add sub temp t%d to %s\n", istemp, symbleTable[indexFunct].name);
        }
        subFhbPtr->dataType = dt;
        return;
    }
    nums = symbleTable[indexFunct].numVars;
    SubFHB *firstsubFHB = symbleTable[indexFunct].subFHB;
    symbleTable[indexFunct].numVars++;
    SubFHB *subFhbPtr = firstsubFHB + nums;
    subFhbPtr->dataType = dt;
    strcpy(subFhbPtr->name,n);
    subFhbPtr->length = 1;
    //printf("add sub stated %s to %s\n", n, symbleTable[indexFunct].name);
}

FHB *getSymbleTableByName(char *name) {
    int pos = hasSymble(name);
    return symbleTable + pos;
}

int findStatedVarible(FHB *FHB, char *name) {
    int i = 0;
    SubFHB *tempSubFHB = FHB->subFHB;
    for (;i < FHB->numVars && strcmp((tempSubFHB + i)->name, name) != 0;i++);
    if (i < FHB->numVars) {
        return i;
    }
    return -1;
}

int findTempVarible(FHB *FHB, char *name) {
    int i = 0;
    SubFHB *tempSubFHB = FHB->subTempFHB;
    for (;i < FHB->numTempVars && strcmp((tempSubFHB + i)->name, name) != 0;i++);
    if (i < FHB->numTempVars) {
        return i;
    }
    return -1;
}

int findVarible(FHB *FHB, char *name) {
    int i = findStatedVarible(FHB, name);
    if (i < 0) {
        i = findTempVarible(FHB, name);
    }
    return i;
}

void setTempDataType(char name[], int dt) {
    int index = symbleTableIndexTable[symbleTableIndexTop - 1];
    FHB *fhb = symbleTable + index;
    SubFHB *subFHB = fhb->subTempFHB;
    int length = fhb->numTempVars;
    for (int i = 0;i < length;i++) {
        SubFHB *subFHBptr = subFHB + i;
        if (strcmp(subFHBptr->name, name) == 0) {
            subFHBptr->dataType = dt;
            printf("set %s to %d\n", subFHBptr->name, dt);
            return;
        }
    }
}

int *addInlineTemp(FHB *fhb, int newtempnum, int *tempids) {
    int *newid = (int *)malloc(MAX_NUM_OF_BZF* sizeof(int));
    int tempnum = fhb->numTempVars;
    for (int i = 0;i < newtempnum;i++) {
        fhb->subTempFHB[i + tempnum].dataType = DATATYPE_INT_VALUE;
        fhb->subTempFHB[i + tempnum].length = 1;
        newid[tempids[i]] = i + tempnum + 1;
    }
    fhb->numTempVars += newtempnum;
    return newid;
}
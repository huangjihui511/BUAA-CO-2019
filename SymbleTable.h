//
// Created by 黄继辉 on 2019-10-30.
//

#ifndef ERROR_HANDLE_SYMBLETABLE_H
#define ERROR_HANDLE_SYMBLETABLE_H

#include "LexicalAnalyzer.h"

#define MAX_NUM_OF_BZF 1000
#define HASH_LENGTH 13

#define DATATYPE_INT_VALUE 0
#define DATATYPE_CHAR_VALUE 1
#define DATATYPE_INT_ARRAY 2
#define DATATYPE_CHAR_ARRAY 3
#define DATATYPE_VOID_NORETURN 4
#define DATATYPE_INT_RETURN 5
#define DATATYPE_CHAR_RETURN 6
#define DATATYPE_INT_CONST 7
#define DATATYPE_CHAR_CONST 8

typedef struct struct_sub_ST {
    char name[MAXTXTLENGTH];
    int dataType;
    int offset;
    int length;
    int rName;
    int isInRegister;
} SubFHB;

typedef struct struct2 {
    char name[MAXTXTLENGTH];
    int dataType;
    int csbLength;
    int nextnode;
    int paraType[200];
    int numVars = 0;
    SubFHB *subFHB;
    int numTempVars = 0;
    SubFHB *subTempFHB;
    int stacklength;
    int slength;
    int tlength;
    int offset2Reg[MAX_NUM_OF_BZF];
} FHB;


void insertSymbleTable(char n[MAXTXTLENGTH], int dt, int cl);

void insertArrayLengthSymbleTable(int v);

int hashSymbleTable(char n[MAXTXTLENGTH]);

void deleteSymbleTable();

void addSymbleTableBlock();

int hasSymble(char n[MAXTXTLENGTH]);

int dupicateSymble(char n[MAXTXTLENGTH]);

int updateSymbleTable();

int getDefaultType();

int getType(int index);

int getCSL(int index);

int getCSType(int index,int pos);

int getFuncType();

int symbleType2McodeType(int symbletype);

void insertSubSymbleTable(char n[MAXTXTLENGTH], int dt, int cl, int istemp);

FHB *getSymbleTableByName(char *name);

int findStatedVarible(FHB *FHB, char *name);

int findTempVarible(FHB *FHB, char *name);

int findVarible(FHB *FHB, char *name);

void setTempDataType(char name[], int dt);

int *addInlineTemp(FHB *fhb, int newtempnum, int *tempids);

#endif //ERROR_HANDLE_SYMBLETABLE_H

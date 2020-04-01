//
// Created by 黄继辉 on 2019-11-12.
//

#ifndef MIDDLE_CODE_MIDDLECODE_H
#define MIDDLE_CODE_MIDDLECODE_H

#include "LexicalAnalyzer.h"
#include "MidCodeExpression.h"

#define MAX_NAME_LENGTH 50

#define MID_FUNC_INT 0
#define MID_FUNC_CHAR 1
#define MID_FUNC_VOID 2
#define MID_PARA_INT 3
#define MID_PARA_CHAR 4
#define MID_PUSH 5
#define MID_CALL 6
#define MID_RET_VALUE 7
#define MID_RET_NOVALUE 18
#define MID_CON_INT 8
#define MID_CON_CHAR 9
#define MID_VAR_INT 10
#define MID_VAR_CHAR 11
#define MID_VAR_ARRAY_INT 12
#define MID_VAR_ARRAY_CHAR 13
#define MID_ADD 14
#define MID_MINUS 15
#define MID_MULT 16
#define MID_DIVI 17
#define MID_GOTO 19
#define MID_BNZ 20
#define MID_BZ 21
#define MID_LABEL 22
#define MID_READ_ARRAY 23
#define MID_ASSIGH_ARRAY 24
#define MID_ASSIGH_VAR 25
#define MID_ASSIGH_INT 26
#define MID_ASSIGH_CHAR 27
#define MID_ASSIGH_OPPOSE 28
#define MID_EQUAL 29
#define MID_LESS 30
#define MID_GREATER 31
#define MID_EQUAL_LESS 32
#define MID_EQUAL_GREATER 33
#define MID_NOT_EQUAL 34
#define MID_SCANF_CALL 35
#define MID_PRINTF_CALL 36
#define MID_SCANF_PUSH 37
#define MID_PRINTF_PUSH_VAR 38
#define MID_PRINTF_PUSH_STR 39
#define MID_CONST_ASSIGH 40
#define MID_END_FUNCTION 41
#define MID_STORE_VAR 42
#define MID_STORE_ARRAY 43
#define MID_NEWLINE 44
#define MID_MOVE_V0 45
#define MID_START_PUSH 46
#define MID_END_PUSH 47
#define MID_INLINE_LOAD 48
#define MID_INLINE_CALL 49
#define MID_INLINE_END 50
#define MID_INLINE_RET_VALUE 51
#define MID_INLINE_RET_NO_VALUE 52
#define MID_INLINE_STORE 53
#define MID_NEW_PRINTF 54
#define MID_ADDI 55
#define MID_BZ0 56
#define MID_BNZ0 57
#define MID_START_LOOP 58
#define MID_END_LOOP 59
#define MID_INLINE_MOVE 60

#define JUMP_IF 0
#define JUMP_DO 1
#define JUMP_FOR 2
#define JUMP_WHILE 3
#define JUMP 4

typedef struct struct3 {
    int midType;
    char nameLeft[MAXTXTLENGTH];
    char nameRight1[MAXTXTLENGTH];
    char nameRight2[MAXTXTLENGTH];
    int numLeft;
    int numRight1;
    int numRight2;
    int tempLeft;
    int tempRight1;
    int tempRight2;
    int indexLeft;
    int indexRight;
    int isInline = 0;
} Mcode;

void addMcodeTable(int midType, char nameLeft[], char nameRight1[]
        , char nameRight2[], int numLeft, int numRight1
        , int numRight2, int tempLeft, int tempRight1
        , int tempRight2, int indexLeft, int indexRight);

void insertMcodeTable(int midType, char name[]);

void insertArrayLength(int v);

void setMcodeTableValue(int value);

void pushMcodeTable();

void startpushMcodeTable();

void endpushMcodeTable();

void scanfPushMcodeTable(char name[]);

void printfPushVarMcodeTable();

void callMcodeTable();

void printfCallMcodeTable();

void printfPushStrMcodeTable(char str[]);

void scanfCallMcodeTable();

void initAssignMcodeTable();

void setIndexAssignMcodeTable();

void setValueAssignMcodeTable();

void initEqualMcodeTable();

void setEqualZeroMcodeTable();

void setEqualMcodeTable();

void returnValueMcodeTable();

void returnNOValueMcodeTable();

int jumpMcodeTable(int jumpType);

int labelMcodeTable();

void fillInJumpMcodeTable(int index, int target);

void addEndFunction();

void addNewline();

void forAdd(char name1[], char name2[], int value);

int allocTempVar();

void recycleTempVar2int(int i);

void recycleTempVar1step();

int expressionMcodeTable(MidCodeExpression *mce);

int itemMcodeTable(MidCodeItem *mci);

int factorMcodeTable(MidCodeFactor *mcf);

void newPrintfMcodeTable();

void addStartLoop();

void addEndLoop();

#endif //MIDDLE_CODE_MIDDLECODE_H

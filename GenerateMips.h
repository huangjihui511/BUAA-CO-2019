//
// Created by 黄继辉 on 2019-11-15.
//

#ifndef MIDDLE_CODE_GENERATEMIPS_H
#define MIDDLE_CODE_GENERATEMIPS_H

#include "MiddleCode.h"
#include "SymbleTable.h"
#include "Optimal.h"

#define byte2word 4
#define S02SP 2
#define VAR2SP 19
#define T02SP 10

#define S_NUM 8
#define T_NUM 8

#define GREATER_JUMP 0
#define GREATER_EQUAL_JUMP 1
#define EQUAL_ZERO_JUMP 2
#define NOT_EQUAL_ZERO_JUMP 3

#define MIPS_VARIBLE_TYPE_INT 0
#define MIPS_VARIBLE_TYPE_char 1

#define T0_OFFSET 8
#define S0_OFFSET 16
#define V0_OFFSET 2
#define A0_OFFSET 4
#define SP_OFFSET 29
#define RA_OFFSET 31
#define T8_OFFSET 24

typedef struct struct10{
    char *str = nullptr;
    int is_value = 0;
    int value;
    int type_value;
    struct struct10 *nextnode = nullptr;
} Printlink;

char *scanMidCode();

char *initStr();

char *Mcode2Mips(McodeList *mcodeListptr);

char *swCommand(int r1, int offset, int r2);

char *lwCommand(int r1, int offset, int r2);

char *liCommand(int r1, int value);

char *jalCommand(char name[]);

char *sllCommand(int r1, int r2, int offset);

char *addCommand(int r1, int r2, int r3);

char *addiCommand(int r1, int r2, int value);

char *subCommand(int r1, int r2, int r3);

char *laCommand(int r1, char variableName[]);

char *multCommand(int r1, int r2, int r3);

char *divCommand(int r1, int r2, int r3);

char *moveCommand(int r1, int r2);

char *jCommand(char labelName[]);

char *syscallCommand(int type, int r1,  char str[]);

char *tempNum2RegisterName(int num);

char *storeRegister2Variable(char variableName[], int rsrc);

char *loadVariable2Register(char variableName[], int rtarget);

char *initConst(char variableName[], int value);

char *loadPara();

char *pushParameter(int num);

char *computeParameter();

char *compare(int type, int r1, int r2);

char *jump2Labelbz(char labelname[]);

char *jump2Labelbnz(char labelname[]);

char *loadArray2Register(char arrayName[], int rtarget, int rindex);

char *assignIntChar(char r1, int value);

char *fourCompute(int type, int r1, int r2, int r3);

char *assignOppose(int rtarget,int rsrc);

char *storeRegister2Array(int rsrc, char arrayName[],int rindex);

char *endFunction();

char *call(char fname[], int *regs);

void setVariableType(char variableName[]);

char *newLine();

char *moveFromV0(int rtarget);

char *move2V0(int rsrc);

char *inlineLoad(int rtarget, int rsrc);

void addPushList(McodeList *mcodelistptr);

McodeList *getPushListTop();

int isTempInRegister(int offset);

char *loadLargeTemp(int offset, int is2nd);

char *storeLargeTemp(int offset);

char *handleTransfore(char *rawstr);

char *addi(int rtarget, int rsrc, int value);

char *jump2Labelbz0(int type, int rsrc, char *labelname);

#endif //MIDDLE_CODE_GENERATEMIPS_H

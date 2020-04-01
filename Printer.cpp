//
// Created by 黄继辉 on 2019-09-30.
//
#include "LexicalAnalyzer.h"
#include "Printer.h"
#include <fstream>
#include "GrammarAnalyzer.h"
#include "ErrorHandle.h"
#include "MiddleCode.h"
#include "ExpressionChecker.h"
#include "Optimal.h"

using namespace std;

extern Token tokens[MAXTXTLENGTH];
extern int tokenstop;
extern int tokensstack;

extern int symbol;
extern char token[MAXTXTLENGTH];
extern char inputtxt[MAXTXTLENGTH];
extern int inputtxt_top;

extern ofstream outfile;
extern ofstream errorfile;
extern ofstream middlecodefile;
extern ofstream mips;

extern char id2Tag[100][100];

extern int countLine;

extern int mcodeTableTop;
extern Mcode mcodeTable[MAXTXTLENGTH];

extern char preExpressionName[20];
extern int isCorrect;
extern int submit;
extern McodeList *mcodeListHead;
Token tokens[MAXTXTLENGTH];
int tokenstop = 0;
int tokensstack = 0;


void printToken() {
    for (;tokensstack < tokenstop;tokensstack++) {
        //printf("%s %s\n", num2typename[tokens[tokensstack].symbol],tokens[tokensstack].name);
        outfile << num2typename[tokens[tokensstack].symbol] << " " << tokens[tokensstack].name << endl;
    }
}

void inputToken() {
    if (!(symbol==0 && token[0]=='\0')) {
        tokens[tokenstop].symbol = symbol;
        tokens[tokenstop].line = countLine;
        strcpy(tokens[tokenstop].name, token);
        tokenstop++;
    }
}

void strRetract() {
    while(inputtxt[inputtxt_top] != ')'
    && inputtxt[inputtxt_top] != ',') {
        inputtxt_top--;
    }
}

void printTokenRetract(int a) {
    tokenstop -= a;
    //printToken();
    tokenstop += a;
}

void printTag(int tag) {
    outfile << id2Tag[tag] << endl;
    //printf("%s\n",id2Tag[tag]);
}

void printError(int errorType) {
    int line = countLine;
    /*
    if (errorType == ERROR_TYPE_K) {
        line -= changeLine;
    }
    else if (errorType == ERROR_TYPE_H) {
        line -= changeLine;
    }
     */
    isCorrect = 0;
    errorfile << tokens[tokenstop - 1].line << " " << char('a' + errorType) << endl;
    printf("%d %c\n",tokens[tokenstop - 1].line,'a' + errorType);
}

char *getMcode(int i) {
    return mcode2string(mcodeTable + i);
}

void printMcode() {
    McodeList *mcodeListptr1 = mcodeListHead;
    while (mcodeListptr1 != NULL) {
        char middleCodeContent[MAXTXTLENGTH] = {0};
        strcat(middleCodeContent, mcode2string(&mcodeListptr1->mcode));
        mcodeListptr1 = mcodeListptr1->nextnode;
        middlecodefile << middleCodeContent << endl;
    }
}

void printMips(char *str) {
    if (str[0] != 0) {
        mips << str;
        printf("%s", str);
    }
}

char *mcode2string(Mcode *mcodeptr) {
    char *tempContent = (char *)malloc(sizeof(char) * MAXTXTLENGTH);
    int isgloble = 0;
    switch (mcodeptr->midType) {
        case MID_FUNC_INT: {
            sprintf(tempContent, "int %s()\n", mcodeptr->nameLeft);
            break;
        }
        case MID_FUNC_CHAR: {
            sprintf(tempContent, "char %s()\n", mcodeptr->nameLeft);
            break;
        }
        case MID_FUNC_VOID: {
            sprintf(tempContent, "void %s()\n", mcodeptr->nameLeft);
            break;
        }
        case MID_CON_CHAR: {
            if (mcodeptr->numRight2) {
                isgloble = 1;
            }
            sprintf(tempContent, "const char %s = \'%c\'\n", mcodeptr->nameLeft, mcodeptr->numRight1);
            break;
        }
        case MID_CON_INT: {
            if (mcodeptr->numRight2) {
                isgloble = 1;
            }
            sprintf(tempContent, "const int %s = %d\n", mcodeptr->nameLeft, mcodeptr->numRight1);
            break;
        }
        case MID_VAR_CHAR: {
            if (mcodeptr->numRight2) {
                isgloble = 1;
            }
            sprintf(tempContent, "var char %s\n", mcodeptr->nameLeft);
            break;
        }
        case MID_VAR_INT: {
            if (mcodeptr->numRight2) {
                isgloble = 1;
            }
            sprintf(tempContent, "var int %s\n", mcodeptr->nameLeft);
            break;
        }
        case MID_VAR_ARRAY_INT: {
            if (mcodeptr->numRight2) {
                isgloble = 1;
            }
            sprintf(tempContent, "var int %s[%d]\n", mcodeptr->nameLeft, mcodeptr->numLeft);
            break;
        }
        case MID_VAR_ARRAY_CHAR: {
            if (mcodeptr->numRight2) {
                isgloble = 1;
            }
            sprintf(tempContent, "var char %s[%d]\n", mcodeptr->nameLeft, mcodeptr->numLeft);
            break;
        }
        case MID_PARA_CHAR: {
            sprintf(tempContent, "para char %s\n", mcodeptr->nameLeft);
            break;
        }
        case MID_PARA_INT: {
            sprintf(tempContent, "para int %s\n", mcodeptr->nameLeft);
            break;
        }
        case MID_SCANF_CALL:
        case MID_PRINTF_CALL:
        case MID_CALL: {
            sprintf(tempContent, "call %s\n", mcodeptr->nameLeft);
            break;
        }
        case MID_BNZ: {
            sprintf(tempContent, "BNZ Label_%d\n", mcodeptr->numRight1);
            break;
        }
        case MID_BZ: {
            sprintf(tempContent, "BZ Label_%d\n", mcodeptr->numRight1);
            break;
        }
        case MID_GOTO: {
            sprintf(tempContent, "GOTO Label_%d\n", mcodeptr->numRight1);
            break;
        }
        case MID_LABEL: {
            sprintf(tempContent, "Label_%d :\n", mcodeptr->numLeft);
            break;
        }
        case MID_READ_ARRAY: {
            sprintf(tempContent, "t%d = %s[t%d]\n", mcodeptr->tempLeft, mcodeptr->nameRight1, mcodeptr->indexRight);
            break;
        }
        case MID_CONST_ASSIGH: {
            sprintf(tempContent, "CONST %s: t%d = %d\n", mcodeptr->nameLeft, mcodeptr->tempLeft, mcodeptr->numRight1);
            break;
        }
        case MID_ASSIGH_INT: {
            sprintf(tempContent, "t%d = %d\n", mcodeptr->tempLeft, mcodeptr->numRight1);
            break;
        }
        case MID_ASSIGH_CHAR: {
            sprintf(tempContent, "t%d = \'%c\'\n", mcodeptr->tempLeft, mcodeptr->numRight1);
            break;
        }
        case MID_DIVI: {
            sprintf(tempContent, "t%d = t%d / t%d\n", mcodeptr->tempLeft, mcodeptr->tempRight1, mcodeptr->tempRight2);
            break;
        }
        case MID_MULT: {
            sprintf(tempContent, "t%d = t%d * t%d\n", mcodeptr->tempLeft, mcodeptr->tempRight1, mcodeptr->tempRight2);
            break;
        }
        case MID_ADD: {
            sprintf(tempContent, "t%d = t%d + t%d\n", mcodeptr->tempLeft, mcodeptr->tempRight1, mcodeptr->tempRight2);
            break;
        }
        case MID_MINUS: {
            sprintf(tempContent, "t%d = t%d - t%d\n", mcodeptr->tempLeft, mcodeptr->tempRight1, mcodeptr->tempRight2);
            break;
        }
        case MID_ASSIGH_OPPOSE: {
            sprintf(tempContent, "t%d = -t%d\n", mcodeptr->tempLeft, mcodeptr->tempRight1);
            break;
        }
        case MID_PUSH: {
            sprintf(tempContent, "PUSH t%d\n", mcodeptr->tempRight1);
            break;
        }
        case MID_STORE_VAR: {
            sprintf(tempContent, "STORE %s = t%d\n"
                    , mcodeptr->nameLeft, mcodeptr->tempRight1);
            break;
        }
        case MID_ASSIGH_VAR: {
            sprintf(tempContent, "t%d = %s\n", mcodeptr->tempLeft, mcodeptr->nameRight1);
            break;
        }
        case MID_STORE_ARRAY: {
            sprintf(tempContent, "STORE %s[t%d] = t%d\n", mcodeptr->nameLeft, mcodeptr->indexLeft, mcodeptr->tempRight1);
            break;
        }
        case MID_LESS: {
            sprintf(tempContent, "t%d < t%d\n", mcodeptr->tempRight1, mcodeptr->tempRight2);
            break;
        }
        case MID_EQUAL_LESS: {
            sprintf(tempContent, "t%d <= t%d\n", mcodeptr->tempRight1, mcodeptr->tempRight2);
            break;
        }
        case MID_GREATER: {
            sprintf(tempContent, "t%d > t%d\n", mcodeptr->tempRight1, mcodeptr->tempRight2);
            break;
        }
        case MID_EQUAL_GREATER: {
            sprintf(tempContent, "t%d >= t%d\n", mcodeptr->tempRight1, mcodeptr->tempRight2);
            break;
        }
        case MID_EQUAL: {
            sprintf(tempContent, "t%d == t%d\n", mcodeptr->tempRight1, mcodeptr->tempRight2);
            break;
        }
        case MID_NOT_EQUAL: {
            sprintf(tempContent, "t%d != t%d\n", mcodeptr->tempRight1, mcodeptr->tempRight2);
            break;
        }
        case MID_RET_VALUE: {
            sprintf(tempContent, "ret t%d END_FUNCTION%d\n", mcodeptr->tempRight1, mcodeptr->numLeft);
            break;
        }
        case MID_RET_NOVALUE: {
            sprintf(tempContent, "ret END_FUNCTION%d\n", mcodeptr->numLeft);
            break;
        }
        case MID_SCANF_PUSH: {
            sprintf(tempContent, "POSISTION t%d SCANF_PUSH %s\n", mcodeptr->tempLeft, mcodeptr->nameLeft);
            break;
        }
        case MID_PRINTF_PUSH_VAR: {
            sprintf(tempContent, "PRINTF_PUSH_VAR t%d %s\n", mcodeptr->tempRight1,(mcodeptr->numLeft)? "int" : "char");
            break;
        }
        case MID_PRINTF_PUSH_STR: {
            sprintf(tempContent, "PRINTF_PUSH_STR %s \"%s\"\n", mcodeptr->nameRight1, mcodeptr->nameLeft);
            break;
        }
        case MID_END_FUNCTION: {
            sprintf(tempContent, "END_FUNCTION%d :\n", mcodeptr->numLeft);
            break;
        }
        case MID_INLINE_END: {
            sprintf(tempContent, "END_INLINE_FUNCTION%d :\n", mcodeptr->numLeft);
            break;
        }
        case MID_NEWLINE: {
            sprintf(tempContent, "NEWLINE\n");
            break;
        }
        case MID_MOVE_V0: {
            sprintf(tempContent, "MOVE t%d v0\n", mcodeptr->tempLeft);
            break;
        }
        case MID_INLINE_MOVE: {
            sprintf(tempContent, "INLINE MOVE t%d t%d\n", mcodeptr->tempLeft, mcodeptr->tempRight1);
            break;
        }
        case MID_INLINE_LOAD: {
            sprintf(tempContent, "INLINE LOAD t%d t%d\n", mcodeptr->tempLeft, mcodeptr->tempRight1);
            break;
        }
        case MID_START_PUSH: {
            sprintf(tempContent, "START PUSH\n");
            break;
        }
        case MID_END_PUSH: {
            sprintf(tempContent, "END PUSH\n");
            break;
        }
        case MID_INLINE_CALL: {
            sprintf(tempContent, "INLINE call %s\n", mcodeptr->nameLeft);
            break;
        }
        case MID_INLINE_STORE: {
            sprintf(tempContent, "INLINE STORE MOVE t%d t%d\n", mcodeptr->tempLeft, mcodeptr->tempRight1);
            break;
        }
        case MID_NEW_PRINTF: {
            sprintf(tempContent, "NEW PRINTF\n");
            break;
        }
        case MID_ADDI: {
            sprintf(tempContent, "ADDI t%d = t%d + %d\n", mcodeptr->tempLeft, mcodeptr->tempRight1, mcodeptr->numRight1);
            break;
        }
        case MID_START_LOOP: {
            sprintf(tempContent, "START LOOP\n");
            break;
        }
        case MID_END_LOOP: {
            sprintf(tempContent, "END LOOP\n");
            break;
        }
    }
    if (isgloble) {
        char temp[MAXTXTLENGTH] = "global ";
        strcat(temp, tempContent);
        strcpy(tempContent, temp);
    }
    return tempContent;
}
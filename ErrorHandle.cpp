//
// Created by »Æ¼Ì»Ô on 2019-10-30.
//

#include <cstdio>
#include "LexicalAnalyzer.h"
#include "ErrorHandle.h"
#include "Printer.h"
#include "SymbleTable.h"
#include "ExpressionChecker.h"
#include "GrammarAnalyzer.h"

extern int tokenstop;

extern Token tokens[MAXTXTLENGTH];

extern int recentHSDYYJ;

extern int symbol;

int checkError(int errorType, int mode) {
    switch (errorType) {
        case ERROR_TYPE_A: {
            printError(errorType);
            for (int i = 0;i < mode;i++) {
                getChar();
            }
            break;
        }
        case ERROR_TYPE_B: {
            char *name = tokens[tokenstop - 1].name;
            if (dupicateSymble(name)) {
                printError(errorType);
            }
            break;
        }
        case ERROR_TYPE_C: {
            char *name = tokens[tokenstop - 1].name;
            if (!hasSymble(name)) {
                printError(errorType);
            }
            break;
        }
        case ERROR_TYPE_D: {
            char *name = tokens[recentHSDYYJ].name;
            int index = hasSymble(name);
            if (index) {
                if (getCSL(index) != mode) {
                    printError(errorType);
                }
            }
            break;
        }
        case ERROR_TYPE_E: {
            int BDSType = getpreBDSType();
            char *name = tokens[recentHSDYYJ].name;
            int index = hasSymble(name);
            int CSType = getCSType(index,mode);
            if (BDSType == EXP_TYPE_INT) {
                if (CSType != DATATYPE_INT_VALUE) {
                    printError(errorType);
                }
            }
            if (BDSType == EXP_TYPE_CHAR) {
                if (CSType != DATATYPE_CHAR_VALUE) {
                    printError(errorType);
                }
            }
            break;
        }
        case ERROR_TYPE_F: {
            if (mode != getpreBDSType()) {
                printError(errorType);
                return 1;
            }
            break;
        }
        case ERROR_TYPE_G: {
            int type = getFuncType();
            if (type == DATATYPE_VOID_NORETURN) {
                printError(errorType);
            }
            break;
        }
        case ERROR_TYPE_H: {
            int funcType = getFuncType();
            int returnType = checkReturnType();
            if (funcType == DATATYPE_CHAR_RETURN) {
                if (returnType == EXP_TYPE_CHAR) {
                    break;
                }
            }
            if (funcType == DATATYPE_INT_RETURN) {
                if (returnType == EXP_TYPE_INT) {
                    break;
                }
            }
            if (funcType == DATATYPE_VOID_NORETURN) {
                if (returnType == EXP_TYPE_NULL) {
                    break;
                }
            }
            printError(errorType);
            break;
        }
        case ERROR_TYPE_I: {
            if (EXP_TYPE_INT != getpreBDSType()) {
                printError(errorType);
            }
        }
        case ERROR_TYPE_J: {
            if (tokens[tokenstop - 2].symbol == IDENFR) {
                char *name = tokens[tokenstop - 2].name;
                int index = hasSymble(name);
                int type = getType(index);
                if (type == DATATYPE_CHAR_CONST || type == DATATYPE_INT_CONST) {
                    printError(errorType);
                }
            }
            break;
        }
        case ERROR_TYPE_K:
        case ERROR_TYPE_L:
        case ERROR_TYPE_M:
        case ERROR_TYPE_N: {
            printError(errorType);
            break;
        }
        case ERROR_TYPE_O: {
            /*
            if (tokens[tokenstop - 2].symbol == IDENFR) {
                char *name = tokens[tokenstop - 2].name;
                int index = hasSymble(name);
                int type = getType(index);
                if (type == DATATYPE_INT_CONST) {
                    if (symbol != ZSid) {
                        printError(errorType);
                    }
                }
                if (type == DATATYPE_CHAR_CONST) {
                    if (symbol != CHARCON) {
                        printError(errorType);
                    }
                }
            }*/
            printError(errorType);
            break;
        }
    }
    return 0;
}

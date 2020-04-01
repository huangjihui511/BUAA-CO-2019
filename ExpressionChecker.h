//
// Created by 黄继辉 on 2019-10-31.
//

#ifndef ERROR_HANDLE_EXPRESSIONCHECKER_H
#define ERROR_HANDLE_EXPRESSIONCHECKER_H

#define EXP_TYPE_NULL 0
#define EXP_TYPE_INT 1
#define EXP_TYPE_CHAR 2

void createBDSType();

void getYZTyoe(int type1);

void getXtype(int type2);

void getBDStype(int type2);

void endBDSType();

int getpreBDSType();

void initReturnType();

void addReturnType(int isBDS);

int checkReturnType();
#endif //ERROR_HANDLE_EXPRESSIONCHECKER_H

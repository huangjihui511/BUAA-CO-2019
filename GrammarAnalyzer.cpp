//
// Created by 黄继辉 on 2019-09-29.
//

#include "GrammarAnalyzer.h"
#include <fstream>
#include <iostream>
#include "LexicalAnalyzer.h"

#include "Printer.h"
#include "SymbleTable.h"
#include "ErrorHandle.h"
#include "ExpressionChecker.h"
#include "MiddleCode.h"
#include "MidCodeExpression.h"
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

using namespace std;

extern int symbol;

extern char token[MAXTXTLENGTH];

extern ofstream outfile;

extern int tokenstop;

extern int countLine;

char id2Tag[100][100];

int isRead_LXBZF_BZF = 0;

int isCSB = 0;

int recentHSDYYJ;

int ZCSBl;

int ZS_value;

MidCodeFactor tempmcf;

void grammarAnalyse() {
    grammerMethods[0] = JFYSF;
    grammerMethods[1] = CFYSF;
    grammerMethods[2] = GXYSF;
    grammerMethods[3] = ZFC;
    grammerMethods[4] = CX;
    grammerMethods[5] = CLSM;
    grammerMethods[6] = CLDY;
    grammerMethods[7] = WFHZS;
    grammerMethods[8] = ZS;
    grammerMethods[9] = BZF;
    grammerMethods[10] = SMTB;
    grammerMethods[11] = BLSM;
    grammerMethods[12] = BLDY;
    grammerMethods[13] = LXBZF;
    grammerMethods[14] = YFHZHSDY;
    grammerMethods[15] = WFHZHSDY;
    grammerMethods[16] = FHYJ;
    grammerMethods[17] = CSB;
    grammerMethods[18] = ZHS;
    grammerMethods[19] = BDS;
    grammerMethods[20] = X;
    grammerMethods[21] = YZ;
    grammerMethods[22] = YJ;
    grammerMethods[23] = FZYJ;
    grammerMethods[24] = TJYJ;
    grammerMethods[25] = TJ;
    grammerMethods[26] = XHYJ;
    grammerMethods[27] = BC;
    grammerMethods[28] = YFHZHSDYYJ;
    grammerMethods[29] = WFHZHSDYYJ;
    grammerMethods[30] = ZCSB;
    grammerMethods[31] = YJL;
    grammerMethods[32] = DYJ;
    grammerMethods[33] = XYJ;
    grammerMethods[34] = FANHYJ;
    grammerMethods[35] = CONSTTK_CLDY_SEMICN;
    grammerMethods[36] = COMMA_BZF_ASSIGN_ZS;
    grammerMethods[37] = COMMA_BZF_ASSIGN_CHARCON;
    grammerMethods[38] = BLSM_SEMICN;
    grammerMethods[39] = COMMA_LXBZF_BZF;
    grammerMethods[40] = JFYSF_X;
    grammerMethods[41] = CFYSF_YZ;
    grammerMethods[42] = ELSETK_YJ;
    grammerMethods[43] = COMMA_BDS;
    grammerMethods[44] = COMMA_BZF;
    grammerMethods[45] = SHORT_BLSM;
    grammerMethods[46] = SHORT_YFHZHSDY;
    grammerMethods[47] = LPARENT_BDS_RPARENT;
    grammerMethods[48] = COMMA_BZF_WITH_BRACKET;

    strcpy(id2Tag[ZFCid],"<字符串>");
    strcpy(id2Tag[ZFCid],"<字符串>");
    strcpy(id2Tag[CXid],"<程序>");
    strcpy(id2Tag[CLSMid],"<常量说明>");
    strcpy(id2Tag[CLDYid],"<常量定义>");
    strcpy(id2Tag[WFHZSid],"<无符号整数>");
    strcpy(id2Tag[ZSid],"<整数>");
    strcpy(id2Tag[SMTBid],"<声明头部>");
    strcpy(id2Tag[BLSMid],"<变量说明>");
    strcpy(id2Tag[BLDYid],"<变量定义>");
    strcpy(id2Tag[YFHZHSDYid],"<有返回值函数定义>");
    strcpy(id2Tag[WFHZHSDYid],"<无返回值函数定义>");
    strcpy(id2Tag[FHYJid],"<复合语句>");
    strcpy(id2Tag[CSBid],"<参数表>");
    strcpy(id2Tag[ZHSid],"<主函数>");
    strcpy(id2Tag[BDSid],"<表达式>");
    strcpy(id2Tag[Xid],"<项>");
    strcpy(id2Tag[YZid],"<因子>");
    strcpy(id2Tag[YJid],"<语句>");
    strcpy(id2Tag[FZYJid],"<赋值语句>");
    strcpy(id2Tag[TJYJid],"<条件语句>");
    strcpy(id2Tag[TJid],"<条件>");
    strcpy(id2Tag[XHYJid],"<循环语句>");
    strcpy(id2Tag[BCid],"<步长>");
    strcpy(id2Tag[YFHZHSDYYJid],"<有返回值函数调用语句>");
    strcpy(id2Tag[WFHZHSDYYJid],"<无返回值函数调用语句>");
    strcpy(id2Tag[ZCSBid],"<值参数表>");
    strcpy(id2Tag[YJLid],"<语句列>");
    strcpy(id2Tag[DYJid],"<读语句>");
    strcpy(id2Tag[XYJid],"<写语句>");
    strcpy(id2Tag[FANHYJid],"<返回语句>");
    /*
    id2Tag[ZFCid] = "<字符串>" ;
    id2Tag[CXid] = "<程序>";
    id2Tag[CLSMid] = "<常量说明>" ;
    id2Tag[CLDYid] = "<常量定义>");
    id2Tag[WFHZSid] = "<无符号整数>";
    id2Tag[ZSid] = "<整数>";
    id2Tag[SMTBid] = "<声明头部>";
    id2Tag[BLSMid] = "<变量说明>";
    id2Tag[BLDYid] = "<变量定义>";
    id2Tag[YFHZHSDYid] = "<有返回值函数定义>";
    id2Tag[WFHZHSDYid] = "<无返回值函数定义>";
    id2Tag[FHYJid] = "<复合语句>";
    id2Tag[CSBid] = "<参数表>";
    id2Tag[ZHSid] = "<主函数>";
    id2Tag[BDSid] = "<表达式>";
    id2Tag[Xid] = "<项>";
    id2Tag[YZid] = "<因子>";
    id2Tag[YJid] = "<语句>";
    id2Tag[FZYJid] = "<赋值语句>";
    id2Tag[TJYJid] = "<条件语句>";
    id2Tag[TJid] = "<条件>";
    id2Tag[XHYJid] = "<循环语句>";
    id2Tag[BCid] = "<步长>";
    id2Tag[YFHZHSDYYJid] = "<有返回值函数调用语句>";
    id2Tag[WFHZHSDYYJid] = "<无返回值函数调用语句>";
    id2Tag[ZCSBid] = "<值参数表>";
    id2Tag[YJLid] = "<语句列>";
    id2Tag[DYJid] = "<读语句>";
    id2Tag[XYJid] = "<写语句>";
    id2Tag[FANHYJid] = "<返回语句>";
    */
    CX();
}

void getSyms() {
    while (symbol != COMMA && symbol != SEMICN) {
        getsym();
    }
}

void grammarError() {
    printf("grammer analyse failure line %d\n", countLine);
    //exit(1);
}
int isEqual(int cmp) {
    if (symbol == cmp) {
        getsym();
        return 1;
    }
    return 0;
}

int mustExist(int cmp) {
    if (isEqual(cmp)) {
        return 1;
    }
    else {
        if (cmp == SEMICN) {
            checkError(ERROR_TYPE_K,0);
            return 1;
        }
        else if (cmp == RPARENT) {
            checkError(ERROR_TYPE_L,0);
            return 1;
        }
        else if (cmp == RBRACK) {
            checkError(ERROR_TYPE_M,0);
            return 1;
        }
        else if (cmp == WHILETK) {
            checkError(ERROR_TYPE_N,0);
            return 1;
        }
        else {
            grammarError();
        }
    }
    return 0;
}

int justyangleBracket(int id) {
    if (grammerMethods[id]()) {
        //getsym();
        return 1;
    }
    return 0;
}

int angleBracket(int id) {
    if (grammerMethods[id]()) {
        //getsym();
        return 1;
    }
    grammarError();
    return 0;
}

int squareBracket(int count, ...) {
    va_list args;
    va_start(args, count);
    for (int i = 0; i < count; ++i)
    {
        int id = va_arg(args, int);
        if (grammerMethods[id]()) {
            //getsym();
            va_end(args);
            return 1;
        }
    }
    va_end(args);
    return 1;
}

int bigBracket(int count,...) {
    int stop = 1;
    va_list args;
    va_start(args, count);
    for (int i = 0; i < count; ++i) {
        int id = va_arg(args, int);
        while(stop) {
            stop = grammerMethods[id]();
        }
    }
    va_end(args);
    return 1;
}

int JFYSF() {
    return isEqual(PLUS) || isEqual(MINU);
}

int CFYSF() {
    return isEqual(MULT) || isEqual(DIV);
}

int GXYSF() {
    return isEqual(LSS) || isEqual(LEQ) || isEqual(GRE) || isEqual(GEQ) || isEqual(EQL) || isEqual(NEQ);
}

int ZFC() {
    if(isEqual(STRCON)) {
        printToken();
        //outfile <<  "<字符串>" << endl;
        printTag(ZFCid);
        return 1;
    }
    return 0;
}

int CX() {
    getsym();
    squareBracket(1,CLSMid);
    //squareBracket(1,BLSMid);
    if (isEqual(INTTK) || isEqual(CHARTK)) {
        angleBracket(BZFid);
        updateSymbleTable();
        isRead_LXBZF_BZF = 1;
        if (!(symbol == LPARENT)) {
            angleBracket(SHORT_BLSMid);
            squareBracket(1,SHORT_YFHZHSDYid);
            //bigBracket(1,YFHZHSDYid);
        }
        else {
            angleBracket(SHORT_YFHZHSDYid);
            //bigBracket(1,YFHZHSDYid);
        }
    }
    //bigBracket(1,WFHZHSDYid);
    while (justyangleBracket(YFHZHSDYid) || justyangleBracket(WFHZHSDYid));
    angleBracket(ZHSid);
    printToken();
    printTag(CXid);
    //outfile<<"<程序>"<<std::endl;
    return 1;
}

int CONSTTK_CLDY_SEMICN() {
    if (isEqual(CONSTTK)) {
        angleBracket(CLDYid);
        if (mustExist(SEMICN)) {
            return 1;
        }
    }
    return 0;
}
int CLSM() {
    if (justyangleBracket(CONSTTK_CLDY_SEMICNid)) {
        bigBracket(1,CONSTTK_CLDY_SEMICNid);
        printToken();
        //outfile<<"<常量说明>"<<std::endl;
        printTag(CLSMid);
        return 1;
    }
    return 0;
}

int COMMA_BZF_ASSIGN_ZS() {
    if (isEqual(COMMA)) {
        angleBracket(BZFid);
        updateSymbleTable();
        mustExist(ASSIGN);
        if (!justyangleBracket(ZSid)) {
            getSyms();
            checkError(ERROR_TYPE_O,0);
        }
        else {
            setMcodeTableValue(ZS_value);
        }
        return 1;
    }
    return 0;
}

int COMMA_BZF_ASSIGN_CHARCON() {
    if (isEqual(COMMA)) {
        angleBracket(BZFid);
        updateSymbleTable();
        mustExist(ASSIGN);
        setMcodeTableValue(int(token[0]));
        if (!isEqual(CHARCON)) {
            getSyms();
            checkError(ERROR_TYPE_O,0);
        }
        return 1;
    }
    return 0;
}

int CLDY() {
    if (isEqual(INTTK)) {
        angleBracket(BZFid);
        updateSymbleTable();
        mustExist(ASSIGN);
        if (!justyangleBracket(ZSid)) {
            getSyms();
            checkError(ERROR_TYPE_O,0);
        }
        else {
            setMcodeTableValue(ZS_value);
        }
        bigBracket(1,COMMA_BZF_ASSIGN_ZSid);
        printToken();
        //outfile<<"<常量定义>"<<std::endl;
        printTag(CLDYid);
        return 1;
    }
    else if(isEqual(CHARTK)) {
        angleBracket(BZFid);
        updateSymbleTable();
        mustExist(ASSIGN);
        setMcodeTableValue(int(token[0]));
        if (!isEqual(CHARCON)) {
            getSyms();
            checkError(ERROR_TYPE_O,0);
        }
        bigBracket(1,COMMA_BZF_ASSIGN_CHARCONid);
        printToken();
        //outfile<<"<常量定义>"<<std::endl;
        printTag(CLDYid);
        return 1;
    }
    return 0;
}

int WFHZS() {
    char tempToken[MAXTXTLENGTH] = {0};
    strcpy(tempToken,token);
    if (isEqual(INTCON)) {
        printToken();
        printTag(WFHZSid);
        ZS_value = atoi(tempToken);
        return 1;
    }
    return 0;
}

int ZS() {
    char tempToken[MAXTXTLENGTH] = {0};
    strcpy(tempToken,token);
    if (justyangleBracket(JFYSFid)) {
        strcat(tempToken,token);
    }
    if (justyangleBracket(WFHZSid)) {
        printToken();
        //outfile<<"<整数>"<<std::endl;
        printTag(ZSid);
        ZS_value = atoi(tempToken);
        return 1;
    }
    return 0;
}

int BZF() {
    return isEqual(IDENFR);
}

int SMTB() {
    if (isEqual(INTTK)) {
        angleBracket(BZFid);
        updateSymbleTable();
        printToken();
        //outfile<<"<声明头部>"<<std::endl;
        printTag(SMTBid);
        return 1;
    }
    else if (isEqual(CHARTK)) {
        angleBracket(BZFid);
        updateSymbleTable();
        printToken();
        //outfile<<"<声明头部>"<<std::endl;
        printTag(SMTBid);
        return 1;
    }
    return 0;
}

int BLSM_SEMICN() {
    if (justyangleBracket(BLDYid)) {
        mustExist(SEMICN);
        return 1;
    }
    return 0;
}
int SHORT_BLSM() {
    if (isEqual(LBRACK)) {
        angleBracket(WFHZSid);
        insertArrayLength(ZS_value);
        insertArrayLengthSymbleTable(ZS_value);
        mustExist(RBRACK);
    }
    bigBracket(1,COMMA_BZF_WITH_BRACKETid);
    printToken();
    //outfile<<"<变量定义>"<<std::endl;
    printTag(BLDYid);
    mustExist(SEMICN);
    bigBracket(1,BLSM_SEMICNid);

    if (isRead_LXBZF_BZF) {
        printTokenRetract(2);
    }
    else {
        printToken();
    }

    //outfile<<"<变量说明>"<<std::endl;
    printTag(BLSMid);
    return 1;
}

int BLSM() {
    if (justyangleBracket(BLDYid)) {
        mustExist(SEMICN);
        bigBracket(1,BLSM_SEMICNid);
        printToken();
        //outfile<<"<变量说明>"<<std::endl;
        printTag(BLSMid);
        return 1;
    }
    return 0;
}

int COMMA_BZF_WITH_BRACKET() {
    if (isEqual(COMMA)) {
        angleBracket(BZFid);
        updateSymbleTable();
        if (isEqual(LBRACK)) {
            angleBracket(WFHZSid);
            insertArrayLength(ZS_value);
            insertArrayLengthSymbleTable(ZS_value);
            mustExist(RBRACK);
        }
        return 1;
    }
    return 0;
}

int BLDY() {
    isRead_LXBZF_BZF = 0;
    if (justyangleBracket(LXBZFid)) {
        angleBracket(BZFid);
        updateSymbleTable();
        if (isEqual(LBRACK)) {
            angleBracket(WFHZSid);
            insertArrayLength(ZS_value);
            insertArrayLengthSymbleTable(ZS_value);
            mustExist(RBRACK);
        }
        bigBracket(1,COMMA_BZF_WITH_BRACKETid);

        if (symbol != LPARENT) {
            printToken();
            //outfile << "<变量定义>" << std::endl;
            printTag(BLDYid);
            isRead_LXBZF_BZF = 0;
            return 1;
        }
        isRead_LXBZF_BZF = 1;
    }
    return 0;
}

int LXBZF() {
    return isEqual(INTTK) || isEqual(CHARTK);
}

int SHORT_YFHZHSDY() {
    if (isRead_LXBZF_BZF) {
        initReturnType();
        printToken();
        //outfile << "<声明头部>" << std::endl;
        printTag(SMTBid);
        if (isEqual(LPARENT)) {
            angleBracket(CSBid);
            mustExist(RPARENT);
            mustExist(LBRACE);
            angleBracket(FHYJid);
            checkError(ERROR_TYPE_H,0);
            mustExist(RBRACE);
            printToken();
            //outfile << "<有返回值函数定义>" << std::endl;
            printTag(YFHZHSDYid);
            updateSymbleTable();
            return 1;
        }
    }
    return 0;
}

int YFHZHSDY() {
    if (justyangleBracket(SMTBid)) {
        initReturnType();
        mustExist(LPARENT);
        angleBracket(CSBid);
        mustExist(RPARENT);
        mustExist(LBRACE);
        angleBracket(FHYJid);
        checkError(ERROR_TYPE_H,0);
        mustExist(RBRACE);
        printToken();
        //outfile<<"<有返回值函数定义>"<<std::endl;
        printTag(YFHZHSDYid);
        updateSymbleTable();
        return 1;
    }
    return 0;
}

int WFHZHSDY() {
    if (isEqual(VOIDTK) && justyangleBracket(BZFid)) {
        initReturnType();
        updateSymbleTable();
        mustExist(LPARENT);
        angleBracket(CSBid);
        mustExist(RPARENT);
        mustExist(LBRACE);
        angleBracket(FHYJid);
        mustExist(RBRACE);
        printToken();
        //outfile<<"<无返回值函数定义>"<<std::endl;
        printTag(WFHZHSDYid);
        updateSymbleTable();
        return 1;
    }
    return 0;
}

int FHYJ() {
    if (squareBracket(1,CLSMid)) {
        squareBracket(1,BLSMid);
        angleBracket(YJLid);
        printToken();
        //outfile<<"<复合语句>"<<std::endl;
        printTag(FHYJid);
        return 1;
    }
    return 0;
}

int COMMA_LXBZF_BZF() {
    if (isEqual(COMMA)) {
        angleBracket(LXBZFid);
        angleBracket(BZFid);
        updateSymbleTable();
        return 1;
    }
    return 0;
}
int CSB() {
    isCSB = 1;
    if (justyangleBracket(LXBZFid)) {
        angleBracket(BZFid);
        updateSymbleTable();
        bigBracket(1,COMMA_LXBZF_BZFid);
        printToken();
        //outfile<<"<参数表>"<<std::endl;
        printTag(CSBid);
        isCSB = 0;
        return 1;
    }
    printToken();
    //outfile<<"<参数表>"<<std::endl;
    printTag(CSBid);
    isCSB = 0;
    return 1;
}

int ZHS() {
    if (isEqual(MAINTK)) {
        updateSymbleTable();
        if (mustExist(LPARENT)) {
            if (mustExist(RPARENT)) {
                if (mustExist(LBRACE)) {
                    justyangleBracket(FHYJid);
                    if (mustExist(RBRACE)) {
                        printToken();
                        //outfile<<"<主函数>"<<std::endl;
                        printTag(ZHSid);
                        updateSymbleTable();
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}

int JFYSF_X() {
    int op = 0;
    if (strcmp(token,"-") == 0) {
        op = 1;
    }
    if (justyangleBracket(JFYSFid)) {
        setNegMidCodeItem(op);
        getBDStype(EXP_TYPE_INT);
        angleBracket(Xid);
        return 1;
    }
    return 0;
}

int BDS() {
    createBDSType();
    createMidCodeExpression();
    int op = 0;
    if (strcmp(token,"-") == 0) {
        op = 1;
    }
    squareBracket(1,JFYSFid);
    setNegMidCodeItem(op);
    if (justyangleBracket(Xid)) {
        //mustExist(ASSIGN);
        bigBracket(1,JFYSF_Xid);
        printToken();
        //outfile<<"<表达式>"<<std::endl;
        printTag(BDSid);
        endBDSType();
        deleteMidCodeExpression();
        return 1;
    }
    endBDSType();
    deleteMidCodeExpression();
    return 0;
}

int CFYSF_YZ() {
    int isdivi = 0;
    if (token[0] == '/') {
        isdivi = 1;
    }
    if (justyangleBracket(CFYSFid)) {
        getXtype(EXP_TYPE_INT);
        angleBracket(YZid);
        if (isdivi) {
            setNegMidCodeFactor(&tempmcf,isdivi);
        }
        addMidCodeItem(tempmcf);
        //outfile<<"<因子>"<<std::endl;
        return 1;
    }
    return 0;
}

int X() {
    if (justyangleBracket(YZid)) {
        addMidCodeItem(tempmcf);
        bigBracket(1,CFYSF_YZid);
        addMidCodeExpression();
        printToken();
        //outfile<<"<项>"<<std::endl;
        printTag(Xid);
        return 1;
    }
    return 0;
}

int YZ() {
    MidCodeFactor mcf;
    char temptoken[MAXTXTLENGTH];
    strcpy(temptoken,token);
    if(justyangleBracket(BZFid)) {
        checkError(ERROR_TYPE_C,0);
        getYZTyoe(EXP_TYPE_NULL);
        if (isEqual(LBRACK)) {
            angleBracket(BDSid);
            checkError(ERROR_TYPE_I,0);
            mustExist(RBRACK);
            mcf = createMidCodeFactor(FACTOR_BZF_ARRAY,0,temptoken);
        }
        else if(justyangleBracket(YFHZHSDYYJid)) {
            mcf = createMidCodeFactor(FACTOR_HS,0,temptoken);
        }
        else {
            mcf = createMidCodeFactor(FACTOR_BZF,0,temptoken);
        }
        printToken();
        //outfile<<"<因子>"<<std::endl;
        printTag(YZid);
        tempmcf = mcf;
        return 1;
    }
    else if(isEqual(LPARENT)) {
        getYZTyoe(EXP_TYPE_INT);
        angleBracket(BDSid);
        mcf = createMidCodeFactor(FACTOR_BDS,0,NULL);
        mustExist(RPARENT);
        printToken();
        //outfile<<"<因子>"<<std::endl;
        printTag(YZid);
        tempmcf = mcf;
        return 1;
    }
    else if(justyangleBracket(ZSid)) {
        mcf = createMidCodeFactor(FACTOR_ZS,ZS_value,NULL);
        getYZTyoe(EXP_TYPE_INT);
        printToken();
        //outfile<<"<因子>"<<std::endl;
        printTag(YZid);
        tempmcf = mcf;
        return 1;
    }
    else if(isEqual(CHARCON)){
        mcf = createMidCodeFactor(FACTOR_ZF,temptoken[0],NULL);
        getYZTyoe(EXP_TYPE_CHAR);
        printToken();
        //outfile<<"<因子>"<<std::endl;
        printTag(YZid);
        tempmcf = mcf;
        return 1;
    }
    return 0;
}

int YJ() {
    char temptoken[20];
    strcpy(temptoken,token);
    if(justyangleBracket(TJYJid)) {
        printToken();
        //outfile<<"<语句>"<<std::endl;
        printTag(YJid);
        return 1;
    }
    else if(justyangleBracket(XHYJid)) {
        printToken();
        //outfile<<"<语句>"<<std::endl;
        printTag(YJid);
        return 1;
    }
    else if(isEqual(LBRACE)) {
        angleBracket(YJLid);
        mustExist(RBRACE);
        printToken();
        //outfile<<"<语句>"<<std::endl;
        printTag(YJid);
        return 1;
    }
    else if(justyangleBracket(BZFid)) {
        checkError(ERROR_TYPE_C,0);
        if (justyangleBracket(FZYJid)) {
            mustExist(SEMICN);
            printToken();
            //outfile<<"<语句>"<<std::endl;
            printTag(YJid);
            return 1;
        }
        else if(justyangleBracket(WFHZHSDYYJid)) {
            mustExist(SEMICN);
            printToken();
            //outfile<<"<语句>"<<std::endl;
            printTag(YJid);
            return 1;
        }
        else if(justyangleBracket(YFHZHSDYYJid)) {
            mustExist(SEMICN);
            printToken();
            //outfile<<"<语句>"<<std::endl;
            printTag(YJid);
            return 1;
        }
    }
    else if(justyangleBracket(DYJid)) {
        mustExist(SEMICN);
        printToken();
        //outfile<<"<语句>"<<std::endl;
        printTag(YJid);
        return 1;
    }
    else if(justyangleBracket(XYJid)) {
        mustExist(SEMICN);
        printToken();
        //outfile<<"<语句>"<<std::endl;
        printTag(YJid);
        return 1;
    }
    else if(isEqual(SEMICN)) {
        printToken();
        //outfile<<"<语句>"<<std::endl;
        printTag(YJid);
        return 1;
    }
    else if(justyangleBracket(FANHYJid)) {
        mustExist(SEMICN);
        printToken();
        //outfile<<"<语句>"<<std::endl;
        printTag(YJid);
        return 1;
    }
    return 0;
}

int FZYJ() {
    int isinit = 0;
    if (isEqual(LBRACK)) {
        initAssignMcodeTable();
        isinit = 1;
        angleBracket(BDSid);
        setIndexAssignMcodeTable();
        checkError(ERROR_TYPE_I, 0);
        mustExist(RBRACK);
    }
    if (isEqual(ASSIGN)) {
        if (isinit == 0) {
            initAssignMcodeTable();
        }
        checkError(ERROR_TYPE_J, 0);
        angleBracket(BDSid);
        setValueAssignMcodeTable();
        printToken();
        //outfile << "<赋值语句>" << std::endl;
        printTag(FZYJid);
        return 1;
    }
    return 0;
}

int ELSETK_YJ() {
    if (isEqual(ELSETK)) {
        angleBracket(YJid);
        return 1;
    }
    return 0;
}

int TJYJ() {
    if (isEqual(IFTK)) {
        mustExist(LPARENT);
        angleBracket(TJid);
        int index1 = jumpMcodeTable(JUMP_IF);
        mustExist(RPARENT);
        angleBracket(YJid);
        int index2;
        int flag = 0;
        if (symbol == ELSETK) {
            index2 = jumpMcodeTable(JUMP);
            flag = 1;
        }
        int label1 = labelMcodeTable();
        squareBracket(1,ELSETK_YJid,1);
        printToken();
        //outfile<<"<条件语句>"<<std::endl;
        printTag(TJYJid);
        fillInJumpMcodeTable(index1, label1);
        if (flag) {
            int label2 = labelMcodeTable();
            fillInJumpMcodeTable(index2, label2);
        }
        return 1;
    }
    return 0;
}

int TJ() {
    if (justyangleBracket(BDSid)) {
        int i = checkError(ERROR_TYPE_F,EXP_TYPE_INT);
        if (justyangleBracket(GXYSFid)) {
            initEqualMcodeTable();
            angleBracket(BDSid);
            setEqualMcodeTable();
            if (i = 0) {
                checkError(ERROR_TYPE_F, EXP_TYPE_INT);
            }
        }
        else {
            setEqualZeroMcodeTable();
        }
        printToken();
        //outfile<<"<条件>"<<std::endl;
        printTag(TJid);
        return 1;
    }
    return 0;
}

int XHYJ() {
    if (isEqual(WHILETK)) {
        addStartLoop();
        mustExist(LPARENT);
        int label2 = labelMcodeTable();
        angleBracket(TJid);
        int index1 = jumpMcodeTable(JUMP_IF);
        mustExist(RPARENT);
        angleBracket(YJid);
        printToken();
        //outfile<<"<循环语句>"<<std::endl;
        printTag(XHYJid);
        int index2 = jumpMcodeTable(JUMP);
        int label1 = labelMcodeTable();
        fillInJumpMcodeTable(index1, label1);
        fillInJumpMcodeTable(index2, label2);
        addEndLoop();
        return 1;
    }
    else if(isEqual(DOTK)) {
        addStartLoop();
        int label1 = labelMcodeTable();
        angleBracket(YJid);
        mustExist(WHILETK);
        mustExist(LPARENT);
        angleBracket(TJid);
        mustExist(RPARENT);
        printToken();
        //outfile<<"<循环语句>"<<std::endl;
        printTag(XHYJid);
        int index1 = jumpMcodeTable(JUMP_DO);
        fillInJumpMcodeTable(index1, label1);
        addEndLoop();
        return 1;
    }
    else if(isEqual(FORTK)) {
        addStartLoop();
        char temptoken1[MAX_NAME_LENGTH] = {0};
        char temptoken2[MAX_NAME_LENGTH] = {0};
        int value = 0;
        mustExist(LPARENT);
        angleBracket(BZFid);
        checkError(ERROR_TYPE_C,0);
        mustExist(ASSIGN);
        initAssignMcodeTable();
        checkError(ERROR_TYPE_J,0);
        angleBracket(BDSid);
        setValueAssignMcodeTable();
        int label2 = labelMcodeTable();
        mustExist(SEMICN);
        angleBracket(TJid);
        int index1 = jumpMcodeTable(JUMP_IF);
        mustExist(SEMICN);
        strcpy(temptoken1, token);
        angleBracket(BZFid);
        checkError(ERROR_TYPE_C,0);
        mustExist(ASSIGN);
        checkError(ERROR_TYPE_J,0);
        strcpy(temptoken2, token);
        angleBracket(BZFid);
        checkError(ERROR_TYPE_C,0);
        int isneg = 0;
        if (symbol == MINU) {
            isneg = 1;
        }
        angleBracket(JFYSFid);
        angleBracket(BCid);
        value = ZS_value;
        mustExist(RPARENT);
        angleBracket(YJid);
        printToken();
        //outfile<<"<循环语句>"<<std::endl;
        printTag(XHYJid);
        forAdd(temptoken1, temptoken2, (isneg) ? value * -1 : value);
        int index2 = jumpMcodeTable(JUMP);
        int label1 = labelMcodeTable();
        fillInJumpMcodeTable(index1, label1);
        fillInJumpMcodeTable(index2, label2);
        addEndLoop();
        return 1;
    }
    return 0;
}

int BC() {
    if (angleBracket(WFHZSid)) {
        printToken();
        //outfile<<"<步长>"<<std::endl;

        printTag(BCid);
        return 1;
    }
    return 0;
}

int YFHZHSDYYJ() {
    if (isEqual(LPARENT)) {
        angleBracket(ZCSBid);
        mustExist(RPARENT);
        printToken();
        //outfile << "<有返回值函数调用语句>" << std::endl;
        printTag(YFHZHSDYYJid);
        return 1;
    }
    return 0;
}

int WFHZHSDYYJ() {
    mustExist(LPARENT);
    angleBracket(ZCSBid);
    mustExist(RPARENT);
    printToken();
    //outfile<<"<无返回值函数调用语句>"<<std::endl;
    printTag(WFHZHSDYYJid);
    return 1;
}


int COMMA_BDS() {
    if(isEqual(COMMA)) {
        angleBracket(BDSid);
        pushMcodeTable();
        checkError(ERROR_TYPE_E,ZCSBl);
        ZCSBl += 1;
        return 1;
    }
    return 0;
}

int ZCSB() {
    recentHSDYYJ = tokenstop - 2;
    int temp = recentHSDYYJ;
    int temp2 = ZCSBl;
    ZCSBl = 0;
    //startpushMcodeTable();
    if (justyangleBracket(BDSid)) {
        pushMcodeTable();
        checkError(ERROR_TYPE_E,ZCSBl);
        ZCSBl += 1;
        bigBracket(1,COMMA_BDSid);
    }
    //printf("%d\n",recentHSDYYJ);
    recentHSDYYJ = temp;
    checkError(ERROR_TYPE_D,ZCSBl);
    ZCSBl = temp2;
    printToken();
    //outfile<<"<值参数表>"<<std::endl;
    printTag(ZCSBid);
    //endpushMcodeTable();
    callMcodeTable();
    return 1;
}

int YJL() {
    if (1) {
        bigBracket(1,YJid);
        printToken();
        //outfile<<"<语句列>"<<std::endl;
        printTag(YJLid);
        return 1;
    }
    return 0;
}

int COMMA_BZF() {
    if (isEqual(COMMA)) {
        char temptoken[MAX_NAME_LENGTH];
        strcpy(temptoken,token);
        angleBracket(BZFid);
        scanfPushMcodeTable(temptoken);
        checkError(ERROR_TYPE_C,0);
        return 1;
    }
    return 0;
}

int DYJ() {
    if (isEqual(SCANFTK)) {
        mustExist(LPARENT);
        char temptoken[MAX_NAME_LENGTH];
        strcpy(temptoken,token);
        angleBracket(BZFid);
        scanfPushMcodeTable(temptoken);
        checkError(ERROR_TYPE_C,0);
        bigBracket(1,COMMA_BZFid);
        mustExist(RPARENT);
        printToken();
        //outfile<<"<读语句>"<<std::endl;
        printTag(DYJid);
        scanfCallMcodeTable();
        return 1;
    }
    return 0;
}

int XYJ() {
    if (isEqual(PRINTFTK)) {
        mustExist(LPARENT);
        newPrintfMcodeTable();
        char temptoken[MAXTXTLENGTH];
        strcpy(temptoken, token);
        if (justyangleBracket(ZFCid)) {
            printfPushStrMcodeTable(temptoken);
            if (isEqual(RPARENT)) {
                printToken();
                //outfile<<"<写语句>"<<std::endl;
                printTag(XYJid);
                addNewline();
                return 1;
            }
            else if(isEqual(COMMA)) {
            }
        }
        angleBracket(BDSid);
        printfPushVarMcodeTable();
        mustExist(RPARENT);
        printToken();
        //outfile<<"<写语句>"<<std::endl;
        printTag(XYJid);
        printfCallMcodeTable();
        addNewline();
        return 1;
    }
    return 0;
}

int LPARENT_BDS_RPARENT() {
    if (isEqual(LPARENT)) {
        angleBracket(BDSid);
        mustExist(RPARENT);

        return 1;
    }
    return 0;
}

int FANHYJ() {
    if (isEqual(RETURNTK)) {
        if (justyangleBracket(LPARENT_BDS_RPARENTid)) {
            checkError(ERROR_TYPE_G,0);
            addReturnType(1);
            returnValueMcodeTable();
        }
        else {
            addReturnType(0);
            returnNOValueMcodeTable();
        }
        printToken();
        //outfile<<"<返回语句>"<<std::endl;
        printTag(FANHYJid);
        return 1;
    }
    return 0;
}
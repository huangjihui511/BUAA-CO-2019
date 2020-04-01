//
// Created by 黄继辉 on 2019-09-29.
//

#ifndef GRAMMER_ANALYSE_GRAMMARANALYZER_H
#define GRAMMER_ANALYSE_GRAMMARANALYZER_H




#define JFYSFid 0
#define CFYSFid 1
#define GXYSFid 2
#define ZFCid 3
#define CXid 4
#define CLSMid 5
#define CLDYid 6
#define WFHZSid 7
#define ZSid 8
#define BZFid 9
#define SMTBid 10
#define BLSMid 11
#define BLDYid 12
#define LXBZFid 13
#define YFHZHSDYid 14
#define WFHZHSDYid 15
#define FHYJid 16
#define CSBid 17
#define ZHSid 18
#define BDSid 19
#define Xid 20
#define YZid 21
#define YJid 22
#define FZYJid 23
#define TJYJid 24
#define TJid 25
#define XHYJid 26
#define BCid 27
#define YFHZHSDYYJid 28
#define WFHZHSDYYJid 29
#define ZCSBid 30
#define YJLid 31
#define DYJid 32
#define XYJid 33
#define FANHYJid 34

#define CONSTTK_CLDY_SEMICNid 35
#define COMMA_BZF_ASSIGN_ZSid 36
#define COMMA_BZF_ASSIGN_CHARCONid 37
#define BLSM_SEMICNid 38
#define COMMA_LXBZF_BZFid 39
#define JFYSF_Xid 40
#define CFYSF_YZid 41
#define ELSETK_YJid 42
#define COMMA_BDSid 43
#define COMMA_BZFid 44
#define SHORT_BLSMid 45
#define SHORT_YFHZHSDYid 46
#define LPARENT_BDS_RPARENTid 47
#define COMMA_BZF_WITH_BRACKETid 48

void grammarError();

int isEqual(int cmp);

int mustExist(int cmp);

int justyangleBracket(int id);

int angleBracket(int id);

int squareBracket(int count, ...);

int bigBracket(int count,...);

int JFYSF();

int CFYSF();

int GXYSF();

int ZFC();

int CX();

int CLSM();

int CLDY();

int WFHZS();

int ZS();

int BZF();

int SMTB();

int BLSM();

int BLDY();

int LXBZF();

int YFHZHSDY();

int WFHZHSDY();

int FHYJ();

int CSB();

int ZHS();

int BDS();

int X();

int YZ();

int YJ();

int FZYJ();

int TJYJ();

int TJ();

int XHYJ();

int BC();

int YFHZHSDYYJ();

int WFHZHSDYYJ();

int ZCSB();

int YJL();

int DYJ();

int XYJ();

int FANHYJ();

int CONSTTK_CLDY_SEMICN();

int COMMA_BZF_ASSIGN_ZS();

int COMMA_BZF_ASSIGN_CHARCON();

int BLSM_SEMICN();

int COMMA_LXBZF_BZF();

int JFYSF_X();

int CFYSF_YZ();

int ELSETK_YJ();

int COMMA_BDS();

int COMMA_BZF();

int SHORT_BLSM();

int SHORT_YFHZHSDY();

int LPARENT_BDS_RPARENT();

int COMMA_BZF_WITH_BRACKET();

static int (*grammerMethods[100])();

void grammarAnalyse();

void getSyms();

#endif //GRAMMER_ANALYSE_GRAMMARANALYZER_H

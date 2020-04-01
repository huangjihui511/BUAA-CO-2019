#include <iostream>
#include "LexicalAnalyzer.h"
#include "GrammarAnalyzer.h"
#include "Printer.h"
#include "GenerateMips.h"
#include "Optimal.h"
#include <fstream>

using namespace std;
void read_testfile();
void open_files();

char tempchar;
char token[MAXTXTLENGTH];
int symbol;
int token_top;
int inputtxt_top;
int inputlength;
char inputtxt[MAXTXTLENGTH * 100];
int isCorrect = 1;
ofstream outfile;
ofstream errorfile;
ofstream middlecodefile;
ofstream mips;

int submit = 0;

int main() {
    read_testfile();
    open_files();
    grammarAnalyse();
    if (isCorrect)
        optimalMain();
    //scanMidCode();
    return 0;
}

void open_files() {
    outfile.open("output.txt");
    errorfile.open("error.txt");
    middlecodefile.open("middleCode.txt");
    mips.open("mips.txt");
}

void read_testfile() {
    char filepath[] = "/Volumes/文档/大学/大学学习/大三上/编译原理/课设作业/optimal/testfile.txt";
    if (submit) {
        strcpy(filepath, "testfile.txt");
    }
    FILE *IN;
    IN = fopen(filepath,"r");
    int i = 0;
    char c;
    for (;(c = fgetc(IN)) != EOF;i++) {
        inputtxt[i] = c;
    }
    inputtxt[i] = '\0';
    inputlength = i;
}

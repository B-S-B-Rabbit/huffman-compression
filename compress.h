#ifndef COMPRESS_H
#define COMPRESS_H

#define CODE_SIZE 10000
#define BIT8 8
#define SIZE_MAS 80

typedef struct finalTree{
    char val;
    unsigned char symb;
    int isSymbol;
    struct finalTree *left;
    struct finalTree *right;
}node2;

typedef union bit2char {
    unsigned  char code;
    struct bit {
        unsigned b1 : 1;
        unsigned b2 : 1;
        unsigned b3 : 1;
        unsigned b4 : 1;
        unsigned b5 : 1;
        unsigned b6 : 1;
        unsigned b7 : 1;
        unsigned b8 : 1;
    }mbit;
}BIT2CHAR;

typedef struct node {
    unsigned char symbol; // ascii-символ
    unsigned char isSymbol;
    unsigned int freq;
    unsigned char code[CODE_SIZE]; // строка с двоичным кодом символа
    struct node *left, *right, *next;
}NODE;

typedef struct table{
    unsigned char code[255];
    unsigned char symbol;
    unsigned char isSymbol;
}tab;

void Add2List(NODE** ppHead, unsigned int freq, int symbol);
void Add2List2(NODE** pHead, NODE* res);
NODE * MakeNodeFromNode(NODE * left, NODE * right);
NODE * MakeTreeFromList(NODE * head);
void symmetric(NODE *root, int level);
void symmetric2(NODE *root, char bitString[256], int level, char value, tab *table);
void initTable(tab ** table);
int writingStructure(FILE * fp,tab * st, unsigned int n, unsigned int tail, int symbolUsed);
FILE * openFileForCompression(long long int *length, char * fileNameComp);
void compress();

#endif

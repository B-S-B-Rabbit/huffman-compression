#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compress.h"

void Add2List(NODE** ppHead, unsigned int freq, int symbol)
{
    while(*ppHead)
    {
        if ((*ppHead)-> freq >= freq)
            break;
        ppHead = &((*ppHead)-> next);
    }
    NODE* pNew = (NODE*)malloc(sizeof(NODE));
    memset(pNew->code, 9, CODE_SIZE);
    pNew->freq = freq;
    pNew->symbol = symbol;
    pNew->isSymbol = 1;
    pNew->next = *ppHead;
    pNew->left = NULL;
    pNew->right = NULL;
    *ppHead = pNew;
}

void Add2List2(NODE** pHead, NODE* res)
{
    while(*pHead)
    {
        if ((*pHead)-> freq >= res->freq) break;
        pHead = &((*pHead)-> next);
    }
    res->next = *pHead;
    *pHead = res;
}

NODE * MakeNodeFromNode(NODE * left, NODE * right) // процедура создания узла из первых двух членов списка, возвращающая созданный узел
{
    NODE * res = (NODE*)malloc(sizeof(NODE));
    res->freq = left->freq + right->freq;
    res->isSymbol = 0;
    res->symbol = 0;
    res->left = left;
    res->right = right;
    res->next = 0;
    return res;
}

NODE * MakeTreeFromList(NODE * head)
{
    while (head && head->next)
    {
        NODE * left = head;
        NODE * right = head->next;
        Add2List2(&(head->next->next), MakeNodeFromNode(left, right));
        head = head->next->next;
    }
    return head;
}

int max=0;
void symmetric(NODE *root, int level)
{
    if (root)
    {
        if (root->isSymbol)
        {
            if (level > max)
            {
                max = level;
            }
        }
        symmetric(root->left, level + 1);
        symmetric(root->right, level + 1);
    }
}

void symmetric2(NODE *root, char bitString[256], int level, char value, tab *table){
    int c = 1;
    if (root)
    {
        bitString[level] = value;
        if (root->isSymbol)
        {
            table[root->symbol].symbol =root->symbol;
            table[root->symbol].isSymbol = 1;
            if (level == 0)
            {
                table[root->symbol].code[0] = '1';
                table[root->symbol].code[1] = '\0';
            }
            else {
                for (int i = 1; i < level + 1; i++) {
                    table[root->symbol].code[i - 1] = bitString[i];
                    c = i + 1;
                }
                table[root->symbol].code[c - 1] = '\0';
            }
        }
        symmetric2(root->left, bitString, level + 1, '0', table);
        symmetric2(root->right, bitString, level + 1, '1', table);
    }
}

void initTable(tab ** table){
    int len = 256;
    *table = (tab*) malloc(len * sizeof(tab));
    for (int i = 0; i < len; i++){
        (*table)[i].symbol = 0;
        (*table)[i].isSymbol = 0;
        strcpy((char *)(*table)[i].code, "");
    }
}

int writingStructure(FILE * fp,tab * st, unsigned int n, unsigned int tail, int symbolUsed)
{
    unsigned char *c;
    unsigned int size = n * sizeof(tab);
    c = (unsigned char *)&tail;
    for (int i = 0; i<sizeof(unsigned int); i++)
    {
        putc(*c++, fp);
    }
    c = ( unsigned char *)&symbolUsed;
    for (int i = 0; i<sizeof(unsigned int); i++)
    {
        putc(*c++, fp);
    }
    // посимвольно записываем в файл все структуры
    c = (unsigned char *)st;
    for (int i = 0; i < size; i++)
    {
        if (st[i / sizeof(tab)].isSymbol != 0) {
            fputc(*c++, fp);
            continue;
        }
        c++;
    }
    return 0;
}

FILE * openFileForCompression(long long int *length, char * fileNameComp){
    FILE * fr = fopen(fileNameComp, "rb");
    if (!fr)
    {
        printf("File can't be open!");
        exit(0);
    }
    fseek(fr, 0L, SEEK_END); //end of file
    (*length) = ftell(fr); // length of file
    return fr;
}

void compress(){
    long long int  length = 0; // length file for compression
    char * fileNameComp = (char*) calloc(100, sizeof(char));
    printf("Input file name:\n");
    scanf("%s", fileNameComp);
    FILE * fr = openFileForCompression(&length, fileNameComp);
    unsigned int i1 = strlen(fileNameComp);
    while (fileNameComp[i1] != '.')
    {
        i1 -= 1;
    }
    char * fileNameNew = (char*) calloc(100, sizeof(char));
    strncpy(fileNameNew, fileNameComp, i1);
    strcpy(fileNameNew + i1,".dat\0" );
    FILE *fw=fopen(fileNameNew,"wb+");
    fseek(fr, 0, SEEK_SET);
    unsigned freq[256] = {0};
    for (int i = 0; i < length; ++i) //count of each symbol
    {
        unsigned char a = (unsigned char) fgetc(fr);
        freq[a]++;
    }
    NODE * pHead = NULL;
    NODE * finalTree = NULL;
    int symbolsUsed = 0;
    for (int  i = 0; i < 256; ++i)
    {
        if (freq[i] != 0)
        {
            symbolsUsed++;
            Add2List(&pHead, freq[i], i);
        }
    }
    finalTree = MakeTreeFromList(pHead); // tree from list
    char bitString[256] = {-1};
    symmetric(finalTree, 0);
    tab *table = NULL;
    initTable(&table);
    symmetric2(finalTree, bitString, 0, '9', table);
    fseek(fr, 0, SEEK_SET);
    unsigned int lenCodeStr = 0;
    for (int i = 0; i < length; i++)
    {
        unsigned char symbol = (unsigned char)fgetc(fr);
        lenCodeStr += strlen((const char *) table[symbol].code);
    }
    unsigned int count = lenCodeStr / BIT8;
    unsigned int tail = lenCodeStr % BIT8;
    unsigned int len = count;
    if (tail != 0)
        len = count + 1;
    fseek(fr, 0, SEEK_SET);
    BIT2CHAR symb;
    unsigned char *c;
    c=(unsigned char*)&len;
    for (int i = 0; i < sizeof(unsigned int); i++)
    {
        putc(*c++, fw); // запись байтов в память
    }
    c = (unsigned char*)&length;
    for (int i = 0; i < sizeof(unsigned int); i++) // т.к. переменная типа int
    {
        putc(*c++, fw); // запись байтов в память
    }
    writingStructure(fw, table, 256, tail, symbolsUsed); // запись структуры в файл
    unsigned int k = 0;
    unsigned char * codeStr = (unsigned char *)calloc(SIZE_MAS , sizeof(unsigned char));
    for (int i = 0; i < length; i++)
    {
        unsigned char symbol = (unsigned char)fgetc(fr);

        if (strlen((const char *)table[symbol].code) + k >= SIZE_MAS)
        {
            strncpy((char*)codeStr + k, (char*)table[symbol].code, SIZE_MAS - k);
            int lenBit = SIZE_MAS / BIT8;
            for (int j = 0; j < lenBit; ++j)
            {
                symb.mbit.b1 = codeStr[j * BIT8 + 0] - '0';
                symb.mbit.b2 = codeStr[j * BIT8 + 1] - '0';
                symb.mbit.b3 = codeStr[j * BIT8 + 2] - '0';
                symb.mbit.b4 = codeStr[j * BIT8 + 3] - '0';
                symb.mbit.b5 = codeStr[j * BIT8 + 4] - '0';
                symb.mbit.b6 = codeStr[j * BIT8 + 5] - '0';
                symb.mbit.b7 = codeStr[j * BIT8 + 6] - '0';
                symb.mbit.b8 = codeStr[j * BIT8 + 7] - '0';
                fputc(symb.code, fw);
            }
            memset(codeStr, 0, SIZE_MAS);
            if (strlen((const char *)table[symbol].code) + k == SIZE_MAS){
                k = 0;
            }else{
                unsigned int z = (SIZE_MAS - k);
                strcpy((char *)codeStr, (char*)table[symbol].code + z);
                k = (strlen((const char *)table[symbol].code) - (SIZE_MAS - k));}
        }
        else
        {
            strcpy((char *) codeStr + k, (char *) table[symbol].code);
            k += strlen((const char *) table[symbol].code);
        }
    }
    unsigned int lenBit = k / BIT8;
    unsigned int tail_ = k % BIT8;
    if (tail_ != 0)
        lenBit++;
    if (lenBit != 0)
    {
        for (int i = 0; i < lenBit; ++i)
        {
            symb.mbit.b1 = codeStr[i * BIT8 + 0] - '0';
            symb.mbit.b2 = codeStr[i * BIT8 + 1] - '0';
            symb.mbit.b3 = codeStr[i * BIT8 + 2] - '0';
            symb.mbit.b4 = codeStr[i * BIT8 + 3] - '0';
            symb.mbit.b5 = codeStr[i * BIT8 + 4] - '0';
            symb.mbit.b6 = codeStr[i * BIT8 + 5] - '0';
            symb.mbit.b7 = codeStr[i * BIT8 + 6] - '0';
            symb.mbit.b8 = codeStr[i * BIT8 + 7] - '0';
            fputc(symb.code, fw);
        }
    }
    fclose(fw);
    printf("\nDone\nThe file was saved in %s", fileNameNew);
}


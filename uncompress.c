#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "uncompress.h"
#define CODE_SIZE 10000
#define BIT8 8
#define SIZE_MAS 80

node2 *addToTree(node2 *root, const char str[1000], int index, int level, tab *arrOfBits){
    int isSymbol = 0;
    if (root == NULL)
    {
        level--;
        root = (node2*)malloc(sizeof(node2));
        root->val = str[level];
        if (str[level + 1] == '\0')
        {
            isSymbol = 1;
            root->symb = arrOfBits[index].symbol;
        }
        root->isSymbol = isSymbol;
        root->left = root->right = NULL;
        level++;
        if (str[level] != '\0') addToTree(root, str, index, level, arrOfBits);
    }
    else
    {
        if (str[level] == '0')
        {
            root->left = addToTree(root->left, str, index, level+1, arrOfBits);
        }
        else if (str[level] == '1')
        {
            root->right = addToTree(root->right, str, index, level+1, arrOfBits);
        }
    }
    return root;
}

node2 *createTree(node2 *root){
    root = (node2*)malloc(sizeof(node2));
    root->left = root->right = NULL;
    return root;
}

node2 * fillTree(node2 *root, tab *arrOfBits, unsigned int kBits){
    root = createTree(root);
    char str[1000] = {'0'};
    for (int i = 0; i < kBits; i++){
        strcpy(str, (const char *)arrOfBits[i].code);
        root = addToTree(root, str, i, 0, arrOfBits);
    }
    return root;
}

unsigned char * symmetricGo(node2 *root, char *buf, int level){
    if (root){
        if (buf[level] == '\0'){
            if (root->isSymbol == 0)
                return NULL;
            else {
                return &root->symb;
            }
        }
        else if (buf[level] != '0' && buf[level] != '1')
            return NULL;
        else if (buf[level] == '0')
            symmetricGo(root->left, buf, level + 1);
        else if (buf[level] == '1')
            symmetricGo(root->right, buf, level + 1);
    }
    else
        return NULL;
}

int find(node2 *root, const char *codeStr, FILE* file, unsigned int count){
    char buf[1000] = {'0'};
    int pos = 0;
    int rub = 0;
    unsigned char *tmp;
    for (int i = 0; i < count; ++i){
        buf[pos++] = codeStr[i];
        tmp = symmetricGo(root, buf, 0);
        if (tmp != NULL) {
            rub = i + 1;
            putc(tmp[0], file);
            memset(buf, '\0', 1000);
            pos = 0;
        }
    }
    return rub;
}

// загрузка из файла массива структур
int loadStr(char * filename, char * nameForDec)
{
    BIT2CHAR encode ;
    FILE * fp;
    unsigned char *c;
    int m = sizeof(unsigned int);
    unsigned int tail,n, i,length;
    // выделяем память для количества структур
    unsigned int *pti = (unsigned int *)malloc(m);
    unsigned int *tailRet = (unsigned int*)calloc(m, sizeof (unsigned int ));
    unsigned int *dLen = (unsigned int*)malloc(m);
    unsigned int *length_ = (unsigned int*)malloc(m);
    fp = fopen(filename, "rb");
    fseek(fp, 0, SEEK_SET);
    c = (unsigned char *)dLen;
    while (m-->0) // выгружаем хвост из файла
    {
        i = getc(fp);
        if (i == EOF) break;
        *c++ = (unsigned char)i;
    }
    length = *dLen;
    m = sizeof(unsigned int);
    c = (unsigned char *)length_;
    while (m-->0) // выгружаем хвостж из файла
    {
        i = getc(fp);
        if (i == EOF) break;
        *c++ = (unsigned char)i;
    }
    m = sizeof(unsigned int);
    c = (unsigned char *)tailRet;
    while (m-->0) // выгружаем хвостж из файла
    {
        i = getc(fp); // первый байт
        if (i == EOF) break;
        *c++ = (unsigned char)i;
    }
    tail = *tailRet;
    m = sizeof(unsigned int);
    c = (unsigned char *)pti; //c-->pri
    while (m-->0)
    {
        i = getc(fp); // первый байт
        if (i == EOF) break;
        *c++ = (unsigned char)i;
    }
    //получаем число элементов
    n = *pti;
    // выделяем память для считанного массива структур
    tab * ptr = (tab *) malloc(n * sizeof(tab));
    c = (unsigned char *)ptr;
    for (int p = 0; p < (n * sizeof(tab)); p++)
    {
        i=getc(fp);
        *c++ = (unsigned char)i;
    }
    node2 *tree2 = NULL;
    node2 *finalTree2 = NULL;
    finalTree2 = fillTree(tree2, ptr, n);
    FILE *fwFinal=fopen(nameForDec, "wb+");
    unsigned char * encodeStr = calloc(SIZE_MAS, sizeof(unsigned char));
    unsigned int iter = 0;
    unsigned char *copy = calloc(255, sizeof(unsigned char));
    unsigned int count = 0;
    int lengthCopy = 0;
    for (unsigned int j = 0; j < length; j++)
    {
        if (count + 8 <= SIZE_MAS)
        {
            unsigned char symbol = getc(fp);
            encode.code = symbol;
            encodeStr[count] = (unsigned char) encode.mbit.b1 + '0';
            encodeStr[count + 1] = (unsigned char) encode.mbit.b2 + '0';
            encodeStr[count + 2] = (unsigned char) encode.mbit.b3 + '0';
            encodeStr[count + 3] = (unsigned char) encode.mbit.b4 + '0';
            encodeStr[count + 4] = (unsigned char) encode.mbit.b5 + '0';
            encodeStr[count + 5] = (unsigned char) encode.mbit.b6 + '0';
            encodeStr[count + 6] = (unsigned char) encode.mbit.b7 + '0';
            encodeStr[count + 7] = (unsigned char) encode.mbit.b8 + '0';
            lengthCopy++;
            count += 8;
            iter++;
        } else {
            j--;
            lengthCopy = 0;
            int end_str = find(finalTree2, (const char*)encodeStr, fwFinal, count);
            encodeStr[count] = '\0';
            count -= count - end_str;
            if (count != SIZE_MAS)
            {
                strcpy((char *) copy, (char *) encodeStr + end_str);
                memset(encodeStr, 0, SIZE_MAS);
                strcpy((char *) encodeStr, (char *) copy);
                count = strlen((const char*) copy);
            }
            else
            {
                memset(encodeStr, 0, SIZE_MAS);
                count = 0;
            }
        }
    }
    if (tail == 0)
        find(finalTree2, (const char*)encodeStr, fwFinal, count);
    else {
        encodeStr[strlen((const char *)encodeStr) - 8 + tail] = '\0';
        find(finalTree2, (const char*)encodeStr, fwFinal, count);
    }
    free(pti);
    free(ptr);
    fclose(fp);
    fclose(fwFinal);
    return 0;
}

int uncompress(){
    char * fileNameComp = (char*) calloc(100, sizeof(char));
    printf("Input file name:\n");
    scanf("%s", fileNameComp);
    FILE * f;
    f =  fopen(fileNameComp, "rb");
    if (f == NULL)
    {
        printf("File can't be open!");
        return 0;
    }
    fclose(f);
    unsigned int i1 = strlen(fileNameComp);
    while (fileNameComp[i1] != '.'){
        i1 -= 1;
    }
    char * fileNameNew = (char*) calloc(100, sizeof(char));
    strncpy(fileNameNew, fileNameComp, i1);
    strcpy(fileNameNew + i1,".dat\0" );
    char * extension = (char*) calloc(10, sizeof(char));
    char * originalName = (char*) calloc(100, sizeof(char));
    char * decompressionName = (char*) calloc(100, sizeof(char));
    strncpy(originalName, fileNameComp, i1);
    strncpy(extension, fileNameComp + i1, strlen(fileNameComp));
    strncpy(decompressionName, originalName, strlen(originalName));
    strcpy(decompressionName + strlen(originalName), "(1)");
    strncpy(decompressionName + strlen(originalName) + 3, extension, strlen(extension));
    loadStr(fileNameNew, decompressionName);
    printf("\nDone\nThe file was saved in %s", decompressionName);
}
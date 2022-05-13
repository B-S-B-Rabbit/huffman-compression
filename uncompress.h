#ifndef UNCOMPRESS_H
#define UNCOMPRESS_H

#define CODE_SIZE 10000
#define BIT8 8
#define SIZE_MAS 80
#include "compress.h"


unsigned char * symmetricGo(node2 *root, char *buf, int level);
int find(node2 *root, const char *codeStr, FILE* file, unsigned int count);
node2 *addToTree(node2 *root, const char str[1000], int index, int level, tab *arrOfBits);
node2 *createTree(node2 *root);
node2 * fillTree(node2 *root, tab *arrOfBits, unsigned int kBits);
int loadStr(char * filename, char * nameForDec);
int uncompress();
#endif

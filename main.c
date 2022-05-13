#include <stdio.h>
#include "compress.h"
#include "uncompress.h"

int main(){
    int command = 0;
    printf("0 - compress\n1 - uncompress\nOther number - nothing\n");
    scanf("%d", &command);
    if (command != 1 && command != 0){
        printf("sorry, we can't do it");
    }
    else if (command == 0)
    {
        compress();
    }
    else
    {
        uncompress();
    }
    return 0;
}


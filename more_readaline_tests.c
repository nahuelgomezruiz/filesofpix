#include <stdio.h>
#include <stdlib.h>
#include "readaline.h"

int main() {
    
    FILE *fp = fopen("test.txt", "r");
    readaline(fp, NULL);
    fclose(fp);

    return 0;
}
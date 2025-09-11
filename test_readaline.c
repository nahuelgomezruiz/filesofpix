#include <stdio.h>
#include <stdlib.h>
#include "readaline.h"

int main() {
    char *line;
    size_t length; /* # of bytes read into line */
    int line_count = 0;

    /* TEST FOR INVALID/NULL ARGUMENTS */ 
    /* printf("Testing NULL inputfd...\n");
    length = readaline(NULL, &line); */

    while ((length = readaline(stdin, &line)) > 0) {
        line_count++;
        printf("Line %d: %zu bytes: ", line_count, length);
        fwrite(line, 1, length, stdout);
        free(line);
    }

    return 0;
}
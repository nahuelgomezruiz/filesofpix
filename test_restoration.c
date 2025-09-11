/*
 * test_restoration.c
 * Authors: Sabeeh Iftikhar (siftik01), Nahuel Gomez (UTLN HERE)
 * filesofpix
 * 
 * Simple test program to test readaline with corrupted PGM files
 */

#include "readaline.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <corrupted_pgm_file>\n", argv[0]);
        return 1;
    }
    
    FILE *input = fopen(argv[1], "rb");
    if (input == NULL) {
        fprintf(stderr, "Error: Cannot open file %s\n", argv[1]);
        return 1;
    }
    
    char *line;
    size_t length;
    int line_count = 0;
    
    printf("Reading corrupted PGM file: %s\n", argv[1]);
    
    while ((length = readaline(input, &line)) > 0) {
        line_count++;
        printf("Line %d: %zu bytes\n", line_count, length);
        
        /* Print the line content */
        if (line != NULL) {
            printf("Content: ");
            for (size_t i = 0; i < length && i < 50; i++) {
                printf("%c", line[i]);
            }
            printf("\n");
        }
        
        free(line);
    }
    
    printf("Total lines read: %d\n", line_count);
    
    fclose(input);
    return 0;
}
/*
 *     test_readaline.c
 *     Authors: Sabeeh Iftikhar (siftik01), Nahuel Gomez (agomez08)
 *     filesofpix
 *     09/15/2025
 *
 *     Test program for the readaline function.
 *     Reads lines from standard input using readaline and displays
 *     each line with its line number and byte count.
 *     Demonstrates proper usage of readaline including memory management.
 */

#include <stdio.h>
#include <stdlib.h>
#include "readaline.h"

/**************** main *****************
 *
 * Test readaline by reading lines from stdin and displaying statistics.
 *
 * Parameters:
 *      None (reads from standard input)
 *
 * Return:
 *      0 on successful completion.
 *
 * Expects:
 *      Standard input contains text lines terminated with newlines.
 *
 * Effects:
 *      Reads all lines from stdin using readaline, prints each line
 *      with its line number and byte count, and frees allocated memory.
 *      Outputs to standard output.
 *
 * Checked Runtime Errors:
 *      May propagate CREs from readaline if input errors occur.
 ************************/
int main() {
        char *line;
        size_t length;
        int line_count = 0;

        while ((length = readaline(stdin, &line)) > 0) {
                line_count++;
                printf("Line %d: %zu bytes: ", line_count, length);
                fwrite(line, 1, length, stdout);
                free(line);
        }

        return 0;
}
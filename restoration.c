/*
 * restoration.c
 * Authors: Sabeeh Iftikhar (siftik01), Nahuel Gomez (UTLN HERE)
 * filesofpix
 * 
 * Main restoration program that processes corrupted PGM files and outputs 
 * clean P5 format
 */

#include "readaline.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <except.h>
#include <assert.h>
#include <ctype.h>

/* Hanson data structures */
#include "table.h"
#include "seq.h"
#include "atom.h"

/* Constants */
#define MAX_LINE_LENGTH 1000
#define MAXVAL 255

/* Structure to hold digit array for a line */
typedef struct digit_array {
    int *digits;
    int length;
} *digit_array_t;

/* Structure to hold PGM header info */
typedef struct pgm_header {
    int width;
    int height;
    int maxval;
} *pgm_header_t;


/*
 * break_line_down: parses a line into character sequence and digit array
 * Example: "a10b6c6d10e11fgh" -> "abcdefgh" and [10, 6, 6, 10, 11]
 */

int main() {
    return 0;
}
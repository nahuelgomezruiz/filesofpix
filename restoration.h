/*
 *     restoration.h
 *     Authors: Sabeeh Iftikhar (siftik01), Nahuel Gomez (agomez08)
 *     filesofpix
 *     09/14/2025
 *
 *     Public interface for the restoration module. Declares the top-level
 *     API to restore a corrupted PGM to P5 and has testable helpers for:
 *       - opening files with CRE on failure,
 *       - parsing corrupted lines into infusion/pixel components,
 *       - assembling PGM headers and writing the raster.
 *
 *     Clients should call restore_image(); other functions are provided
 *     to facilitate unit testing and composition within this project.
 */

#ifndef RESTORATION_H
#define RESTORATION_H

#include "readaline.h"
#include "line_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <except.h>
#include <assert.h>
#include <ctype.h>
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

/* Error Definition */
Except_T Checked_Runtime_Error;

                /* HELPER FUNCTION DECLARATIONS */

/* Digit Array Management */
digit_array_t create_digit_array(int *digits, int length);
void write_digit_arrays(FILE *output, Seq_T digit_arrays);
void write_digit_arrays_from_sequence(FILE *output, Seq_T digit_sequences, 
                                        int row_width);

/* String parsing utilities */
int parse_number(const char *line, size_t *i, size_t line_len);
void extract_characters(const char *line, size_t line_len, char *chars, 
                        int *char_count);
void extract_digits(const char *line, size_t line_len, int *digits, 
                    int *digit_count);

/* PGM header management */
pgm_header_t create_pgm_header(int width, int height);
void free_pgm_header(pgm_header_t header);
void write_pgm_header(FILE *output, pgm_header_t header);

/* Line processing */
void break_line_down(const char *line, int line_len, char **char_sequence, 
                    int *char_sequence_len, digit_array_t *digit_array);

/* FILE I/O */
FILE *open_file(const char *filename, const char *mode);

/* Restoration */
void process_image_file(FILE *input, LineTable *table);
void restore_image(const char *input_filename);

#endif /* RESTORATION_H */
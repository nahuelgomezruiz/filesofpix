/*
 *     line_table.h
 *     Authors: Sabeeh Iftikhar (siftik01), Nahuel Gomez (agomez08)
 *     filesofpix
 *     09/15/2025
 *
 *     Interface for LineTable, a structure mapping strings to lists of
 *     integer arrays. Provides creation, insertion, lookup of reconstructed
 *     digits, and memory cleanup.
 */

#ifndef LINE_TABLE_H
#define LINE_TABLE_H

#include "list.h"
#include "seq.h"

/********** LineTable ********
 * Abstract type representing a line table mapping strings to
 * sequences of integer arrays.
 ************************/
typedef struct LineTable LineTable;

/* Functions */
LineTable *create_line_table();
void add_to_line_table(LineTable *lt, char* s, int s_len, int *intarr, int len);
Seq_T get_reconstructed_digits(LineTable *lt, int *size);
void free_line_table(LineTable *lt);

/* Helper for freeing contents */
void free_seq_contents(const void *key, void **value, void *cl);

#endif /* LINE_TABLE_H */
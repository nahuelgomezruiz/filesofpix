/* 
readaline.c
Authors: Sabeeh Iftikhar (siftik01), Nahuel Gomez (UTLN HERE)
filesofpix

This file implements the readaline as denoted by the spec. ADD MORE HERE
*/

#include "readaline.h"
#include <stdio.h>
#include <stdlib.h>
#include <except.h>
#include <assert.h>

/* initial value for the output buffer */
#define INITIAL_OUTPUT_ARRAY_CAPACITY 1050

/* Defining errors */
Except_T Checked_Runtime_Error = {"Checked Runtime Error"};

/* HELPER FUNCTION DECLARATION */
void check_valid_input(FILE *inputfd, char **datapp);
char *allocate_buffer_mem(size_t capacity);
void check_output_array_length(size_t curr_length, size_t curr_capacity, 
                               char *buffer);
void check_read_errors(FILE *inputfd, char *buffer);

/* FUNCTION CONTRACT */ 
size_t readaline(FILE *inputfd, char **datapp) 
{
    /* Check that no pointer is NULL */ 
    check_valid_input(inputfd, datapp); 
    
    /* open file and check if we're at EOF */
    int ch = fgetc(inputfd);
    if (ch == EOF) {
        *datapp = NULL;
        return 0;
    }

    /* build the output array */
    size_t curr_capacity = INITIAL_OUTPUT_ARRAY_CAPACITY, curr_length = 0;
    char *buffer = allocate_buffer_mem(curr_capacity);
    while (ch != '\n') {
        /* store the current char in the buffer and read the next one */
        check_output_array_length(curr_length, curr_capacity, buffer);
        buffer[curr_length++] = (char) ch;
        ch = fgetc(inputfd); 
    }

    buffer[curr_length++] = '\n';
    check_read_errors(inputfd, buffer);    
    *datapp = buffer;
    return curr_length;
}

/* Function contract */
void check_valid_input(FILE *inputfd, char **datapp) {
    if ((inputfd == NULL) || (datapp == NULL)) { /* Check for valid input */
        RAISE(Checked_Runtime_Error);
    }
    return;
}

char *allocate_buffer_mem(size_t capacity) {
    char *out = malloc(capacity);
    /* check if enough memory was found */
    if (out == NULL) {
        RAISE(Checked_Runtime_Error);
    }
    return out;
}

void check_output_array_length(size_t curr_length, size_t curr_capacity, 
                               char *buffer) {
    /* Check for more space (FULL CREDIT) */
    if (curr_length >= curr_capacity) {
        fprintf(stderr, "readaline: input line too long\n");
        free(buffer);
        exit(4);
        // curr_capacity *= 2; /* FULL CREDIT Reallocate buffer */ 
        // buffer = realloc(buffer, curr_capacity);
        // if (buffer == NULL) { raise error }
    }
    return;
}

void check_read_errors(FILE *inputfd, char *buffer) {
    if (ferror(inputfd)) {
        free(buffer);
        RAISE(Checked_Runtime_Error);
    }
}
/*
 *     readaline.c
 *     Authors: Sabeeh Iftikhar (siftik01), Nahuel Gomez (agomez08)
 *     filesofpix
 *     09/15/2025
 *
 *     This file implements the readaline function as specified.
 *     Reads a single line (ending with '\n') from an input stream
 *     into a dynamically allocated buffer, while ensuring checked runtime
 *     error safety.
 */

#include "readaline.h"
#include <stdio.h>
#include <stdlib.h>
#include <except.h>
#include <assert.h>

/* initial capacity of output buffer */
#define INITIAL_OUTPUT_ARRAY_CAPACITY 1050

/* Defining errors */
Except_T Runtime_Error;

/* Helper function declarations */
void check_null(void *pointer_to_check);
void check_valid_input(FILE *inputfd, char **datapp);
char *allocate_buffer_mem(size_t capacity);
void check_output_array_length(size_t curr_length, size_t *curr_capacity, 
                               char **buffer);
void check_read_errors(FILE *inputfd, char *buffer);

/********** readaline ********
 *
 * Read a single line from inputfd into a newly allocated buffer.
 *
 * Parameters:
 *      FILE *inputfd: input file stream (not NULL)
 *      char **datapp: pointer to char* to hold output buffer (not NULL)
 *
 * Return:
 *      size_t: number of characters read (including final '\n'),
 *              or 0 if EOF encountered before reading any characters.
 *
 * Expects:
 *      inputfd not NULL, datapp not NULL
 * Effects:
 *      Allocates memory for buffer, assigns to *datapp.
 *      Caller responsible for freeing *datapp.
 ************************/
size_t readaline(FILE *inputfd, char **datapp) 
{
        check_valid_input(inputfd, datapp); 
        
        int ch = fgetc(inputfd);
        /* Check for read errors after 1st fgetc */
        check_read_errors(inputfd, NULL);
        if (ch == EOF) {
                *datapp = NULL;
                return 0;
        }

        /* Build the output array */
        size_t curr_capacity = INITIAL_OUTPUT_ARRAY_CAPACITY, curr_length = 0;
        char *buffer = allocate_buffer_mem(curr_capacity);
        while (ch != '\n' && ch != EOF) {
                /* Store the current char in the buffer and read the next one */
                check_output_array_length(curr_length, &curr_capacity, &buffer);
                buffer[curr_length++] = (char) ch;
                ch = fgetc(inputfd);
                check_read_errors(inputfd, buffer);
        }
        if (ch != EOF) {
                buffer[curr_length++] = '\n';
        }   
        *datapp = buffer;
        return curr_length;  
}

/********** check_valid_input ********
 *
 * Validate input parameters for readaline.
 *
 * Parameters:
 *      FILE *inputfd:  input stream pointer to validate
 *      char **datapp:  output pointer location to validate
 *
 * Expects:
 *      Nothing beyond the provided arguments.
 *
 * Checked Runtime Errors:
 *      Raises a CRE if inputfd or datapp is NULL.
 ************************/
void check_valid_input(FILE *inputfd, char **datapp) 
{
        check_null(inputfd);
        check_null(datapp);
        return;
}

/********** allocate_buffer_mem ********
 *
 * Allocate a character buffer of the requested capacity.
 *
 * Parameters:
 *      size_t capacity:  number of bytes to allocate
 *
 * Return:
 *      Pointer to newly allocated buffer of size capacity.
 *
 * Expects:
 *      capacity > 0
 *
 * Effects:
 *      Allocates memory with malloc; caller must free it.
 *
 * Checked Runtime Errors:
 *      Raises a CRE if malloc fails.
 ************************/
char *allocate_buffer_mem(size_t capacity) 
{
        char *out = malloc(capacity);
        /* Check if enough memory was found */
        check_null(out);
        return out;
}

/********** check_output_array_length ********
 *
 * Ensure there is space in the buffer for the next write (and trailing '\n').
 * Doubles capacity and updates the caller’s buffer and capacity if needed.
 *
 * Parameters:
 *      size_t  curr_length:    number of bytes currently stored in buffer
 *      size_t *curr_capacity:  in/out; current capacity updated if grown
 *      char  **buffer:         in/out; pointer updated if reallocated
 *
 * Expects:
 *      curr_capacity and buffer are not NULL and point to valid objects.
 *
 * Effects:
 *      May reallocate the buffer via realloc and update *buffer and
 *      *curr_capacity accordingly.
 *
 * Checked Runtime Errors:
 *      Raises a CRE if realloc fails (original buffer remains owned by caller).
 ************************/
void check_output_array_length(size_t curr_length, size_t *curr_capacity, 
                                char **buffer) 
{
        if (curr_length >= *curr_capacity - 1) {
                *curr_capacity *= 2;
                char *new_buffer = realloc(*buffer, *curr_capacity);
                check_null(new_buffer);
                *buffer = new_buffer;
        }
        return;
}

/********** check_read_errors ********
 *
 * Check for a stream read error on inputfd after reading operations.
 *
 * Parameters:
 *      FILE *inputfd:  input stream to check
 *      char *buffer:   buffer to free if a read error is detected
 *
 * Expects:
 *      inputfd is not NULL; buffer is either NULL or an allocated block.
 *
 * Effects:
 *      Frees buffer if a read error is detected.
 *
 * Checked Runtime Errors:
 *      Raises a CRE if ferror(inputfd) is set.
 ************************/
void check_read_errors(FILE *inputfd, char *buffer) 
{
        if (ferror(inputfd)) {
                free(buffer);
                RAISE(Runtime_Error);
        }
}

/********** check_null ********
 *
 * Check if a pointer is NULL and raise a checked runtime error if so.
 *
 * Parameters:
 *      void *pointer_to_check:  pointer to validate (may be NULL)
 *
 * Effects:
 *      None if pointer is valid (not NULL).
 *
 * Checked Runtime Errors:
 *      Raises a CRE if pointer_to_check is NULL.
 ************************/
void check_null(void *pointer_to_check) 
{
        if (pointer_to_check == NULL) {
                RAISE(Runtime_Error);
        }
}
 /*
 *     line_table.c
 *     Authors: Sabeeh Iftikhar (siftik01), Nahuel Gomez (agomez08)
 *     Date: 09/14/2025
 *     filesofpix
 *
 *     Implements a line table that stores integer arrays associated with
 *     strings. The first duplicate occurrence of a string marks it as the
 *     "target" string. Clients can retrieve the arrays corresponding to
 *     that target for reconstruction.
 */

#include <stdio.h>
#include <stdlib.h>
#include "atom.h"
#include "table.h"
#include "seq.h"
#include "line_table.h"

#define INITIAL_TABLE_SIZE 10000

/* Struct Definition */
struct LineTable {
        const char *original_string;
        int original_row_size;
        Table_T t;
};

/********** line_table_contains ********
 *
 * Check if the table already contains a key.
 *
 * Parameters:
 *      LineTable *lt:   line table (not NULL)
 *      const char *key: string key (not NULL, already atomized)
 *
 * Return: nonzero if present, 0 if not
 *
 * Expects:
 *      lt and key are not NULL
 ***************************************/
static int line_table_contains(LineTable *lt, const char *key) 
{
        return Table_get(lt->t, key) != NULL;
}

/*********** get_or_make_seq ***********
 *
 * Finds the sequence in the table associated to a given key.
 * If there is no any coincidences, it creates a new sequence and returns it.
 *
 * Parameters:
 *      LineTable *lt:   line table (not NULL)
 *      const char *key: string key (not NULL, already atomized)
 *
 * Return: sequence matching key if present, new sequence if key not in table
 *
 * Expects:
 *      lt and key are not NULL.
 ***************************************/
static Seq_T get_or_make_seq(LineTable *lt, const char *key) 
{
        Seq_T seq = Table_get(lt->t, key);
        if (seq == NULL) {
                seq = Seq_new(0);
                Table_put(lt->t, key, seq);
        }
        return seq;
}

/********** add_to_line_table ********
 *
 * Insert a new integer array under string key. If this string has been
 * inserted before, mark it as the "target string."
 *
 * Parameters:
 *      LineTable *lt: line table (not NULL)
 *      char *s:       string key (not NULL, will be atomized)
 *      int s_len:     length of string
 *      int *intarr:   integer array to store (not NULL)
 *      int len:       number of integers in intarr
 *
 * Effects:
 *      Updates lt->original_string and lt->original_row_size if s has
 *      appeared before.
 *      Pushes intarr onto the seq stored in the table under key.
 *
 * Checked Runtime Errors:
 *      CRE if lt or s or intarr is NULL
 ***************************************/
void add_to_line_table(LineTable *lt, char *s, int s_len, int *intarr, int len) 
{ 
        const char *key = Atom_new(s, s_len); 
        /* If string is already present in table (target string) */
        if (line_table_contains(lt, key)) {
                lt->original_string = key;
                lt->original_row_size = len;
        } 

        Seq_T sq = get_or_make_seq(lt, key);
        Seq_addhi(sq, intarr);
}

/********** get_reconstructed_digits ********
 *
 * Retrieve the list of integer arrays corresponding to the target string.
 *
 * Parameters:
 *      LineTable *lt: line table (not NULL, with a target string set)
 *      int *size:     pointer to store array size (not NULL)
 *
 * Return:
 *      List_T of integer arrays for the target string
 *
 * Expects:
 *      lt not NULL
 *      size not NULL
 *      lt->original_string is set (CRE otherwise)
 ***************************************/
Seq_T get_reconstructed_digits(LineTable *lt, int *size) 
{
        /* Set size var equal to size of stored arrays */
        *size = lt->original_row_size;
        return Table_get(lt->t, lt->original_string);
}

/********** create_line_table ********
 *
 * Allocate and initialize a new LineTable.
 *
 * Return:
 *      Pointer to new LineTable, or NULL if allocation fails
 *
 * Notes:
 *      Caller must free with free_line_table
 ***************************************/
LineTable *create_line_table() 
{
        LineTable *out = malloc(sizeof *out);
        if (out == NULL) {
                return NULL;
        }
        
        /* Set struct and populate data members */
        *out = (struct LineTable){0};
        out->t = Table_new(INITIAL_TABLE_SIZE, NULL, NULL);
        if (out->t == NULL) {
                free(out);
                return NULL;
        }
        out->original_string = NULL;
        return out;
}

/********** free_line_table ************
 *
 * Free all memory associated with a LineTable, including all arrays
 * stored in lists.
 *
 * Parameters:
 *      LineTable *lt: line table to free (may be NULL)
 ***************************************/
void free_line_table(LineTable *lt) 
{
        if (lt == NULL) {
                 return;
        }
        /* Free digit arrays in lists */
        Table_map(lt->t, free_seq_contents, NULL);
        Table_free(&lt->t);
        free(lt);
}

/********** free_seq_contents *********
 *
 * Helper for free_line_table: frees all arrays in a seq and the seq itself
 *
 * Parameters:
 *      const void *key: unused
 *      void **value:    pointer to list (List_T *)
 *      void *cl:        unused
 ***************************************/
void free_seq_contents(const void *key, void **value, void *cl) 
{
        (void)key; 
        (void)cl;
        Seq_T seq = *value;
        int *intarr;
        while (Seq_length(seq) > 0) {
                intarr = Seq_remhi(seq);
                free(intarr);
        }
        Seq_free(&seq);
}

/*
 *     restoration.c
 *     Authors: Sabeeh Iftikhar (siftik01), Nahuel Gomez (agomez08)
 *     filesofpix
 *     09/14/2025
 *
 *     Implements restoration:
 *       - reads a corrupted “plain”-style PGM raster line-by-line,
 *       - extracts the infusion sequence (non-digits) and pixel values,
 *       - identifies the target infusion (first duplicate per spec),
 *       - emits a valid P5 (raw) PGM to the output stream.
 *
 *     This file owns the program entry point (main) and orchestrates file I/O,
 *     parsing, grouping lines via the LineTable, and final P5 emission.
 *     Public interfaces used by clients/tests are declared in restoration.h.
 *
 *     Dependencies: restoration.h, readaline.h, line_table.h, seq.h, except.h
 */

#include "restoration.h" 

/**************** main *****************
 *
 * Drive restoration: parse args, invoke restore_image, handle CREs.
 *
 * Parameters:
 *      int argc:       number of command-line arguments
 *      char *argv[]:   vector of command-line arguments
 *
 * Return:
 *      EXIT_SUCCESS on success; exits with nonzero on usage/CRE.
 *
 * Expects:
 *      At most 1 argument: program name and optional input path.
 *
 * Effects:
 *      Opens files inside restore_image; may print diagnostics to stderr.
 *      If no argument provided, reads from standard input.
 *
 * Checked Runtime Errors:
 *      Raises CRE if more than 1 argument is supplied.
 *      Exits with nonzero; restore_image may raise CRE.
 ************************/
int main(int argc, char *argv[]) 
{
        if (argc > 2) {
                RAISE(Checked_Runtime_Error);
        }
        TRY
                if (argc == 2) {
                        restore_image(argv[1]);
                } else { 
                        /* Read from stdin */
                        restore_image(NULL);
                }
        EXCEPT(Checked_Runtime_Error)
                exit(1);
        END_TRY;
        
        return EXIT_SUCCESS;
}

/********** check_if_null ********
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
void check_if_null(void *pointer_to_check) 
{
        if (pointer_to_check == NULL) {
                RAISE(Checked_Runtime_Error);
        }
}

/**************** create_digit_array *****************
 *
 * Wrap a digits buffer and its logical length into a digit_array_t.
 *
 * Parameters:
 *      int *digits:   pointer to heap-allocated integer pixel row
 *      int length:    number of integers in digits
 *
 * Return:
 *      Newly allocated digit_array_t with fields set.
 *
 * Expects:
 *      digits is not NULL; length >= 0.
 *
 * Effects:
 *      Allocates a digit_array_t; caller must free this struct (but not
 *      necessarily the digits buffer if ownership is transferred elsewhere).
 *
 * Checked Runtime Errors:
 *      May raise a CRE if malloc fails.
 ************************/
digit_array_t create_digit_array(int *digits, int length) 
{
        digit_array_t da = malloc(sizeof(struct digit_array));
        check_if_null(da);
        da->digits = digits;
        da->length = length;
        return da;
}

/**************** write_digit_arrays_from_sequence *****************
 *
 * Write a PGM raster from a sequence of int* rows as single-byte pixels.
 *
 * Parameters:
 *      FILE *output:        open stream for writing (binary)
 *      Seq_T digit_sequences: sequence whose elements are (int *) rows
 *      int row_width:       number of pixels in each row
 *
 * Expects:
 *      output not NULL and writable; digit_sequences not NULL;
 *      each row has at least row_width integers in [0,255].
 *
 * Effects:
 *      Writes row_count * row_width bytes to output.
 ************************/
void write_digit_arrays_from_sequence(FILE *output, Seq_T digit_sequences, 
                                        int row_width)
{
        /* Write each digit array as a row of pixel vals */
        for (int i = 0; i < Seq_length(digit_sequences); i++) {
                int *digit_array = Seq_get(digit_sequences, i);
                for (int j = 0; j < row_width; j++) {
                        /* Write each pixel as a single byte */
                        unsigned char pixel = (unsigned char)digit_array[j];
                        fwrite(&pixel, sizeof(unsigned char), 1, output);
                }
        }
}

/**************** parse_number *****************
 *
 * Parse a nonnegative decimal integer starting at index *i.
 *
 * Parameters:
 *      const char *line:  input character buffer
 *      size_t *i:         in/out index into line; advanced past the digits
 *      size_t line_len:   total usable length of line
 *
 * Return:
 *      Parsed integer value (0 if no leading digit at *i).
 *
 * Expects:
 *      line not NULL; i not NULL; *i <= line_len.
 *
 * Effects:
 *      Advances *i to first non-digit or to line_len.
 ************************/
int parse_number(const char *line, size_t *i, size_t line_len) 
{
        int num = 0;
        /* Parse multi-digit numbers ("123" should be one number) */
        while (*i < line_len && isdigit(line[*i])) {
                num = num * 10 + (line[*i] - '0');
                (*i)++;
        }
        return num;
}

/**************** extract_characters *****************
 *
 * Collect all non-digit bytes from line into chars (not including '\n').
 *
 * Parameters:
 *      const char *line:  input line buffer
 *      size_t line_len:   number of bytes to consider from line
 *      char *chars:       output buffer for non-digit sequence
 *      int *char_count:   out count of bytes written (excludes terminator)
 *
 * Expects:
 *      line, chars, char_count not NULL; chars has at least line_len + 1.
 *
 * Effects:
 *      Writes char_count bytes to chars and NUL-terminates.
 ************************/
void extract_characters(const char *line, size_t line_len, char *chars, 
                        int *char_count) 
{
        *char_count = 0;
        /* Exact alphabetic chars from line */
        for (size_t i = 0; i < line_len; i++) {
                if (!isdigit(line[i])) {
                        chars[(*char_count)++] = line[i];
                }
        }
        chars[*char_count] = '\0';
}

/**************** extract_digits *****************
 *
 * Extract all integers from line by concatenating digit runs.
 *
 * Parameters:
 *      const char *line:  input line buffer
 *      size_t line_len:   number of bytes to consider from line
 *      int *digits:       output array to store parsed integers
 *      int *digit_count:  out count of integers written
 *
 * Expects:
 *      line, digits, digit_count not NULL; digits capacity >= line_len.
 *
 * Effects:
 *      Fills digits[0..*digit_count-1] with parsed nonnegative ints.
 *
 ************************/
void extract_digits(const char *line, size_t line_len, int *digits, 
                    int *digit_count) 
{
        *digit_count = 0;
        /* Exact #'s from line */
        for (size_t i = 0; i < line_len; i++) {
                if (isdigit(line[i])) {
                        digits[(*digit_count)++] = parse_number(line, &i,
                                                                line_len);
                }
        }
}

/*--------------PGM header management---------------*/

/**************** create_pgm_header *****************
 *
 * Allocate and initialize a P5 PGM header descriptor.
 *
 * Parameters:
 *      int width:   image width in pixels
 *      int height:  image height in pixels
 *
 * Return:
 *      Newly allocated pgm_header_t with width/height and maxval=255.
 *
 * Expects:
 *      width > 0; height > 0.
 *
 * Effects:
 *      Allocates header; caller must free via free_pgm_header.
 *
 * Checked Runtime Errors:
 *      May raise a CRE if malloc fails.
 ************************/
pgm_header_t create_pgm_header(int width, int height) 
{
        pgm_header_t header = malloc(sizeof(struct pgm_header));
        check_if_null(header);
        header->width = width;
        header->height = height;
        header->maxval = MAXVAL;
        return header;
}

/**************** free_pgm_header *****************
 *
 * Free a PGM header descriptor.
 *
 * Parameters:
 *      pgm_header_t header: header to free (may be NULL)
 *
 * Effects:
 *      Frees header if non-NULL.
 ************************/
void free_pgm_header(pgm_header_t header) 
{
        free(header);
}

/**************** write_pgm_header *****************
 *
 * Write a P5 PGM header to output.
 *
 * Parameters:
 *      FILE *output:        stream opened for binary write
 *      pgm_header_t header: header with width, height, maxval
 *
 * Expects:
 *      output and header not NULL; header fields valid.
 *
 * Effects:
 *      Writes ASCII header lines for P5 format to output.
 *
 ************************/
void write_pgm_header(FILE *output, pgm_header_t header) 
{
        fprintf(output, "P5\n%d %d\n%d\n", header->width, header->height, 
                header->maxval);
}

/*----------------Line processing--------------------*/

/**************** break_line_down *****************
 *
 * Split a corrupted raster line into its infusion sequence and pixel list.
 *
 * Parameters:
 *      const char *line:           NUL-terminated line without trailing '\n'
 *      int line_len:               number of characters in line to process
 *      char **char_sequence:       out; malloc'd infusion (non-digit) sequence
 *      int *char_sequence_len:     out; length of the character sequence
 *      digit_array_t *digit_array: out; struct holding malloc'd int array
 *
 * Expects:
 *      line, char_sequence, char_sequence_len, digit_array not NULL.
 *      line_len >= 0.
 *
 * Effects:
 *      Allocates and sets *char_sequence and *digit_array; caller must free
 *      the returned char_sequence and the digit_array struct. Ownership of
 *      the digits buffer inside digit_array may be transferred to downstream
 *      data structures. Sets *char_sequence_len to the number of non-digit
 *      characters extracted.
 *
 * Checked Runtime Errors:
 *      May raise a CRE if malloc fails (via helpers).
 ************************/
void break_line_down(const char *line, int line_len, char **char_sequence, 
                    int *char_sequence_len, digit_array_t *digit_array) 
{
        /* Allocate max possible sizes */
        char *chars = malloc(line_len + 1);
        check_if_null(chars);

        int *digits = malloc(line_len * sizeof(int));
        if (digits == NULL) {
                /* Clean up first allocation */
                free(chars);
                check_if_null(digits);
        }

        int digit_count;
        extract_characters(line, line_len, chars, char_sequence_len);
        extract_digits(line, line_len, digits, &digit_count);
        
        *char_sequence = chars;
        *digit_array = create_digit_array(digits, digit_count);
}

/* FILE I/O */

/**************** open_file *****************
 *
 * Open a file with the requested mode or raise a CRE on failure.
 *
 * Parameters:
 *      const char *filename: path to file
 *      const char *mode:     fopen mode string (e.g., "rb", "wb")
 *
 * Return:
 *      FILE* stream opened with fopen.
 *
 * Expects:
 *      filename and mode not NULL.
 *
 * Effects:
 *      Opens a file descriptor; caller must fclose it.
 *
 * Checked Runtime Errors:
 *      Raises a CRE if fopen fails.
 ************************/
FILE *open_file(const char *filename, const char *mode) 
{
        FILE *file = fopen(filename, mode);
        check_if_null(file);
        return file;
}

/*--------------------Restoration--------------------*/

/**************** process_image_file *****************
 *
 * Read all corrupted lines from input and populate the line table.
 *
 * Parameters:
 *      FILE *input:     stream positioned at start of corrupted raster
 *      LineTable *table: destination table for infusion groups
 *
 * Expects:
 *      input and table not NULL; each input line ends with '\n'.
 *
 * Effects:
 *      For each line: reads with readaline, derives infusion and digits,
 *      inserts digits under its infusion key in table. Frees transient buffers.
 *
 * Checked Runtime Errors:
 *      Propagates CREs from readaline, allocation wrappers, or file errors.
 ************************/
void process_image_file(FILE *input, LineTable *table)
{
        char *line;
        size_t line_len;
        
        /* Process corrupted image line by line */
        while ((line_len = readaline(input, &line)) > 0) {
                line[line_len - 1] = '\0';
                char *char_sequence;
                digit_array_t digit_array;
                
                int char_sequence_len;
                break_line_down(line, line_len, &char_sequence, 
                                &char_sequence_len, &digit_array);
                add_to_line_table(table, char_sequence, char_sequence_len, 
                                    digit_array->digits, digit_array->length);
                free(char_sequence);
                free(digit_array);
                free(line);
        }
}   

/********** check_if_stdin_or_open_file ********
 *
 * Initialize input stream to either stdin or a named file based on filename.
 *
 * Parameters:
 *      FILE **input:           out; pointer to FILE* to be set
 *      const char *input_filename: path to file (NULL for stdin)
 *
 * Expects:
 *      input not NULL; input_filename may be NULL.
 *
 * Effects:
 *      Sets *input to stdin if input_filename is NULL, otherwise opens
 *      the named file for reading. Caller must close file if not stdin.
 *
 * Checked Runtime Errors:
 *      Raises a CRE if input is NULL or if file opening fails.
 ************************/
void check_if_stdin_or_open_file(FILE **input, const char *input_filename) 
{
        /* Read from standard input */
        if (input_filename == NULL) {
                *input = stdin;
        } else {
                /* Read from named file */
                *input = open_file(input_filename, "rb");
        }
}

/********** close_if_not_stdin ********
 *
 * Close a file stream only if it is not stdin.
 *
 * Parameters:
 *      FILE **input:  in/out; pointer to FILE* to potentially close
 *
 * Expects:
 *      input not NULL; *input may be NULL or any valid FILE*.
 *
 * Effects:
 *      Closes *input via fclose if it is not stdin, otherwise no-op.
 *      Sets *input to indeterminate value after closing.
 *
 * Checked Runtime Errors:
 *      Raises a CRE if input is NULL.
 ************************/
void close_if_not_stdin(FILE **input)
{
        if (*input != stdin) {
                fclose(*input);
        }
}

/**************** restore_image *****************
 *
 * Orchestrate full restoration: read corrupted input, derive original rows,
 * and write a valid P5 PGM to output.
 *
 * Parameters:
 *      const char *input_filename:   path to corrupted PGM (plain-like)
 *
 * Expects:
 *      input_filename if necessary.
 *
 * Effects:
 *      Opens/closes input and output; builds line table; selects target
 *      infusion (first duplicate per spec); writes P5 header and raster.
 *      Frees all owned resources before return.
 *
 * Checked Runtime Errors:
 *      Raises a CRE if files cannot be opened, if read errors occur,
 *      or if memory allocation fails.
 ************************/
void restore_image(const char *input_filename)
{
        FILE *input;
        check_if_stdin_or_open_file(&input, input_filename);
        
        /* Process lines and build hash */
        LineTable *table = create_line_table();
        process_image_file(input, table);
        close_if_not_stdin(&input);

        /* Get reconstructed digits */
        int row_width;
        Seq_T digit_sequences = get_reconstructed_digits(table, &row_width);

        if (digit_sequences != NULL) {
                /* Calculate dimensions */
                int total_rows = Seq_length(digit_sequences);

                /* Create and write PGM header to stdout */
                pgm_header_t header = create_pgm_header(row_width, total_rows);
                write_pgm_header(stdout, header);
                free_pgm_header(header);

                /* Write digit arrays from reconstructed sequence to stdout */
                write_digit_arrays_from_sequence(stdout, digit_sequences, 
                                                row_width);
        }
        /* Cleanup */
        free_line_table(table);
}
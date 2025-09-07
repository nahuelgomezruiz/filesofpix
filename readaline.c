/* 
readaline.c
Authors: Sabeeh Iftikhar (siftik01), Nahuel Gomez (UTLN HERE)
filesofpix

This file implements the readaline as denoted by the spec. ADD MORE HERE
*/


// FUNCTION CONTRACT HERE
size_t readaline(FILE *inputfd, char **datapp) 
{
    if (inputfd == NULL | datapp == NULL) { /* Check for valid input */
        /* Runtime error */
    }
    int ch = fgetc(inputfd); /* Check for EOF @ start */
    if (ch == EOF) {
        *datapp = NULL;
        return 0;
    }
    size_t capacity = 1050; /* Init buffer */
    char *buffer = malloc(capacity);
    if (buffer == NULL) {
        /* Runtime error */
    }
    size_t length = 0;
    while (ch != EOF && ch != '\n') { /* Read chars until newline/EOF */
        if (length >= capacity) { /* Check for more space (FULL CREDIT) */
            // Partial credit: exit with error
            fprintf(stderr, "readaline: input line too long\n");
            free(buffer);
            exit(4);
            // Full credit: reallocate buffer
            // capacity *= 2;
            // buffer = realloc(buffer, capacity);
            // if (buffer == NULL) { raise error }
        }
        buffer[length++] = (char)ch; /* Store char in buffer */
        ch = fgetc(inputfd); /* Read next char */
    }
    if (ch == '\n') { /* Handle newline char */
        buffer[length++] = '\n';
    }
    if (ferror(inputfd)) { /* Check for read errors */
        free(buffer);
        /* Runtime error*/
    }
    *dataapp = buffer;
    return length;
}
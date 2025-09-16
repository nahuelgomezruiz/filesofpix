/*
 * test_restoration.c
 * Authors: Sabeeh Iftikhar (siftik01), Nahuel Gomez (agomez08)
 * filesofpix
 * 
 * Comprehensive test program for restoration functionality with line table 
 * integration
 */

#include "restoration.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <except.h>
#include <assert.h>
#include <ctype.h>

/* Test counters */
static int tests_passed = 0;
static int tests_failed = 0;

/* Testing function declaration */
void free_digit_array(digit_array_t da);
void run_all_tests();
void test_digit_array_creation();
void test_parse_number();
void test_extract_characters();
void test_extract_digits();
void test_break_line_down();
void test_pgm_header();
void test_line_table_functionality();
void test_get_reconstructed_digits();
void test_restore_image_integration();
void test_write_digit_arrays_from_sequence();
void test_edge_cases();
void test_memory_management();
void test_file_io();

/* Testing format */
#define TEST_ASSERT(condition, test_name) \
    do { \
        if (condition) { \
            printf("PASS: %s\n", test_name); \
            tests_passed++; \
        } else { \
            printf("FAIL: %s\n", test_name); \
            tests_failed++; \
        } \
    } while(0)

int main() {
    TRY
        run_all_tests();
    EXCEPT(Checked_Runtime_Error)
        printf("Runtime error during testing!\n");
        return 1;
    END_TRY;
    
    return 0;
}

void free_digit_array(digit_array_t da) 
{
    if (da != NULL) {
        free(da->digits);
        free(da);
    }
}

void test_digit_array_creation() {
    printf("\nTesting Digit Array Creation\n");
    
    // Test basic creation
    int *digits1 = malloc(5 * sizeof(int));
    digits1[0] = 10; digits1[1] = 6; digits1[2] = 6; digits1[3] = 10; 
    digits1[4] = 11;
    digit_array_t da1 = create_digit_array(digits1, 5);
    TEST_ASSERT(da1 != NULL, "Basic digit array creation");
    TEST_ASSERT(da1->length == 5, "Correct length");
    TEST_ASSERT(da1->digits[0] == 10, "First digit correct");
    TEST_ASSERT(da1->digits[4] == 11, "Last digit correct");
    
    // Test empty array
    int *digits2 = malloc(1 * sizeof(int));
    digit_array_t da2 = create_digit_array(digits2, 0);
    TEST_ASSERT(da2 != NULL, "Empty digit array creation");
    TEST_ASSERT(da2->length == 0, "Empty array length");
    
    // Test single element
    int *digits3 = malloc(1 * sizeof(int));
    digits3[0] = 42;
    digit_array_t da3 = create_digit_array(digits3, 1);
    TEST_ASSERT(da3 != NULL, "Single element array creation");
    TEST_ASSERT(da3->length == 1, "Single element length");
    TEST_ASSERT(da3->digits[0] == 42, "Single element value");
    
    // Cleanup
    free_digit_array(da1);
    free_digit_array(da2);
    free_digit_array(da3);
}

void test_parse_number() {
    printf("\nTesting Number Parsing\n");
    
    // Test single digit
    const char *line1 = "5";
    size_t i1 = 0;
    int result1 = parse_number(line1, &i1, 1);
    TEST_ASSERT(result1 == 5, "Single digit parsing");
    TEST_ASSERT(i1 == 1, "Index after single digit");
    
    // Test multi-digit
    const char *line2 = "123";
    size_t i2 = 0;
    int result2 = parse_number(line2, &i2, 3);
    TEST_ASSERT(result2 == 123, "Multi-digit parsing");
    TEST_ASSERT(i2 == 3, "Index after multi-digit");
    
    // Test number followed by non-digit
    const char *line3 = "456abc";
    size_t i3 = 0;
    int result3 = parse_number(line3, &i3, 6);
    TEST_ASSERT(result3 == 456, "Number with trailing non-digit");
    TEST_ASSERT(i3 == 3, "Index after number with trailing non-digit");
    
    // Test zero
    const char *line4 = "0";
    size_t i4 = 0;
    int result4 = parse_number(line4, &i4, 1);
    TEST_ASSERT(result4 == 0, "Zero parsing");
    TEST_ASSERT(i4 == 1, "Index after zero");
    
    // Test very large number
    const char *line5 = "1234567890";
    size_t i5 = 0;
    int result5 = parse_number(line5, &i5, 10);
    TEST_ASSERT(result5 == 1234567890, "Large number parsing");
    TEST_ASSERT(i5 == 10, "Index after large number");
}

void test_extract_characters() {
    printf("\nTesting Character Extraction\n");
    
    // Test basic extraction
    const char *line1 = "a10b6c6d10e11fgh";
    char chars1[100];
    int char_count1;
    extract_characters(line1, strlen(line1), chars1, &char_count1);
    TEST_ASSERT(char_count1 == 8, "Correct character count");
    TEST_ASSERT(strcmp(chars1, "abcdefgh") == 0, "Correct character sequence");
    
    // Test only characters
    const char *line2 = "abcdef";
    char chars2[100];
    int char_count2;
    extract_characters(line2, strlen(line2), chars2, &char_count2);
    TEST_ASSERT(char_count2 == 6, "Only characters count");
    TEST_ASSERT(strcmp(chars2, "abcdef") == 0, "Only characters sequence");
    
    // Test only digits
    const char *line3 = "123456";
    char chars3[100];
    int char_count3;
    extract_characters(line3, strlen(line3), chars3, &char_count3);
    TEST_ASSERT(char_count3 == 0, "Only digits count");
    TEST_ASSERT(strcmp(chars3, "") == 0, "Only digits sequence");
    
    // Test empty string
    const char *line4 = "";
    char chars4[100];
    int char_count4;
    extract_characters(line4, 0, chars4, &char_count4);
    TEST_ASSERT(char_count4 == 0, "Empty string count");
    TEST_ASSERT(strcmp(chars4, "") == 0, "Empty string sequence");
    
    // Test mixed case
    const char *line5 = "A10b6C6d10E11fGh";
    char chars5[100];
    int char_count5;
    extract_characters(line5, strlen(line5), chars5, &char_count5);
    TEST_ASSERT(char_count5 == 8, "Mixed case character count");
    TEST_ASSERT(strcmp(chars5, "AbCdEfGh") == 0, "Mixed case character sequence");
}

void test_extract_digits() {
    printf("\nTesting Digit Extraction\n");
    
    // Test basic extraction
    const char *line1 = "a10b6c6d10e11fgh";
    int digits1[100];
    int digit_count1;
    extract_digits(line1, strlen(line1), digits1, &digit_count1);
    TEST_ASSERT(digit_count1 == 5, "Correct digit count");
    TEST_ASSERT(digits1[0] == 10, "First digit");
    TEST_ASSERT(digits1[1] == 6, "Second digit");
    TEST_ASSERT(digits1[2] == 6, "Third digit");
    TEST_ASSERT(digits1[3] == 10, "Fourth digit");
    TEST_ASSERT(digits1[4] == 11, "Fifth digit");
    
    // Test only digits
    const char *line2 = "123456";
    int digits2[100];
    int digit_count2;
    extract_digits(line2, strlen(line2), digits2, &digit_count2);
    TEST_ASSERT(digit_count2 == 1, "Only digits count");
    TEST_ASSERT(digits2[0] == 123456, "Only digits value");
    
    // Test only characters
    const char *line3 = "abcdef";
    int digits3[100];
    int digit_count3;
    extract_digits(line3, strlen(line3), digits3, &digit_count3);
    TEST_ASSERT(digit_count3 == 0, "Only characters count");
    
    // Test empty string
    const char *line4 = "";
    int digits4[100];
    int digit_count4;
    extract_digits(line4, 0, digits4, &digit_count4);
    TEST_ASSERT(digit_count4 == 0, "Empty string count");
    
    // Test multiple separate numbers
    const char *line5 = "a123b456c789d";
    int digits5[100];
    int digit_count5;
    extract_digits(line5, strlen(line5), digits5, &digit_count5);
    TEST_ASSERT(digit_count5 == 3, "Multiple numbers count");
    TEST_ASSERT(digits5[0] == 123, "First number");
    TEST_ASSERT(digits5[1] == 456, "Second number");
    TEST_ASSERT(digits5[2] == 789, "Third number");
}

void test_break_line_down() {
    printf("\nTesting break_line_down\n");
    
    // Test basic functionality
    const char *line1 = "a10b6c6d10e11fgh";
    char *char_sequence1;
    digit_array_t digit_array1;
    break_line_down(line1, &char_sequence1, &digit_array1);
    
    TEST_ASSERT(char_sequence1 != NULL, "Character sequence allocated");
    TEST_ASSERT(digit_array1 != NULL, "Digit array allocated");
    TEST_ASSERT(strcmp(char_sequence1, "abcdefgh") == 0, "Correct character sequence");
    TEST_ASSERT(digit_array1->length == 5, "Correct digit array length");
    TEST_ASSERT(digit_array1->digits[0] == 10, "First digit");
    TEST_ASSERT(digit_array1->digits[4] == 11, "Last digit");
    
    // Test single character
    const char *line2 = "a1b";
    char *char_sequence2;
    digit_array_t digit_array2;
    break_line_down(line2, &char_sequence2, &digit_array2);
    
    TEST_ASSERT(strcmp(char_sequence2, "ab") == 0, "Single character sequence");
    TEST_ASSERT(digit_array2->length == 1, "Single digit length");
    TEST_ASSERT(digit_array2->digits[0] == 1, "Single digit value");
    
    // Test only characters
    const char *line3 = "abcdef";
    char *char_sequence3;
    digit_array_t digit_array3;
    break_line_down(line3, &char_sequence3, &digit_array3);
    
    TEST_ASSERT(strcmp(char_sequence3, "abcdef") == 0, "Only characters");
    TEST_ASSERT(digit_array3->length == 0, "No digits");
    
    // Test only digits
    const char *line4 = "123456";
    char *char_sequence4;
    digit_array_t digit_array4;
    break_line_down(line4, &char_sequence4, &digit_array4);
    
    TEST_ASSERT(strcmp(char_sequence4, "") == 0, "No characters");
    TEST_ASSERT(digit_array4->length == 1, "One digit group");
    TEST_ASSERT(digit_array4->digits[0] == 123456, "Large number");
    
    // Cleanup
    free(char_sequence1);
    free_digit_array(digit_array1);
    free(char_sequence2);
    free_digit_array(digit_array2);
    free(char_sequence3);
    free_digit_array(digit_array3);
    free(char_sequence4);
    free_digit_array(digit_array4);
}

void test_pgm_header() {
    printf("\nTesting PGM Header\n");
    
    // Test header creation
    pgm_header_t header = create_pgm_header(100, 200);
    TEST_ASSERT(header != NULL, "Header creation");
    TEST_ASSERT(header->width == 100, "Width set correctly");
    TEST_ASSERT(header->height == 200, "Height set correctly");
    TEST_ASSERT(header->maxval == MAXVAL, "Maxval set correctly");
    
    // Test header modification
    header->width = 50;
    header->height = 75;
    header->maxval = 128;
    TEST_ASSERT(header->width == 50, "Width modification");
    TEST_ASSERT(header->height == 75, "Height modification");
    TEST_ASSERT(header->maxval == 128, "Maxval modification");
    
    // Test header writing to file
    FILE *test_file = fopen("test_header.pgm", "w");
    if (test_file != NULL) {
        write_pgm_header(test_file, header);
        fclose(test_file);
        
        // Verify file contents
        FILE *read_file = fopen("test_header.pgm", "r");
        if (read_file != NULL) {
            char buffer[100];
            fgets(buffer, sizeof(buffer), read_file);
            TEST_ASSERT(strcmp(buffer, "P5\n") == 0, "PGM format header");
            fgets(buffer, sizeof(buffer), read_file);
            TEST_ASSERT(strcmp(buffer, "50 75\n") == 0, "Width and height");
            fgets(buffer, sizeof(buffer), read_file);
            TEST_ASSERT(strcmp(buffer, "128\n") == 0, "Max value");
            fclose(read_file);
        }
        remove("test_header.pgm");
    }
    
    free_pgm_header(header);
}

void test_line_table_functionality() {
    printf("\nTesting Line Table Functionality\n");
    
    // Test table creation
    LineTable *table = create_line_table();
    TEST_ASSERT(table != NULL, "Line table creation");
    
    // Test adding first occurrence
    int *digits1 = malloc(3 * sizeof(int));
    digits1[0] = 10; digits1[1] = 20; digits1[2] = 30;
    add_to_line_table(table, "abc", digits1, 3);
    
    // Test adding duplicate (should become target)
    int *digits2 = malloc(3 * sizeof(int));
    digits2[0] = 40; digits2[1] = 50; digits2[2] = 60;
    add_to_line_table(table, "abc", digits2, 3);
    
    // Test adding different string
    int *digits3 = malloc(2 * sizeof(int));
    digits3[0] = 70; digits3[1] = 80;
    add_to_line_table(table, "def", digits3, 2);
    
    free_line_table(table);
}

void test_get_reconstructed_digits() {
    printf("\nTesting get_reconstructed_digits\n");
    
    LineTable *table = create_line_table();
    
    // Add first occurrence
    int *digits1 = malloc(3 * sizeof(int));
    digits1[0] = 10; digits1[1] = 20; digits1[2] = 30;
    add_to_line_table(table, "target", digits1, 3);
    
    // Add duplicate to make it target
    int *digits2 = malloc(3 * sizeof(int));
    digits2[0] = 40; digits2[1] = 50; digits2[2] = 60;
    add_to_line_table(table, "target", digits2, 3);
    
    // Add another occurrence
    int *digits3 = malloc(3 * sizeof(int));
    digits3[0] = 70; digits3[1] = 80; digits3[2] = 90;
    add_to_line_table(table, "target", digits3, 3);
    
    // Test reconstruction
    int row_width;
    Seq_T sequences = get_reconstructed_digits(table, &row_width);
    TEST_ASSERT(sequences != NULL, "Reconstructed sequences not null");
    TEST_ASSERT(row_width == 3, "Correct row width");
    TEST_ASSERT(Seq_length(sequences) == 3, "Correct number of sequences");
    
    // Verify first sequence
    int *first_seq = Seq_get(sequences, 0);
    TEST_ASSERT(first_seq[0] == 10, "First sequence first element");
    TEST_ASSERT(first_seq[1] == 20, "First sequence second element");
    TEST_ASSERT(first_seq[2] == 30, "First sequence third element");
    
    // Verify second sequence
    int *second_seq = Seq_get(sequences, 1);
    TEST_ASSERT(second_seq[0] == 40, "Second sequence first element");
    TEST_ASSERT(second_seq[1] == 50, "Second sequence second element");
    TEST_ASSERT(second_seq[2] == 60, "Second sequence third element");
    
    // Verify third sequence
    int *third_seq = Seq_get(sequences, 2);
    TEST_ASSERT(third_seq[0] == 70, "Third sequence first element");
    TEST_ASSERT(third_seq[1] == 80, "Third sequence second element");
    TEST_ASSERT(third_seq[2] == 90, "Third sequence third element");
    
    free_line_table(table);
}

void test_restore_image_integration() {
    printf("\nTesting restore_image Integration\n");
    
    // Create test input file
    FILE *test_input = fopen("test_input.pgm", "w");
    if (test_input == NULL) {
        printf("SKIP: Could not create test input file\n");
        return;
    }
    
    fprintf(test_input, "a10b20c30d\n");  // First occurrence
    fprintf(test_input, "x40y50z60w\n");  // Different string
    fprintf(test_input, "a10b20c30d\n");  // Duplicate - becomes target
    fprintf(test_input, "a10b20c30d\n");  // Another duplicate
    fclose(test_input);
    
    // Test restoration
    TRY
        restore_image("test_input.pgm", "test_output.pgm");
        
        // Verify output file exists
        FILE *output = fopen("test_output.pgm", "r");
        TEST_ASSERT(output != NULL, "Output file created");
        
        if (output != NULL) {
            // Read and verify header
            char header[100];
            fgets(header, sizeof(header), output);
            TEST_ASSERT(strcmp(header, "P5\n") == 0, "PGM format header");
            
            fgets(header, sizeof(header), output);
            TEST_ASSERT(strcmp(header, "3 3\n") == 0, "Width and height");
            
            fgets(header, sizeof(header), output);
            TEST_ASSERT(strcmp(header, "255\n") == 0, "Max value");
            
            fclose(output);
        }
        
    EXCEPT(Checked_Runtime_Error)
        printf("FAIL: Runtime error during restoration\n");
        tests_failed++;
    END_TRY;
    
    // Cleanup
    remove("test_input.pgm");
    remove("test_output.pgm");
}

void test_write_digit_arrays_from_sequence() {
    printf("\nTesting write_digit_arrays_from_sequence\n");
    
    // Create test sequence
    Seq_T test_seq = Seq_new(0);
    
    // Add test digit arrays
    int *digits1 = malloc(3 * sizeof(int));
    digits1[0] = 10; digits1[1] = 20; digits1[2] = 30;
    Seq_addhi(test_seq, digits1);
    
    int *digits2 = malloc(3 * sizeof(int));
    digits2[0] = 40; digits2[1] = 50; digits2[2] = 60;
    Seq_addhi(test_seq, digits2);
    
    // Write to file
    FILE *output = fopen("test_sequence.pgm", "wb");
    if (output != NULL) {
        write_digit_arrays_from_sequence(output, test_seq, 3);
        fclose(output);
        
        // Verify file size (3 ints * 2 sequences * sizeof(int))
        FILE *input = fopen("test_sequence.pgm", "rb");
        if (input != NULL) {
            fseek(input, 0, SEEK_END);
            long file_size = ftell(input);
            fclose(input);
            TEST_ASSERT(file_size == 3 * 2 * sizeof(int), "Correct file size");
        }
        
        remove("test_sequence.pgm");
    }
    
    // Cleanup sequence
    for (int i = 0; i < Seq_length(test_seq); i++) {
        free(Seq_get(test_seq, i));
    }
    Seq_free(&test_seq);
}

void test_edge_cases() {
    printf("\nTesting Edge Cases\n");
    
    // Test very long number
    const char *line1 = "a1234567890b";
    char *char_sequence1;
    digit_array_t digit_array1;
    break_line_down(line1, &char_sequence1, &digit_array1);
    
    TEST_ASSERT(strcmp(char_sequence1, "ab") == 0, "Long number characters");
    TEST_ASSERT(digit_array1->length == 1, "Long number digit count");
    TEST_ASSERT(digit_array1->digits[0] == 1234567890, "Long number value");
    
    // Test mixed case
    const char *line2 = "A10b6C6d10E11fGh";
    char *char_sequence2;
    digit_array_t digit_array2;
    break_line_down(line2, &char_sequence2, &digit_array2);
    
    TEST_ASSERT(strcmp(char_sequence2, "AbCdEfGh") == 0, "Mixed case characters");
    TEST_ASSERT(digit_array2->length == 5, "Mixed case digit count");
    
    // Test special characters
    const char *line3 = "a10!@#b6$%^c6";
    char *char_sequence3;
    digit_array_t digit_array3;
    break_line_down(line3, &char_sequence3, &digit_array3);
    
    TEST_ASSERT(strcmp(char_sequence3, "abc") == 0, "Special characters ignored");
    TEST_ASSERT(digit_array3->length == 3, "Special characters digit count");
    
    // Cleanup
    free(char_sequence1);
    free_digit_array(digit_array1);
    free(char_sequence2);
    free_digit_array(digit_array2);
    free(char_sequence3);
    free_digit_array(digit_array3);
}

void test_memory_management() {
    printf("\nTesting Memory Management\n");
    
    // Test multiple allocations and frees
    for (int i = 0; i < 100; i++) {
        const char *line = "a10b6c6d10e11fgh";
        char *char_sequence;
        digit_array_t digit_array;
        break_line_down(line, &char_sequence, &digit_array);
        
        TEST_ASSERT(char_sequence != NULL, "Memory allocation");
        TEST_ASSERT(digit_array != NULL, "Digit array allocation");
        
        free(char_sequence);
        free_digit_array(digit_array);
    }
    
    printf("PASS: 100 iterations of allocate/free\n");
    tests_passed++;
    
    // Test line table memory management
    LineTable *table = create_line_table();
    for (int i = 0; i < 50; i++) {
        int *digits = malloc(3 * sizeof(int));
        digits[0] = i; digits[1] = i+1; digits[2] = i+2;
        char key[10];
        sprintf(key, "key%d", i);
        add_to_line_table(table, key, digits, 3);
    }
    
    // Make one key the target
    int *target_digits = malloc(3 * sizeof(int));
    target_digits[0] = 100; target_digits[1] = 101; target_digits[2] = 102;
    add_to_line_table(table, "key0", target_digits, 3);
    
    int width;
    Seq_T sequences = get_reconstructed_digits(table, &width);
    TEST_ASSERT(sequences != NULL, "Large table reconstruction");
    TEST_ASSERT(Seq_length(sequences) == 2, "Correct sequence count");
    
    free_line_table(table);
}

void test_file_io() {
    printf("\nTesting File I/O\n");
    
    // Test open_file with valid file
    FILE *test_file = fopen("test_io.txt", "w");
    if (test_file != NULL) {
        fprintf(test_file, "test content");
        fclose(test_file);
        
        FILE *opened_file = open_file("test_io.txt", "r");
        TEST_ASSERT(opened_file != NULL, "Valid file opening");
        fclose(opened_file);
        
        remove("test_io.txt");
    }
    
    // Test open_file with invalid file (should raise exception)
    TRY
        open_file("nonexistent_file.txt", "r");
        printf("FAIL: Should have raised exception for invalid file\n");
        tests_failed++;
    EXCEPT(Checked_Runtime_Error)
        printf("PASS: Exception raised for invalid file\n");
        tests_passed++;
    END_TRY;
}

void run_all_tests() {
    printf("Starting comprehensive restoration.c functionality tests...\n");
    printf("Testing updated restoration with line table integration...\n");
    
    test_digit_array_creation();
    test_parse_number();
    test_extract_characters();
    test_extract_digits();
    test_break_line_down();
    test_pgm_header();
    test_line_table_functionality();
    test_get_reconstructed_digits();
    test_restore_image_integration();
    test_write_digit_arrays_from_sequence();
    test_file_io();
    test_edge_cases();
    test_memory_management();
    
    printf("\nTest Results\n");
    printf("Tests passed: %d\n", tests_passed);
    printf("Tests failed: %d\n", tests_failed);
    printf("Total tests: %d\n", tests_passed + tests_failed);
    
    if (tests_failed == 0) {
        printf("All tests passed!\n");
    } else {
        printf("Some tests failed!\n");
    }
}
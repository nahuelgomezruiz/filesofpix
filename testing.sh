make clean
make

./test_restoration /comp/40/bin/images/corruption/jumbos-corrupt.pgm

#                     # TESTING USED FOR READALINE FUNCTION
# # Create test files
# echo "Hello world" > test.txt
# echo "This is line 2" >> test.txt
# echo "Line 3" >> test.txt

# # Create expect output file
# cat > expected.txt << EOF
# Line 1: 12 bytes: Hello world
# Line 2: 15 bytes: This is line 2
# Line 3: 7 bytes: Line 3
# EOF

# ./test_readaline < test.txt > actual.txt 2>&1
# diff expected.txt actual.txt
# echo "diffed basic functionality"

# # Test File w/ Null Chars

# # Empty File Test
# echo -n "" > empty.txt
# echo -n "" > expected.txt
# ./test_readaline < empty.txt > actual.txt 2>&1
# diff expected.txt actual.txt
# echo "diffed empty"

# # Single line with newline
# echo "Single line" > single.txt
# echo "Line 1: 12 bytes: Single line" > expected.txt
# ./test_readaline < single.txt > actual.txt 2>&1
# diff expected.txt actual.txt
# echo "diffed single w/ newline"

# # Multiple lines
# echo "Hello world" > multi.txt
# echo "This is line 2" >> multi.txt
# echo "Line 3" >> multi.txt
# echo "Line 1: 12 bytes: Hello world" > expected.txt
# echo "Line 2: 15 bytes: This is line 2" >> expected.txt
# echo "Line 3: 7 bytes: Line 3" >> expected.txt
# ./test_readaline < multi.txt > actual.txt 2>&1
# diff expected.txt actual.txt
# echo "diffed multiple lines"

# # Long line (should error for partial credit version)
# printf 'P%.0s' {1..10000} > long_line.txt
# printf 'A%.0s' {1..10000} > long_line.txt
# echo "readaline: input line too long" > expected.txt
# ./test_readaline < long_line.txt > actual.txt 2>&1
# diff expected.txt actual.txt
# echo "diffed long line"

# # NULL characters
# printf "Hello\0world\0test\n" > null_chars.txt
# printf "Line 1: 17 bytes: Hello\0world\0test\n" > expected.txt
# ./test_readaline < null_chars.txt > actual.txt 2>&1
# diff expected.txt actual.txt
# echo "diffed NULL characters"

# # Special characters
# printf "Hello\x01world\x02test\n" > special_chars.txt
# printf "Line 1: 17 bytes: Hello\x01world\x02test\n" > expected.txt
# ./test_readaline < special_chars.txt > actual.txt 2>&1
# diff expected.txt actual.txt
# echo "diffed special characters"

# # 256 ASCII characters (0-255)
# printf '%c' {0..255} > all_ascii.txt
# printf '\n' >> all_ascii.txt
# printf 'Line 1: 257 bytes: ' > expected.txt
# printf '%c' {0..255} >> expected.txt
# printf '\n' >> expected.txt
# ./test_readaline < all_ascii.txt > actual.txt 2>&1
# diff expected.txt actual.txt
# echo "diffed all 256 ASCII"
make clean
make

# Testing readaline

# Create test files + corresponding output files
echo "Hello world" > test.txt
echo "This is line 2" >> test.txt
echo "Line 3" >> test.txt

cat > expected.txt << EOF
Line 1: 12 bytes: Hello world
Line 2: 15 bytes: This is line 2
Line 3: 7 bytes: Line 3
EOF

# Basic functionality
./test_readaline < test.txt > actual.txt 2>&1
diff expected.txt actual.txt
echo "diffed basic functionality"

# Single line with newline
echo "Single line" > single.txt
echo "Line 1: 12 bytes: Single line" > expected.txt
./test_readaline < single.txt > actual.txt 2>&1
diff expected.txt actual.txt
echo "diffed single w/ newline"

# Multiple lines
echo "Line 1: 12 bytes: Hello world" > expected.txt
echo "Line 2: 15 bytes: This is line 2" >> expected.txt
echo "Line 3: 7 bytes: Line 3" >> expected.txt
./test_readaline < test.txt > actual.txt 2>&1
diff expected.txt actual.txt
echo "diffed multiple lines"

# NULL characters
printf "Hello\0world\0test\n" > null_chars.txt
printf "Line 1: 17 bytes: Hello\0world\0test\n" > expected.txt
./test_readaline < null_chars.txt > actual.txt 2>&1
diff expected.txt actual.txt
echo "diffed NULL characters"

# Special characters
printf "Hello\x01world\x02test\n" > special_chars.txt
printf "Line 1: 17 bytes: Hello\x01world\x02test\n" > expected.txt
./test_readaline < special_chars.txt > actual.txt 2>&1
diff expected.txt actual.txt
echo "diffed special characters"

# # Long line without newline (partial credit error, not needed for full credit)
# printf 'A%.0s' {1..2000} > long_no_newline.txt
# echo "readaline: input line too long" > expected.txt
# ./test_readaline < long_no_newline.txt > actual.txt 2>&1
# diff expected.txt actual.txt
# echo "diffed long line without newline"
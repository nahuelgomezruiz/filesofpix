# Makefile for CS 40 Homework 1
#
# Author: Noah Mendelsohn (adapted from Norman Ramsey's compile script)
#		  Modified for usage by Sabeeh Iftikhar (siftik01) and
# 		  Nahuel Gomez (agomez08)
# filesofpix
# 09/15/2025


# Executables to build using "make all"
EXECUTABLES = restoration

#
#  The following is a compromise. You MUST list all your .h files here.
#  If any .h file changes, all .c files will be recompiled. To do better,
#  we could make an explicit target for each .o, naming only the .h
#  files it really uses.
#
# Add your own .h files to the right side of the assingment below.
INCLUDES = line_table.h restoration.h

# C compiles with gcc
CC = gcc

# Comp 40 directory
COMP40 = /comp/40

# Hanson directory
HANSON = /usr/sup/cii40

# Updating include path to use current directory
IFLAGS = -I. -I$(COMP40)/build/include -I$(HANSON)/include/cii


# Compile and link against course software
CFLAGS =  -g -std=c99 -Wall -Wextra -Werror -Wfatal-errors -pedantic $(IFLAGS)

# Linking flags, used in the linking step
# Set debugging information and update linking path
# to include course binaries and CII implementations
LDFLAGS = -g -L$(COMP40)/build/lib -L$(HANSON)/lib64

# Libraries needed for any of the programs that will be linked
# Both programs need cii40 (Hanson binaries) and *may* need -lm (math)
# Only brightness requires the binary for pnmrdr.
LDLIBS = -lpnmrdr -lcii40 -lm

#    'make all' will build all executables. "all" is default target 
all: $(EXECUTABLES)

#    'make clean' will remove all object and executable files
clean:
	rm -f $(EXECUTABLES) *.o

#    To get any .o, compile the corresponding .c
%.o:%.c $(INCLUDES) 
	$(CC) $(CFLAGS) -c $<

# Individual executables

restoration: restoration.o readaline.o line_table.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

test_readaline: test_readaline.o readaline.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

# Other Shortcuts worth nothing
# $@ takes the name of the build rule and inserts it into the command
# $^ inserts the relocatable object file names into the command

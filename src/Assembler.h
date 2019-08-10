/*  Assembler Project
        Authors: Dor Meiri
                 Omer Miara
        An assembler is a program that takes basic computer instructions and converts them 
        into a pattern of bits that the computer's processor can use 
        to perform its basic operations.
this assembler works on an imaginairy coputer with the following "hardware" properties:
    -Proccessor- CPU
    -registers- 8 entities of 14 bits each that are the storage pieces of the cpu itself
    -RAM (random access memory) storage- memory cell refered as 'word' the memory has 4096 cells
                                        numberd 0-4095, word is a 14 bit unit.
    -numbers represented in the 2's complement method.
    -the length of a single row is up to 80 characters, not include the '\n' sign, INCLUDE whitespaces.
    Project Assumptions:
        -the project is compiled with gcc compiler in anci c90 (ansi c89) standard.
        -compiled with the following flags: -Wal -anci -pedantic
        -running examples included.
        -assembler.c module includes the main function and its the controller of the assembly .
        -assembler.h wraps both step one and step two and is the main header file.
        - Comment is in seperated line, comment EOL is undefined (Throw error according to context)
        - Instructions names begin with '.' and followed by lower case letters
        - The compiler is case sensative
        - Parsing:
            - Step one:
                - declaring macro using another macro is acceptable due to conflict in the booklet, 
                    For example, the following is acceptable:
                        .define x = 1
                        .define y = x
        - Registers:
            - r0,...,r7 valid registers. 
            - r+x/r-x/r0x (where x is integer) not register.
            - r8,r9,r10,... invalid syntax (invalid register, label, macro, etc.)
            - examples: r01/r+1/r-1 is not register, r19 is invalid, r1 is register
        -instruction 
            -structure: <optional label> <.data> <one or more parameters> up to 80 characters
            -doesn't contain the A,R,E field.
            -instruction begin with '.' followed by lowercase characters.
            -".data" and ".string" instructions may have labels defined.
            -".entry" and ".extern" instructions lables are ignored and warning is thrown.
            -the ".data" parameters are all signed integers minus ('-') sign is must to represent negative numbers plus ('+') sign is optional
            (meaning '+17' and '17' represent the same number).
            -instructions doesn't create machine operation word
        - files:
            -Filename appears twice is compiling twice, the second compilition overriding the first
            -if line exceeds the 80 chars, fatal error is thrown due to inability to put upper bound on line length
           -Empty file (no code lines, comments allowed), produce empty object file (0 0 header)
        -operations:
            -operation statement is constructed from at leats one word, and up to five words.
            -the first word structure is always identical
            usage:    |   unused  |   opcode  |src addressing type|dst addressing type | A,R,E | 
            bit count |13 12 11 10| 9  8  7  6|       5   4       |       3   2        | 1   0 |
            -the other 0-4 words also contain the A,R,E field, the other bits encode the word.
         */


#ifndef ASSEMBLER_HEADER
#define ASSEMBLER_HEADER

#include <stdio.h>
#include "helpers/Error.h"
#include "helpers/List.h"
#include "helpers/Queue.h"
#include "entities/Symbol.h"

#define NUM_OF_REGISTERS 8 /*registers r0, r1...r7. the char 'r' followed by integer is a reserved word */
#define DATA_SIZE 100

typedef struct
{
    const char *name;               /* The name of the assembler program as received from argv (withouth extention) */
    FILE *input_fp;
    FILE *output_fp;                /* Only 1 given file pointer at given time, we don't need to hold ext, ent etc. togeter */
    SymbolTable *symbols_table;
    Boolean succeed;
    Word ic;
    Word dc;
} Assembler;


#endif
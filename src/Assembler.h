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
        -registers names are r0, r1,...r7.
        -r<integer> is invalid label name (e.g. r8 is defined as reserved word.)
        -instruction 
            -structure: <optional label> <.data> <one or more parameters> up to 80 characters
            -doesn't contain the A,R,E field.
            -instruction begin with '.' followed by lowercase characters.
            -".data" and ".string" instructions may have labels defined.
            -".entry" and ".extern" instructions lables are ignored and warning is thrown.
            -the ".data" parameters are all signed integers minus ('-') sign is must to represent negative numbers plus ('+') sign is optional
            (meaning '+17' and '17' represent the same number).
            -instructions doesn't create machine operation word
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
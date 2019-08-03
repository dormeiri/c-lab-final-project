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
#include "helpers/error.h"
#include "helpers/list.h"
#include "helpers/queue.h"

#define NUM_OF_REGISTERS 8 /*registers r0, r1...r7. the char 'r' followed by integer is a reserved word */
#define WORD_SIZE 14       /*TODO r193 is reserved as well? */
/* (2^(WORD_SIZE-1) - 1) the msb as 0 represent positivity*/
#define MAX_NUM 8191
/* (2^(WORD_SIZE-1)) the msb as 1 represent negativity*/
#define MIN_NUM -8192

typedef unsigned int word;

/* word converter union creates logical break in a word to all of the ruqired components:
        -raw: the word in its raw form (without logical seperations)
        -operation word: two lsb are the A,R,E field, followed by two bits of the DESTINATION ADDRESSING TYPE, if exist.
            the next two bits are the SOURCE ADDRESSING TYPE, if exist
        -operand word: two lsb are the A,R,E field(the content may be immidate, represented by a symbol, or external), 
            followed by 12 bits to represent the data explicitly, reffer to a label, the external location it's defined in  */
typedef union
{
    word raw;

    struct
    {
        word are : 2;
        word address_dest : 2;
        word address_src : 2;
        word op_code: 4;
    } operation_word;

    struct
    {
        word are : 2;
        word value : 12;
    } operand_word;

    struct
    {
        word are : 2;
        word dest : 3;
        word source : 3;
    } register_word;
} word_converter;


/*A, R, E is the 2 lsb's that indicates weather the value of the word is absolute, relocatable (changes according to the cell
that the word started to load in), or external (there's an external source that the value of the word is defined there) */
typedef enum
{
    A_ARE = 0,/*00 */
    R_ARE = 2,/*01 */
    E_ARE = 1 /*10 */
} are;

typedef enum
{
    EMPTY_KEY,
    COMMENT_KEY,
    MACRO_KEY,
    STRING_KEY,
    DATA_KEY,
    OPERATION_KEY,
    ENTRY_KEY,
    EXTERN_KEY
} statementType;

/* Operations the assmbler recognize, the value of the enums is the exact value of the binary code of the operation
opcode is four bits long (0(0000)-15(1111)), TODO: whats NONE_OP? (-1) */
typedef enum
{
    NONE_OP = -1,
    MOV_OP = 0,
    CMP_OP = 1,
    ADD_OP = 2,
    SUB_OP = 3,
    NOT_OP = 4,
    CLR_OP = 5,
    LEA_OP = 6,
    INC_OP = 7,
    DEC_OP = 8,
    JMP_OP = 9,
    BNE_OP = 10,
    RED_OP = 11,
    PRN_OP = 12,
    JSR_OP = 13,
    RTS_OP = 14,
    STOP_OP = 15
} operationType;

/* The addresses types of the assembler, the values of the enums are the binary value when building operation word */
typedef enum
{
    INSTANT = 0,    /* Yashir */
    DIRECT = 1,     /* Miadi */
    ARRAY = 2,      /* During step1, the value of address in array type is the value of the index,
                    * During step2, the value of the address is += the value of the symbol */
    REGISTER = 3,
    DATA = -1            /* Not in use in operation word so it does not matter what its value */
} addressingType;

/*******************/
/* Symbols defines */
/*******************/

#define SYMBOL_HASHSIZE 100 /* This is the number of available hashes, this is not the limit of the hashset size */

/*symbol property enum classifies the all the exaptble symbols  */
typedef enum 
{
    UNKNOWN_SYM = 0,    /* 0000 in binary */
    MACRO_SYM = 1,      /* 0001 in binary */
    DATA_SYM = 2,       /* 0010 in binary */
    CODE_SYM = 3,       /* 0011 in binary */
    EXTERN_SYM = 4      /* 0100 in binary */
} symbolProperty;

typedef struct
{
    int address_index;
    long file_pos;
    long line_num;
    char *line_str;
} symbol_usage;
typedef struct 
{
    /* TODO: add declaration address index */
    int declaration_index;
    char *symbol_name;
    struct
    {
        unsigned int prop : 4;
        unsigned int ent : 1;
    } property;
    word value;
    List *usages;
} symbol;

typedef struct symbol_list
{
    struct symbol_list *next;
    symbol *value;
} symbol_list;

typedef symbol_list *symbols_table[SYMBOL_HASHSIZE];

/*************/
/* Assembler */
/*************/

typedef struct
{
    const char *name;               /* The name of the assembler program as received from argv (withouth extention) */
    FILE *input_fp;
    FILE *output_fp;                /* Only 1 given file pointer at given time, we don't need to hold ext, ent etc. togeter */
    symbols_table *symbols_table;
    boolean succeed;
    word ic;
    word dc;
} assembler;


#endif
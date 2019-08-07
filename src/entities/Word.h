#ifndef WORD_HEADER
#define WORD_HEADER

#define WORD_SIZE 14       /*TODO r193 is reserved as well? */
/* (2^(WORD_SIZE-1) - 1) the msb as 0 represent positivity*/
#define MAX_NUM 8191
/* (2^(WORD_SIZE-1)) the msb as 1 represent negativity*/
#define MIN_NUM -8192

/*A, R, E is the 2 lsb's that indicates weather the value of the word is absolute, relocatable (changes according to the cell
that the word started to load in), or external (there's an external source that the value of the word is defined there) */
typedef enum
{
    A_ARE = 0,/*00 */
    R_ARE = 2,/*01 */
    E_ARE = 1 /*10 */
} Are;

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
} OperationType;

/* The addresses types of the assembler, the values of the enums are the binary value when building operation word */
typedef enum
{
    INSTANT = 0,    /* Yashir */
    DIRECT = 1,     /* Miadi */
    ARRAY = 2,  
    REGISTER = 3,
    DATA = -1
} AddressingType;

/* Keep it unsigned */
typedef unsigned int Word;

Word word_operation(Are are, AddressingType dest, AddressingType src, OperationType op_type);
Word word_register(Word dest, Word src);
Word word_push_are(Word word, Are are);
Word word_add_to_value(Word word, Word x);
Word word_get_value(Word word);

#endif
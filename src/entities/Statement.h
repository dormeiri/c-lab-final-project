#ifndef STATEMENT_HEADER
#define STATEMENT_HEADER

#include "ImageLine.h"
#include "Symbol.h"

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
} StatementType;

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

typedef struct
{
    char *tag;
    StatementType statement_type;
    OperationType operation_type;
    char *args;
    ImageLine *image_line;
} Statement;

Statement *statement_new();

SymbolProperty statement_get_symbol_property(Statement *statement);

char operation_operands(OperationType op_type);

void statement_free(Statement *s);

#endif
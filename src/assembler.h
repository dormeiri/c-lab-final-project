#ifndef ASSEMBLER_HEADER
#define ASSEMBLER_HEADER

typedef int word; /* TODO: bitfield */

#define NUM_OF_REGISTERS 8

/* 2^14 */
#define MAX_NUM 16384

typedef enum
{
    EMPTY,
    COMMENT,
    MACRO,
    STRING_KEY,
    DATA_KEY,
    OPERATION,
    ENTRY,
    EXTERN
} statementType;

typedef enum
{
    NONE,
    MOV,
    JMP,
    ADD,
    SUB
    /* TODO: Complete */
} operationType;

typedef enum
{
    INSTANT,
    DIRECT,
    INDEX,
    REGISTER,
    DATA
} addressingType;

typedef struct
{
    addressingType type;
    word value;
} address;

typedef struct
{
    char *tag;
    statementType statement_type;
    operationType operation_type;
    char *args;
} statement;

#endif
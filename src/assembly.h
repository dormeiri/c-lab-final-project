#ifndef ASSEMBLY_HEADER
#define ASSEMBLY_HEADER

typedef enum statementType
{
    EMPTY,
    COMMENT,
    MACRO,
    STRING_KEY,
    DATA_KEY,
    OPERATION
} statementType;

typedef enum operationType
{
    NONE,
    MOV,
    JMP,
    ADD,
    SUB
    /* TODO: Complete */
} operationType;

typedef struct statement
{
    char *tag;
    statementType statement_type;
    operationType operation_type;
    char *args;
} statement;

#endif
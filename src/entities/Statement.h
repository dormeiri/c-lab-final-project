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
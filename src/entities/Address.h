#ifndef ADDRESS_HEADER
#define ADDRESS_HEADER

#include "Word.h"

typedef enum
{
    OP_SRC,
    OP_DEST
} OperandDirection;

typedef struct
{
    AddressingType type;
    Word value;
    char *symbol_name; /* Used for step 1 where value not found in symbol table */
    Word index;
} Address;

Address *address_new();

Word get_operand_word(Address *operand, OperandDirection op_dir);

char operation_num_of_operands(OperationType op_type);

void address_free(Address *a);


#endif
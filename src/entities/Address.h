#ifndef ADDRESS_HEADER
#define ADDRESS_HEADER

#include "Word.h"

/* The addresses types of the assembler, the values of the enums are the binary value when building operation word */
typedef enum
{
    INSTANT = 0,    /* Yashir */
    DIRECT = 1,     /* Miadi */
    ARRAY = 2,  
    REGISTER = 3,
    DATA = -1
} AddressingType;

typedef struct
{
    AddressingType type;
    Word value;
    char *symbol_name; /* Used for step 1 where value not found in symbol table */
} Address;

Address *address_new();

Word get_operand_word(Address *operand);

void address_free(Address *a);

#endif
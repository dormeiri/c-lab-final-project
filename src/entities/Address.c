#include <stdlib.h>

#include "Address.h"

Address *address_new()
{
    Address *result;
    if(!(result = (Address *)calloc(1, sizeof(Address))))
    {
        exit(EXIT_FAILURE);
    }

    return result;
}
void address_free(Address *a)
{
    if((a->symbol_name))
    {
        free(a->symbol_name);
        a->symbol_name = NULL;
    }

    a->type = 0;
    a->value = 0;
}
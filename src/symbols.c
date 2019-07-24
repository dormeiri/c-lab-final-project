#include <string.h>
#include "symbols.h"

#define NEXT_NODE(TAB, I, DEST) {\
    size_t J = (I) + 1;\
    for(; J < SYMBOL_HASHSIZE && !(*TAB)[J]; J++);\
    if(J == SYMBOL_HASHSIZE)\
    {\
        DEST = NULL;\
    }\
    else\
    {\
        DEST = (*TAB)[J];\
    }\
    (I) = J; /* To gurantee the inserted value is lvalue */\
}

static unsigned hash(const char *symbol_name);
static symbol_list *install(symbols_table *tab, symbol *sym);
static symbol_list *lookup(symbols_table *tab, const char *symbol_name);
static symbol *create_symbol(const char *name, symbolProperty prop);

void free_symbols_table(symbols_table *tab)
{
    /* TODO: this makes error, try to fix */
    /*symbol *temp;
    symbol *temp2;
    while((temp = next_symbol(tab)) && (temp2 = next_symbol(tab)))
    {
        free(temp);
    }
    free(temp);*/
    free(tab);
}

symbol *next_symbol(symbols_table *tab)
{
    static symbols_table *sym_tab;
    static symbol_list *curr_node = NULL;
    static size_t i;

    if(tab)
    {
        sym_tab = tab;
        i = 0;
    }
    if(curr_node && curr_node->next)
    {
        curr_node = curr_node->next;
    }
    else
    {
        NEXT_NODE(sym_tab, i, curr_node);
    }
    return curr_node ? curr_node->value : NULL;
}

errorCode add_entry_declaration(symbols_table *tab, const char *symbol_name)
{
    /* TODO: TBD does entry can be declared with external? */
    symbol_list *sptr;
    if((sptr = lookup(tab, symbol_name)))
    {
        if(sptr->value->property.ent)
        {
            return ENTRY_ALREADY_EXISTS;
        }
    }
    else
    {
        sptr = install(tab, create_symbol(symbol_name, UNKNOWN_SYM));
    }
    
    sptr->value->property.ent = 1;
    sptr->value->value = 0;

    return OK;
}

errorCode add_symbol_declaration(symbols_table *tab, const char *symbol_name, symbolProperty prop, word value)
{
    word_converter w;
    symbol_list *sptr;

    if((sptr = lookup(tab, symbol_name)))
    {
        if(sptr->value->property.prop != UNKNOWN_SYM)
        {
            return SYMBOL_ALREADY_EXIST;
        }
        sptr->value->property.prop = prop;
    }
    else
    {
        sptr = install(tab, create_symbol(symbol_name, prop));
    }

    switch (prop)
    {
        case EXTERN_SYM:
            w.operand_word.are = E_ARE;
            w.operand_word.value = 0;
            break;

        case MACRO_SYM:
            w.raw = value;
            break;

        default:
            w.operand_word.are = R_ARE;
            w.operand_word.value = value;
            break;
    }
    
    sptr->value->value = w.raw;

    return OK;
}

errorCode add_symbol_usage(symbols_table *tab, const char *symbol_name, long pos, long line_num, const char *line_str, long address_index)
{
    symbol_list *sptr;
    symbol_usage *result;
    if(!(result = (symbol_usage *)malloc(sizeof(symbol_usage))))
    {
        exit(EXIT_FAILURE);
    }

    result->file_pos = pos;
    result->line_num = line_num;
    result->address_index = address_index;
    if(!(result->line_str = (char *)malloc(sizeof(char) * (strlen(line_str) + 1))))
    {
        exit(EXIT_FAILURE);
    }
    result->line_str = strcpy(result->line_str, line_str);

    if(!(sptr = lookup(tab, symbol_name)))
    {
        sptr = install(tab, create_symbol(symbol_name, UNKNOWN_SYM)); /* Once found the real value this will be changed */
    }

    list_add(sptr->value->usages, result);
    return OK;
}

symbol *find_symbol(symbols_table *tab, const char *symbol_name)
{
    symbol_list *sptr;
    
    sptr = lookup(tab, symbol_name);
    return sptr ? sptr->value : NULL;
}

symbol *create_symbol(const char *name, symbolProperty prop)
{
    symbol *result;
    char *sym_name_buf;

    if(!(sym_name_buf = (char *)malloc((strlen(name) + 1) * sizeof(char))))
    {
        exit(EXIT_FAILURE);
    }

    if(!(result = (symbol *)malloc(sizeof(symbol))))
    {
        exit(EXIT_FAILURE);
    }

    result->usages = list_new();
    strncpy(sym_name_buf, name, MAX_TAG_LEN);
    result->symbol_name = sym_name_buf;
    result->property.prop = prop;
    result->property.ent = 0;

    return result;
}

symbol_list *install(symbols_table *tab, symbol *sym)
{
    symbol_list *sptr;
    unsigned hashval;
    
    
    if(!(sptr = lookup(tab, sym->symbol_name)))
    {
        if(!(sptr = (symbol_list *) malloc(sizeof(symbol_list))))
        {
            exit(EXIT_FAILURE);
        }
        hashval = hash(sym->symbol_name);

        sptr->next = (*tab)[hashval];
        sptr->value = sym;
        (*tab)[hashval] = sptr;
    }
    else
    {
        return NULL; /* Already exists */
    }

    return sptr;
}

symbol_list *lookup(symbols_table *tab, const char *symbol_name)
{
    symbol_list *sptr;

    for(sptr = (*tab)[hash(symbol_name)]; sptr; sptr = sptr->next)
    {
        if(!strcmp(sptr->value->symbol_name, symbol_name))
        {
            return sptr;
        }
    }

    return NULL;
}

unsigned hash(const char *symbol_name)
{
    unsigned hashval;
    for(hashval = 0; *symbol_name; symbol_name++)
    {
        hashval = *symbol_name + 31 * hashval;
    }
    return hashval % SYMBOL_HASHSIZE;
}
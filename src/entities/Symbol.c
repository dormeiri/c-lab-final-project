#include <string.h>
#include "Symbol.h"

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
static SymbolList *install(SymbolTable *tab, Symbol *sym);
static SymbolList *lookup(SymbolTable *tab, const char *symbol_name);
static Symbol *create_symbol(const char *name, SymbolProperty prop);
void symbol_usages_list_free(List *l);

void free_symbols_table(SymbolTable *tab)
{
    int i = 0;
    SymbolList *temp;

    for(; i < SYMBOL_HASHSIZE; i++)
    {
        if(((*tab)[i]))
        {
            while((temp = (*tab)[i]))
            {
                (*tab)[i] = (*tab)[i]->next;

                free(temp->value);
                temp->value = NULL;

                free(temp);
                temp = NULL;
            }
            free(((*tab)[i]));
            (*tab)[i] = NULL;
        }
    }
}

Symbol *next_symbol(SymbolTable *tab)
{
    static SymbolTable *sym_tab;
    static SymbolList *curr_node = NULL;
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

ErrorCode add_entry_declaration(SymbolTable *tab, const char *symbol_name)
{
    /* TODO: TBD does entry can be declared with external? */
    SymbolList *sptr;
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
        sptr->value->value = 0;
    }
    
    sptr->value->property.ent = 1;

    return OK;
}

ErrorCode add_symbol_declaration(SymbolTable *tab, const char *symbol_name, SymbolProperty prop, Word value)
{
    SymbolList *sptr;

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
            sptr->value->value = word_push_are(0, E_ARE);
            break;

        case MACRO_SYM:
            sptr->value->value = value;
            break;

        default:
            sptr->value->value = word_push_are(value, R_ARE);
            break;
    }

    return OK;
}

ErrorCode add_symbol_usage(SymbolTable *tab, const char *symbol_name, long pos, long line_num, const char *line_str, int address_index)
{
    SymbolList *sptr;
    SymbolUsage *result;
    if(!(result = (SymbolUsage *)malloc(sizeof(SymbolUsage))))
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

Symbol *find_symbol(SymbolTable *tab, const char *symbol_name)
{
    SymbolList *sptr;
    
    sptr = lookup(tab, symbol_name);
    return sptr ? sptr->value : NULL;
}

Symbol *create_symbol(const char *name, SymbolProperty prop)
{
    Symbol *result;

    if(!(result = (Symbol *)malloc(sizeof(Symbol))))
    {
        exit(EXIT_FAILURE);
    }

    if(!(result->symbol_name = ((char *)malloc((strlen(name) + 1) * sizeof(char)))))
    {
        exit(EXIT_FAILURE);
    }

    result->usages = list_new();
    strcpy(result->symbol_name, name);
    result->property.prop = prop;
    result->property.ent = 0;
    result->value = 0;

    return result;
}

SymbolList *install(SymbolTable *tab, Symbol *sym)
{
    SymbolList *sptr;
    unsigned hashval;
    
    
    if(!(sptr = lookup(tab, sym->symbol_name)))
    {
        if(!(sptr = (SymbolList *) malloc(sizeof(SymbolList))))
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

SymbolList *lookup(SymbolTable *tab, const char *symbol_name)
{
    SymbolList *sptr;

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

void symbol_queue_free(Queue *q)
{
    Symbol *temp;
    while((temp = dequeue(q)))
    {        
        symbol_free(temp);   
    }
    queue_free(q);
}

void symbol_list_free(List *l)
{
    Symbol *temp;
    while((temp = list_get_next(l)))
    {
        symbol_free(temp);   
    }
    list_free(l);
}

void symbol_free(Symbol *s)
{
    if((s->symbol_name))
    {        
        free(s->symbol_name);
        s->symbol_name= NULL;
    }
    if((s->usages))
    {
        symbol_usages_list_free(s->usages);
    }
}

void symbol_usages_list_free(List *l)
{
    SymbolUsage *temp;
    while((temp = list_get_next(l)))
    {
        if(temp->line_str)
        {
            free(temp->line_str);
            temp->line_str = NULL;
        }
    }
    list_free(l);
}
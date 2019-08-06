#ifndef SYMBOLS_HEADER
#define SYMBOLS_HEADER

#include "../helpers/Queue.h"
#include "../helpers/List.h"
#include "../helpers/Error.h"
#include "Word.h"

#define SYMBOL_HASHSIZE 100 /* This is the number of available hashes, this is not the limit of the hashset size */

/*symbol property enum classifies the all the exaptble symbols  */
typedef enum 
{
    UNKNOWN_SYM = 0,    /* 0000 in binary */
    MACRO_SYM = 1,      /* 0001 in binary */
    DATA_SYM = 2,       /* 0010 in binary */
    CODE_SYM = 3,       /* 0011 in binary */
    EXTERN_SYM = 4      /* 0100 in binary */
} SymbolProperty;

typedef struct
{
    int address_index;
    long file_pos;
    long line_num;
    char *line_str;
} SymbolUsage;
typedef struct 
{
    /* TODO: add declaration address index */
    int declaration_index;
    char *symbol_name;
    struct
    {
        unsigned int prop : 4;
        unsigned int ent : 1;
    } property;
    Word value;
    List *usages;
} Symbol;

typedef struct SymbolList
{
    struct SymbolList *next;
    Symbol *value;
} SymbolList;

typedef SymbolList *SymbolTable[SYMBOL_HASHSIZE];

/*free symbols table free up the memory allocated to reserve the symbols found during step one
    Params:
        -tab: pointer to the symbol table */
void free_symbols_table(SymbolTable *tab);

Symbol *next_symbol(SymbolTable *tab);

ErrorCode add_entry_declaration(SymbolTable *tab, const char *symbol_name);

ErrorCode add_symbol_declaration(SymbolTable *tab, const char *symbol_name, SymbolProperty prop, Word value, int declaration_index);

/* TODO: Make it use struct */
ErrorCode add_symbol_usage(SymbolTable *tab, const char *symbol_name, long pos, long line_num, const char *line_str, int address_index);

/* Search the given symbol name in the symbol table and return the symbol */
Symbol *find_symbol(SymbolTable *tab, const char *symbol_name);

void symbol_queue_free(Queue *q);

void symbol_list_free(List *l);

void symbol_free(Symbol *s);

#endif
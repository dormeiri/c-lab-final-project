#ifndef SYMBOLS_HEADER
#define SYMBOLS_HEADER

#include "assembler.h"

void free_symbols_table(symbols_table *tab);

symbol *next_symbol(symbols_table *tab);

errorCode add_entry_declaration(symbols_table *tab, const char *symbol_name);

errorCode add_symbol_declaration(symbols_table *tab, const char *symbol_name, symbolProperty prop, word value);

/* TODO: Make it use struct */
errorCode add_symbol_usage(symbols_table *tab, const char *symbol_name, long pos, long line_num, const char *line_str, long address_index);

/* Search the given symbol name in the symbol table and return the symbol */
symbol *find_symbol(symbols_table *tab, const char *symbol_name);

#endif
#include "step_two.h"
#include "helpers/files.h"
#include "symbols.h"

static void append_entries(assembler *assembler, queue *entries)
{
    FILE *fp;

    get_output(assembler, ENTRY_FILE, &fp);
}

static void append_externals(assembler *assembler, queue *externals)
{
    FILE *fp;

    get_output(assembler, EXTERN_FILE, &fp);
}

static void update_addresses(assembler *assembler, symbol *sym)
{
    symbol_usage *sym_usage;

    while((sym_usage = (symbol_usage *)dequeue(sym->usages)))
    {
        update_symbol_usage(assembler, sym, sym_usage);
    }
}

static void create_error_step_two(assembler *assembler, symbol *sym)
{
    symbol_usage *sym_usage;
    while((sym_usage = (symbol_usage *)dequeue(sym->usages)))
    {
        /* Not declared symbol is the only error can occured during step two */
        create_error(NOT_DECLARED, sym_usage->line_num, assembler->name, sym_usage->line_str);
    }
}

void run_step_two(assembler *assembler)
{
    /* TODO: Create error functions */

    symbol *curr_sym;
    queue *entries;     /* Queue that stores entries and at the end append them to entries file in batch */
    queue *externals;   /* The same as entries queue but for externals */

    entries = initilize_queue(sizeof(symbol));
    externals = initilize_queue(sizeof(symbol));

    while((curr_sym = next_symbol(assembler->symbols_table)))
    {
        if(curr_sym->property == UNKNOWN_SYM)
        {
            create_error_step_two(assembler, curr_sym);
        }
        /* TODO: Create warning on declared but not used */
        else
        {
            switch(curr_sym->property)
            {
                case ENTRY_SYM:
                    enqueue(entries, curr_sym);
                    break;

                case EXTERN_SYM:
                    enqueue(externals, curr_sym);
                    break;

                case MACRO_SYM:
                    break;

                case CODE_SYM:
                case DATA_SYM:
                    update_addresses(assembler, curr_sym);
                    break;

                default: break; /* Should not get here */
            }
        }
    }

    append_entries(assembler, entries);
    append_externals(assembler, externals);

    free_symbols_table(assembler->symbols_table);
}
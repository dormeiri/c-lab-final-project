#include <stdio.h>
#include "step_two.h"
#include "helpers/files.h"
#include "symbols.h"
#include "helpers/list.h"

/* TODO: Move private to the and and create prototypes */

static void append_entries(assembler *assembler, Queue *entries)
{
    FILE *fp;
    symbol *curr_sym;
    symbol_usage *sym_usage;

    /* TODO: Very similar to append_external, exactly the same logic but different ouput file and different queue name, should be generic*/

    if(assembler->succeed)
    {
        files_get_output(assembler, ENTRY_FILE, &fp);
        while((curr_sym = (symbol *)dequeue(entries)))
        {
            while((sym_usage = (symbol_usage *)list_get_next(curr_sym->usages)))
            {
                fprintf(fp, "%s\t%ld\n", curr_sym->symbol_name, sym_usage->address_index);
            }
            symbol_free(curr_sym);
        }
    }
    fclose(fp);
}

static void append_externals(assembler *assembler, Queue *externals)
{
    FILE *fp;
    symbol *curr_sym;
    symbol_usage *sym_usage;

    if(assembler->succeed)
    {
        files_get_output(assembler, EXTERN_FILE, &fp);
        while((curr_sym = (symbol *)dequeue(externals)))
        {
            while((sym_usage = (symbol_usage *)list_get_next(curr_sym->usages)))
            {
                fprintf(fp, "%s\t%ld\n", curr_sym->symbol_name, sym_usage->address_index);
            }
            symbol_free(curr_sym);
        }
    }
    fclose(fp);
}

static void update_addresses(assembler *assembler, symbol *sym)
{
    symbol_usage *sym_usage;

    if(assembler->succeed)
    {
        while((sym_usage = (symbol_usage *)list_get_next(sym->usages)))
        {
            files_update_symbol_usage(assembler, sym, sym_usage);
        }
    }
}

static void error_new_step_two(assembler *assembler, symbol *sym)
{
    symbol_usage *sym_usage;

    assembler->succeed = FALSE;
    while((sym_usage = (symbol_usage *)list_get_next(sym->usages)))
    {
        /* Not declared symbol is the only error can occured during step two */
        error_print(NOT_DECLARED, sym_usage->line_num, assembler->name, sym_usage->line_str, sym->symbol_name);
    }
}

void step_two_run(assembler *assembler)
{
    symbol *curr_sym;
    Queue *entries;     /* Queue that stores entries and at the end append them to entries file in batch */
    Queue *externals;   /* The same as entries queue but for externals */

    puts("Start step two");

    entries = queue_new(sizeof(symbol));
    externals = queue_new(sizeof(symbol));

    curr_sym = next_symbol(assembler->symbols_table);
    while((curr_sym))
    {
        if(curr_sym->property.prop == UNKNOWN_SYM)
        {
            error_new_step_two(assembler, curr_sym);
        }
        /* TODO: Create warning on declared but not used */
        else
        {
            if(curr_sym->property.ent)
            {
                enqueue(entries, curr_sym);
            }
            switch(curr_sym->property.prop)
            {
                case MACRO_SYM:
                    break;
                case EXTERN_SYM:
                    enqueue(externals, curr_sym);
                    update_addresses(assembler, curr_sym);
                    break;
                case CODE_SYM:
                case DATA_SYM:
                    update_addresses(assembler, curr_sym);
                    break;

                default: break; /* Should not get here */
            }
        }
        curr_sym = next_symbol(NULL);
    }

    files_frecopy(assembler);
    if(!IS_EMPTY_QUEUE(entries))
    {
        append_entries(assembler, entries);
        symbol_queue_free(entries);
    }
    if(!IS_EMPTY_QUEUE(externals))
    {
        append_externals(assembler, externals);
        symbol_queue_free(externals);
    }
    /* free_symbols_table(assembler->symbols_table); */
}
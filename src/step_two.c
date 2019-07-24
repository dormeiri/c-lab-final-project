#include "step_two.h"
#include "helpers/files.h"
#include "symbols.h"
#include "helpers/list.h"

/* TODO: Move private to the and and create prototypes */

static void append_entries(assembler *assembler, queue *entries)
{
    FILE *fp;

    if(assembler->succeed)
    {
        if(!IS_EMPTY_QUEUE(entries))
        {
            get_output(assembler, ENTRY_FILE, &fp);
        }
    }
}

static void append_externals(assembler *assembler, queue *externals)
{
    FILE *fp;
    symbol *curr_sym;
    symbol_usage *sym_usage;

    if(assembler->succeed)
    {
        if(!IS_EMPTY_QUEUE(externals))
        {
            get_output(assembler, EXTERN_FILE, &fp);
            curr_sym = (symbol *)dequeue(externals);
            while((curr_sym))
            {
                while((sym_usage = (symbol_usage *)list_get_next(curr_sym->usages)))
                {
                    fprintf(fp, "%s\t%ld\n", curr_sym->symbol_name, sym_usage->address_index);
                }
                puts("1");
                getchar();
                curr_sym = (symbol *)dequeue(externals);
                puts("2");
                getchar();
            }
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
            update_symbol_usage(assembler, sym, sym_usage);
        }
    }
}

static void create_error_step_two(assembler *assembler, symbol *sym)
{
    symbol_usage *sym_usage;

    assembler->succeed = FALSE;
    while((sym_usage = (symbol_usage *)list_get_next(sym->usages)))
    {
        /* Not declared symbol is the only error can occured during step two */
        create_error(NOT_DECLARED, sym_usage->line_num, assembler->name, sym_usage->line_str);
    }
}

void run_step_two(assembler *assembler)
{
    symbol *curr_sym;
    queue *entries;     /* Queue that stores entries and at the end append them to entries file in batch */
    queue *externals;   /* The same as entries queue but for externals */

    entries = initilize_queue(sizeof(symbol));
    externals = initilize_queue(sizeof(symbol));

    curr_sym = next_symbol(assembler->symbols_table);
    while((curr_sym))
    {
        if(curr_sym->property.prop == UNKNOWN_SYM)
        {
            create_error_step_two(assembler, curr_sym);
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
    frecopy_temp_to_obj(assembler);
    append_entries(assembler, entries);
    append_externals(assembler, externals);
    free_queue(entries);
    free_queue(externals);
    /* free_symbols_table(assembler->symbols_table); */
}
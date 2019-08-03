#include <stdio.h>
#include "step_two.h"
#include "helpers/files.h"
#include "symbols.h"
#include "helpers/list.h"

/* TODO: Move private to the and and create prototypes */
   /*TODO suggestion: append_ent_ext: receives assembler and enum (extern and entry) and append w/ cases, will shorten run step 
   two and we can deque seemlessly, and move it to files*/
   /*append entries creates from the 'entries' queue an entries file, if queue isn't empty 
        Params:
                -assembler: an assembler struct that holds the file that is processed by the step.
                -entries: (name,index) queue to convert to FILE 
           */ 

static void append_entries(assembler *assembler, queue *entries)
{
    FILE *fp;
    symbol *curr_sym;
    symbol_usage *sym_usage;

    /* TODO: Very similar to append_external, exactly the same logic but different ouput file and different queue name, should be generic*/

    if(assembler->succeed)
    {
        get_output(assembler, ENTRY_FILE, &fp);
        while((curr_sym = (symbol *)dequeue(entries)))
        {
            while((sym_usage = (symbol_usage *)list_get_next(curr_sym->usages)))
            {
                fprintf(fp, "%s\t%ld\n", curr_sym->symbol_name, sym_usage->address_index);
            }
            free(curr_sym);
        }
    }
    fclose(fp);
}
 /*append externals creates from the 'externals' queue an externals file, if queue isn't empty 
        Params:
                -assembler: an assembler struct that holds the file that is processed by the step.
                -externals: (name,index) queue to convert to FILE 
           */
static void append_externals(assembler *assembler, queue *externals)
{
    FILE *fp;
    symbol *curr_sym;
    symbol_usage *sym_usage;

    if(assembler->succeed)
    {
        get_output(assembler, EXTERN_FILE, &fp);
        while((curr_sym = (symbol *)dequeue(externals)))
        {
            while((sym_usage = (symbol_usage *)list_get_next(curr_sym->usages)))
            {
                fprintf(fp, "%s\t%ld\n", curr_sym->symbol_name, sym_usage->address_index);
            }
            free(curr_sym);
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

void step_two_run(assembler *assembler)
{
    symbol *curr_sym;
    queue *entries;     /* Queue that stores entries and at the end append them to entries file in batch */
    queue *externals;   /* The same as entries queue but for externals */

    puts("Start step two");

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
/*  */
    frecopy_temp_to_obj(assembler);
    if(!IS_EMPTY_QUEUE(entries))
    {
        append_entries(assembler, entries);
        free_queue(entries);
    }
    if(!IS_EMPTY_QUEUE(externals))
    {
        append_externals(assembler, externals);
        free_queue(externals);
    }
    /* free_symbols_table(assembler->symbols_table); */
}
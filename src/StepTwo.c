#include <stdio.h>
#include "StepTwo.h"
#include "helpers/Files.h"
#include "entities/Symbol.h"
#include "helpers/List.h"

/* TODO: Move private to the and and create prototypes */
   /*TODO suggestion: append_ent_ext: receives assembler and enum (extern and entry) and append w/ cases, will shorten run step 
   two and we can deque seemlessly, and move it to files*/
   /*append entries creates from the 'entries' queue an entries file, if queue isn't empty 
        Params:
                -assembler: an assembler struct that holds the file that is processed by the step.
                -entries: (name,index) queue to convert to FILE 
           */ 

static void append_entries(Assembler *assembler, Queue *entries)
{
    FILE *fp;
    Symbol *curr_sym;
    
    files_get_output(assembler, ENTRY_FILE, &fp);
    while((curr_sym = (Symbol *)dequeue(entries)))
    {
        fprintf(fp, "%s\t%04d\n", curr_sym->symbol_name, word_get_value(curr_sym->value));
        symbol_free(curr_sym);
    }
    fclose(fp);
}

 /*append externals creates from the 'externals' queue an externals file, if queue isn't empty 
        Params:
                -assembler: an assembler struct that holds the file that is processed by the step.
                -externals: (name,index) queue to convert to FILE 
           */
static void append_externals(Assembler *assembler, Queue *externals)
{
    FILE *fp = NULL;
    Symbol *curr_sym;
    SymbolUsage *sym_usage;

    while((curr_sym = (Symbol *)dequeue(externals)))
    {
        while((sym_usage = (SymbolUsage *)list_get_next(curr_sym->usages)))
        {
            if(!(fp))
            {
                files_get_output(assembler, EXTERN_FILE, &fp);
            }
            fprintf(fp, "%s\t%04d\n", curr_sym->symbol_name, sym_usage->address_index);
        }
        symbol_free(curr_sym);
    }
    if(fp)
    {
        fclose(fp);
    }
}

static void update_addresses(Assembler *assembler, Symbol *sym)
{
    SymbolUsage *sym_usage;

    if(assembler->succeed)
    {
        while((sym_usage = (SymbolUsage *)list_get_next(sym->usages)))
        {
            files_update_symbol_usage(assembler, sym, sym_usage);
        }
    }
}

static void error_new_step_two(Assembler *assembler, Symbol *sym)
{
    SymbolUsage *sym_usage;

    assembler->succeed = FALSE;
    while((sym_usage = (SymbolUsage *)list_get_next(sym->usages)))
    {
        /* Not declared symbol is the only error can occured during step two */
        error_print(NOT_DECLARED, sym_usage->line_num, assembler->name, sym_usage->line_str, sym->symbol_name);
    }
}

void step_two_run(Assembler *assembler)
{
    Symbol *curr_sym;
    Queue *entries;     /* Queue that stores entries and at the end append them to entries file in batch */
    Queue *externals;   /* The same as entries queue but for externals */

    entries = queue_new(sizeof(Symbol));
    externals = queue_new(sizeof(Symbol));

    curr_sym = next_symbol(assembler->symbols_table);
    while((curr_sym))
    {
        switch (curr_sym->property.prop)
        {
            case DATA_SYM:
                curr_sym->value = word_add_to_value(curr_sym->value, assembler->ic);
            case CODE_SYM:
                curr_sym->value = word_add_to_value(curr_sym->value, DATA_SIZE);
                update_addresses(assembler, curr_sym);
                break;
            case EXTERN_SYM:
                enqueue(externals, curr_sym);
                update_addresses(assembler, curr_sym);
                break;
            default: break;
        }
        if(curr_sym->property.prop == UNKNOWN_SYM)
        {
            error_new_step_two(assembler, curr_sym);
        }
        else if(curr_sym->property.ent)
        {
            enqueue(entries, curr_sym);
        }
        curr_sym = next_symbol(NULL);
    }

    if((assembler->succeed))
    {
        files_frecopy(assembler);
        if(!IS_EMPTY_QUEUE(entries))
        {
            append_entries(assembler, entries);
            symbol_queue_free(entries);
            free(entries);
        }
        if(!IS_EMPTY_QUEUE(externals))
        {
            append_externals(assembler, externals);
            symbol_queue_free(externals);
            free(externals);
        }
    }
}
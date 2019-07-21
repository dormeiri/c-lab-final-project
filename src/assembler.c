#include <stdlib.h>
#include <stdio.h>
#include "symbols.h"
#include "assembler.h"
#include "helpers/parsing.h"
#include "helpers/files.h"

void run_assembler(const char *filename);
void free_assembler(assembler *assembler);
errorCode create_assembler(const char *name, assembler *out); /* TODO: Make all output by pointer functions out parameter name to "out" */

int main(int argc, char **argv)
{
    for(argv++; argv; argv++)
    {
        
    }
    return EXIT_SUCCESS;
}

void run_assembler(const char *filename)
{
    assembler *curr_assembler;
    create_assembler(filename, &curr_assembler);
    run_step_one(curr_assembler);
    if(curr_assembler->succeed)
    {
        /* TODO: run_step_two(curr_assembler); */
    }
    if(curr_assembler->succeed)
    {
        frecopy_temp_to_obj(curr_assembler);
        printf("%s compiled succeessfuly", filename);
    }
    else
    {
        printf("Error occured while compiling %s", filename);
    }
    
    free_assembler(curr_assembler);
}

errorCode create_assembler(const char *name, assembler *out)
{
    out = (assembler *)malloc(sizeof(assembler));
    out->name = name;
    TRY_THROW(set_input_file(out));
    TRY_THROW(set_output_file(out, TEMP_OBJECT_FILE));
    out->symbol_table = initilize_queue(sizeof(symbol));

    return OK;
}

void free_assembler(assembler *assembler)
{
    fclose(assembler->input_fp);
    fclose(assembler->output_fp);
    free_queue(assembler->symbol_table);
}
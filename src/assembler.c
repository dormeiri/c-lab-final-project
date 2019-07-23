#include <stdlib.h>
#include <stdio.h>
#include "symbols.h"
#include "assembler.h"
#include "helpers/parsing.h"
#include "helpers/files.h"
#include "step_one.h"
#include "step_two.h"

boolean run_assembler(const char *filename);
void free_assembler(assembler *assembler);
errorCode create_assembler(const char *name, assembler **out); /* TODO: Make all output by pointer functions out parameter name to "out" */

int main(int argc, char **argv)
{
    if(argc == 1)
    {
        create_error(INVALID_CL, -1, NULL, NULL);
    }
    else
    {
        while(*(++argv) && run_assembler(*argv));
    }
    return EXIT_SUCCESS;
}

boolean run_assembler(const char *filename)
{
    errorCode res;
    assembler *curr_assembler = NULL;

    printf("Started compiling %s\n", filename);

    if((res = create_assembler(filename, &curr_assembler)) != OK)
    {
        create_error(res, -1, filename, NULL);
        return FALSE;
    }
    
    run_step_one(curr_assembler);
    printf("Finished step one\n");
    if(curr_assembler->succeed)
    {
        run_step_two(curr_assembler);
    }
    
    if(curr_assembler->succeed)
    {
        frecopy_temp_to_obj(curr_assembler);
        printf("%s compiled succeessfuly\n", filename);
    }
    else
    {
        printf("Error occured while compiling %s\n", filename);
    }
    
    free_assembler(curr_assembler);
    return curr_assembler->succeed;
}

errorCode create_assembler(const char *name, assembler **out)
{
    if(!(*out = (assembler *)malloc(sizeof(assembler))))
    {
        exit(EXIT_FAILURE);
    }
    (*out)->succeed = TRUE;
    (*out)->name = name;
    if(!((*out)->symbols_table = (symbols_table *)malloc(sizeof(symbols_table))))
    {
        exit(EXIT_FAILURE);
    }

    TRY_THROW(get_input(*out, &(*out)->input_fp));
    TRY_THROW(get_output(*out, TEMP_OBJECT_FILE, &(*out)->output_fp));

    return OK;
}

void free_assembler(assembler *assembler)
{
    fclose(assembler->input_fp);
    fclose(assembler->output_fp);
    free(assembler->symbols_table);
}
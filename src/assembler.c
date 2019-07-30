#include <stdlib.h>
#include <stdio.h>
#include "symbols.h"
#include "assembler.h"
#include "helpers/parsing.h"
#include "helpers/files.h"
#include "step_one.h"
#include "step_two.h"

static boolean run_assembler(const char *filename);
static void free_assembler(assembler *assembler);
static ErrorCode create_assembler(const char *name, assembler **out); /* TODO: Make all output by pointer functions out parameter name to "out" */

int main(int argc, char **argv)
{
    if(argc == 1)
    {
        error_print(INVALID_CL, -1, "", "", "Expected at least one argument");
    }
    else
    {
        while(*(++argv) && run_assembler(*argv));
    }
    return EXIT_SUCCESS;
}

boolean run_assembler(const char *filename)
{
    /* TODO: Remove old .ext, .entm .ob files, create function in files.h for that */

    ErrorCode res;
    assembler *curr_assembler = NULL;

    printf("Started compiling %s\n", filename);

    if((res = create_assembler(filename, &curr_assembler)) != OK)
    {
        error_print(res, -1, filename, NULL, NULL);
        return FALSE;
    }
    
    run_step_one(curr_assembler);
    printf("Finished step one\n");
    if(curr_assembler->succeed)
    {
        step_two_run(curr_assembler);
    }
    
    if(curr_assembler->succeed)
    {
        printf("%s compiled succeessfuly\n", filename);
    }
    else
    {
        printf("Error occured while compiling %s\n", filename);
    }
    
    free_assembler(curr_assembler);
    return curr_assembler->succeed;
}

ErrorCode create_assembler(const char *name, assembler **out)
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

    TRY_THROW(files_get_input(*out, &(*out)->input_fp));
    TRY_THROW(files_get_output(*out, TEMP_OBJECT_FILE, &(*out)->output_fp));

    return OK;
}

void free_assembler(assembler *assembler)
{
    fclose(assembler->input_fp);
    fclose(assembler->output_fp);
    free(assembler->symbols_table);
}
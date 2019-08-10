#include <stdlib.h>
#include <stdio.h>
#include "Assembler.h"
#include "StepOne.h"
#include "StepTwo.h"

static Boolean run_assembler(const char *filename);
static void free_assembler(Assembler *assembler);
static ErrorCode create_assembler(const char *name, Assembler **out); /* TODO: Make all output by pointer functions out parameter name to "out" */
 /*create assembler builds an assembler entity with the followitg pointers: filename pointer, input file pointer, output file pointer
 and a Boolean variable to have a single source flow control.*/

int main(int argc, char **argv)
{
/*freopen("null","r", stdin);*/

    if(argc == 1)
    {
        error_print(INVALID_CL, -1, "", "", "Expected at least one argument");
    }
    else
    {
        while(--argc > 0 && run_assembler(argv[argc]));
    }
    puts("Z");
    fflush(stdout);
    puts("X");
    return 0;
}

/*run_assembler controlls the flow across all assembler steps. the assembler is two steps oriented.
run assembler builds the entity that wraps the file that received as input and mangage it across the different 
steps.
notice: run assembler is unaware of the existence of other files that it will handle. its scope is a file 
to make machinecode file/ errors file by source code (<file>.as (assembly language)--assembler-->object code (<file>.ob))
        Params: 
            -filename: pointer to the cell in argv where the current file to work on is located
        return: true(1) if both steps ran successfully, false(0) otherwise.*/
Boolean run_assembler(const char *filename)
{
    Boolean result;
    ErrorCode err;
    Assembler *curr_assembler = NULL;
   
    if((err = create_assembler(filename, &curr_assembler)) != OK)
    {
        error_print(err, -1, "", "", filename);
        return FALSE;
    }
    
    printf("Started compiling \"%s\"\n", filename);
    puts("Start step one");
    run_step_one(curr_assembler);
    puts("Finish step one");

    fclose(curr_assembler->input_fp);
    curr_assembler->input_fp = NULL;
    
    if(curr_assembler->succeed)
    {
        puts("Start step two");
        step_two_run(curr_assembler);
        puts("Finish step two");

    }
    
    fclose(curr_assembler->output_fp);
    curr_assembler->output_fp = NULL;

    if(curr_assembler->succeed)
    {
        printf("\"%s\" compiled succeessfuly\n", filename);
    }
    else
    {
        printf("Error occured while compiling %s\n", filename);
    }
    
    result = curr_assembler->succeed;
    free_assembler(curr_assembler);
    return result;
}

/*create assembler build the antity that run assembler use to manage the input file across steps and holds the
final output file.
        Params:
            -name: the name of the file as received in argv[] */
ErrorCode create_assembler(const char *name, Assembler **out)
{
    if(!(*out = (Assembler *)malloc(sizeof(Assembler))))
    {
        exit(EXIT_FAILURE);
    }
    (*out)->succeed = TRUE;
    (*out)->name = name;
    if(!((*out)->symbols_table = (SymbolTable *)malloc(sizeof(SymbolTable))))
    {
        exit(EXIT_FAILURE);
    }

    TRY_THROW(files_get_input(*out, &(*out)->input_fp));
    TRY_THROW(files_get_output(*out, TEMP_OBJECT_FILE, &(*out)->output_fp));
    files_remove_old_files(*out);

    (*out)->ic = 0;
    (*out)->dc = 0;

    return OK;
}

/*free assembler free up the memory the assembler used to generate nontemp files.
    Params:
        -assembler: the entity that is passed between the various steps.
         */
void free_assembler(Assembler *assembler)
{
    free_symbols_table(assembler->symbols_table);
    free(assembler->symbols_table);
}
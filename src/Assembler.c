#include <stdlib.h>
#include <stdio.h>
#include "Assembler.h"
#include "StepOne.h"
#include "StepTwo.h"

static boolean run_assembler(const char *filename);
static void free_assembler(Assembler *assembler);
static ErrorCode create_assembler(const char *name, Assembler **out); /* TODO: Make all output by pointer functions out parameter name to "out" */
 /*create assembler builds an assembler entity with the followitg pointers: filename pointer, input file pointer, output file pointer
 and a boolean variable to have a single source flow control.*/

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

/*run_assembler controlls the flow across all assembler steps. the assembler is two steps oriented.
run assembler builds the entity that wraps the file that received as input and mangage it across the different 
steps.
notice: run assembler is unaware of the existence of other files that it will handle. its scope is a file 
to make machinecode file/ errors file by source code (<file>.as (assembly language)--assembler-->object code (<file>.ob))
        Params: 
            -filename: pointer to the cell in argv where the current file to work on is located
        return: true(1) if both steps ran successfully, false(0) otherwise.*/
boolean run_assembler(const char *filename)
{
    /* TODO: Remove old .ext, .entm .ob files, create function in files.h for that */

    ErrorCode res;
    Assembler *curr_assembler = NULL;

    printf("Started compiling \"%s\"\n", filename);
   
    if((res = create_assembler(filename, &curr_assembler)) != OK)
    {
        error_print(res, -1, filename, NULL, NULL);
        return FALSE;
    }
    
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

        fclose(curr_assembler->output_fp);
        curr_assembler->output_fp = NULL;
    }
    
    if(curr_assembler->succeed)
    {
        printf("\"%s\" compiled succeessfuly\n", filename);
    }
    else
    {
        printf("Error occured while compiling %s\n", filename);
    }
    
    free_assembler(curr_assembler);
    return curr_assembler->succeed;
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
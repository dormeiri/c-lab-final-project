#include <stdlib.h>
#include <stdio.h>
#include "symbols.h"
#include "assembler.h"
#include "helpers/parsing.h"
#include "helpers/files.h"
#include "step_one.h"
#include "step_two.h"

boolean run_assembler(const char *filename);
errorCode create_assembler(const char *name, assembler **out); /* TODO: Make all output by pointer functions out parameter name to "out" */
void free_assembler(assembler *assembler);
 /*create assembler builds an assembler entity with the followitg pointers: filename pointer, input file pointer, output file pointer
 and a boolean variable to have a single source flow control.*/


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

/*create assembler build the antity that run assembler use to manage the input file across steps and holds the
final output file.
        Params:
            -name: the name of the file as received in argv[] */
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

/*free assembler free up the memory the assembler used to generate nontemp files.
    Params:
        -assembler: the entity that is passed between the various steps.
         */
void free_assembler(assembler *assembler)
{
    fclose(assembler->input_fp);
    fclose(assembler->output_fp);
    free(assembler->symbols_table);
}
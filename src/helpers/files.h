/* 
*   Documenting:        DONE
*   Naming:             DONE
*   Code complexity:    DONE
*   Unused check:       DONE
*   Encapsulation:      DONE
*   Order:              DONE
*/

#ifndef FILES_HEADER
#define FILES_HEADER

#include <stdio.h>
#include "../assembler.h"


/* Entry file extention */
#define ENTRY_EXT ".ent"

/* Extern file extention */
#define EXTERN_EXT ".ext"

/* Object file extention */
#define OBJECT_EXT ".ob"

/* Assembly file extention */
#define INPUT_EXT ".as"

/* Output file streams types */
typedef enum {
    ENTRY_FILE,
    EXTERN_FILE,
    OBJECT_FILE,
    TEMP_OBJECT_FILE
} OutputFileType;


/*
*   Description:    Get input file stream by the name of the assembler
*   Params:
*       - as:       Assembler struct which contains name
*       - out:      File pointer reference which will point to the new file stream
*   Return:         If error occured, the error code, otherwise, OK
*/
ErrorCode files_get_input(assembler *as, FILE **out);

/*
*   Description:    Get output file stream by the name of the assembler and output file type
*   Params:
*       - as:       Assembler struct which contains name
*       - type:     Output file pointer
*       - out:      File pointer reference which will point to the new file stream
*   Return:         If error occured, the error code, otherwise, OK
*/
ErrorCode files_get_output(assembler *as, OutputFileType type, FILE **out);

/*
*   Description:    Read next line of the input of the assembler
*   Params:
*       - as:       Assembler struct which contains input file
*       - out:      Store the line read
*   Return:         If error occured, the error code, otherwise, if EOF occured, return EOF_OCCURED, otherwise OK
*/
ErrorCode files_read_line(assembler *as, char **out);

/*
*   Description:            Write assembly line output file which contains address_index and value
*   Params:
*       - as:               Assembler struct which contains output file
*       - address_index:    The index of the address which the line represents
*       - value:            The value of the address which the line represents
*/
void files_write_address(assembler *as, long address_index, word value);

/*
*   Description:        Update symbol value of symbol which was unknown until now
*   Params:
*       - as:           Assembler struct which contains output file
*       - symbol:       The symbol which is now known
*       - sym_usage:    The symbol usage struct of the symbol
*/
void files_update_symbol_usage(assembler *as, symbol *symbol, symbol_usage *sym_usage);

/*
*   Description:    Free assembler's temp file and copy its data into .as file
*   Params:
*       - as:       Assebler struct which contains temp output file
*/
void files_frecopy(assembler *as);

#endif
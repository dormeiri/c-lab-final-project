#ifndef FILES_HEADER
#define FILES_HEADER

#include <stdio.h>
#include "../errors.h"
#include "../assembler.h"
#include "../step_one.h"

#define ENTRY_EXT ".ent"
#define EXTERN_EXT ".ext"
#define OBJECT_EXT ".ob"
#define INPUT_EXT ".as"

typedef enum {
    ENTRY_FILE,
    EXTERN_FILE,
    OBJECT_FILE,
    TEMP_OBJECT_FILE
} outputFileType;


void frecopy_temp_to_obj(assembler *assembler);

/* Set the input file pointer in the assembler by the name of the assembler */
errorCode get_input(assembler *assember, FILE **out);

/* Set the output file pointer in the assembler by the name of the assembler and the type in the parameter */
errorCode get_output(assembler *assembler, outputFileType type, FILE **out);

/* Read the next line from the input file pointer to the line_ref */
errorCode read_line(assembler *assembler, char **line_ref);

void write_address(assembler *assembler, long address_index, word value);

void update_symbol_usage(assembler *assembler, symbol *symbol, symbol_usage *sym_usage);

#endif
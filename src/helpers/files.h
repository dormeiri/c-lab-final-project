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
errorCode set_input_file(assembler *assember);

/* Set the output file pointer in the assembler by the name of the assembler and the type in the parameter */
errorCode set_output_file(assembler *assember, outputFileType type);

/* Read the next line from the input file pointer to the line_ref */
errorCode read_line(assembler *assembler, char **line_ref);

/* TODO: Should be in step_one? */
/* Translate the current image_line of the current statement in step_one and append the addresses to the file */
errorCode append_line(step_one *step_one);

#endif
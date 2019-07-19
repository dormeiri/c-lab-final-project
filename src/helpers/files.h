#ifndef FILES_HEADER
#define FILES_HEADER

#include <stdio.h>
#include "../errors.h"
#include "../assembler.h"

#define ENTRY_EXT ".ent"
#define EXTERN_EXT ".ext"
#define OBJECT_EXT ".ob"
#define INPUT_EXT ".as"

typedef enum {
    ENTRY,
    EXTERN,
    OBJECT
} outputFileType;

/* Set the input file pointer in the assembler by the name of the assembler */
errorCode set_input_file(assembler *assember);

/* Set the output file pointer in the assembler by the name of the assembler and the type in the parameter */
errorCode set_output_file(assembler *assember, outputFileType type);

/* Read the next line from the input file pointer to the line_ref */
errorCode read_line(assembler *assembler, char **line_ref);

/* Not yet implemented, it supposed to translate the addresses stored in the image_line and put them in the file */
errorCode append_line(assembler *assembler, image_line *image);

#endif
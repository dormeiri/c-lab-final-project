#include "files.h"
#include "converts.h"
#include "../commons.h"
#include <string.h>
#include <stdlib.h>

/* Copy source file to dest file, used to copy temp file stream to object file */
static void copy_file(FILE *source, FILE *dest);

static char *get_filename(const char *name, const char *extention);
/*
Description:    Open the input file to read and gets pointer to that file
Parameters: 
    - filename: The file name
    - fp_ref:   The file pointer reference
Return: If any error occured, error code, otherwise OK
*/
static errorCode get_input_file(char *filename, FILE **fp_ref);

/*
Description:    Open the output file to write and gets pointer to that file
Parameters: 
    - filename: The file name
    - fp_ref:   The file pointer reference
Return: If any error occured, error code, otherwise OK
 */
static errorCode get_output_file(char *filename, FILE **fp_ref);

/*
Description:    Extention for fgets that does validations 
Parameters: 
    - fp:       FILE pointer
    - line_ref: Reference to line string
Return: If any error occured, error code, otherwise OK
 */
static errorCode fgets_wrapper(FILE *fp, char **line_ref);

/* 
Description:        Extention to fopen to do validation
Parameters:
    - file_path:    The path of the file to open
    - mode:         fopen mode
    - fp_ref:       The reference to the FILE pointer
Return:             If error occured, error code, otherwise, OK
 */
static errorCode fopen_wrapper(char *file_path, char *mode, FILE ** fp_ref);

/* 
Description:    Generic open file function
Parameters:
    - filename: The relative path of the file
    - mode:     fopen mode
    - fp_ref:   The reference to the FILE pointer
Return:         If error occured, error code, otherwise, OK
 */
static errorCode get_file(char *filename, char *mode, FILE **fp_ref);

/****************/
/*    Public    */
/****************/

void frecopy_temp_to_obj(assembler *assembler)
{
    FILE *temp_fp = assembler->output_fp;
    set_output_file(assembler, OBJECT_FILE);

    copy_file(temp_fp, assembler->output_fp);
    fclose(temp_fp);
}

errorCode append_line(step_one *step_one)
{
    #define APPEND_ADDRESS(VALUE) fprintf(step_one->assembler->output_fp, "%d\t%s\n", step_one->address_index++, convert_to_base4(VALUE))

    statement *statement = step_one->curr_statement;
    address *curr_address;
    if(step_one->curr_statement->operation_type != NONE)
    {
        APPEND_ADDRESS(convert_operation_first_line(statement->operation_type, statement->image_line));
    }

    while(curr_address = dequeue(statement->image_line->addresses))
    {
        APPEND_ADDRESS(convert_to_base4(curr_address->value));
    }
}

errorCode set_input_file(assembler *assembler)
{
    TRY_THROW(get_input_file(get_filename(assembler->name, INPUT_EXT), &assembler->input_fp));
}

errorCode set_output_file(assembler *assembler, outputFileType type)
{
    char *filename;
    switch (type)
    {
        case ENTRY_FILE:
            filename = get_filename(assembler->name, ENTRY_EXT);
            break;

        case EXTERN_FILE:
            filename = get_filename(assembler->name, EXTERN_EXT);
            break;

        case OBJECT_FILE:
            filename = get_filename(assembler->name, OBJECT_EXT);
            break;

        case TEMP_OBJECT_FILE:
            assembler->output_fp = tmpfile();
            if(assembler->output_fp == NULL)
            {
                exit(EXIT_FAILURE); /* it is not error for user, shuold be treated like malloc fail */
            }
            return OK;
        
        default:
            filename = NULL;
            break;
    }

    TRY_THROW(get_output_file(filename, &assembler->output_fp));
}
errorCode read_line(assembler *assembler, char **line_ref)
{
    fgets_wrapper(assembler->input_fp, line_ref);
}

errorCode append_line(assembler *assembler, image_line *line)
{
}

/******************/
/*    Privates    */
/******************/

void copy_file(FILE *source, FILE *dest)
{
    char c;

    rewind(source);
    rewind(dest);
    while((c = fgetc(source)) != EOF)
    {
        putc(c, dest);
    }
}

/* Do get_file with 'r' mode */
errorCode get_input_file(char *filename, FILE **fp_ref)
{
    FILE *fp;

    TRY_THROW(get_file(filename, "r", &fp));

    *fp_ref = fp;
    return OK;
}

/* Do get_file with 'w' mode */
errorCode get_output_file(char *filename, FILE **fp_ref)
{
    FILE *fp;

    TRY_THROW(get_file(filename, "w", &fp));

    *fp_ref = fp;
    return OK;
}

errorCode fgets_wrapper(FILE *fp, char **line_ref)
{
    char *buffer = (char *)malloc(MAX_STRING_LEN * sizeof(char));
    size_t line_len;

    if(fgets(buffer, LINE_BUFFER_LEN, fp) == NULL)
    {
        return EOF_OCCURED;
    }

    line_len = strlen(buffer);

    if((buffer)[line_len - 1] == '\n')
    {
        (buffer)[line_len - 1] = '\0';
    }
    else if(line_len == LINE_BUFFER_LEN - 1 && feof(fp) == FALSE)
    {
        /* TODO: Decide wheter we want to loop fgets until not buffer exceed */
        return BUF_LEN_EXCEEDED;
    }
    

    *line_ref = buffer;

    return OK;
}

/* Do get_relative_file_path and fopen_wrapper, save the result in fp_ref, return error on fail */
errorCode get_file(char *filename, char *mode, FILE **fp_ref)
{
    FILE *fp;

    TRY_THROW(fopen_wrapper(filename, mode, &fp));
    
    *fp_ref = fp;
    return OK;
}

/* Validate that fopen didn't returned NULL */
errorCode fopen_wrapper(char *file_path, char *mode, FILE ** fp_ref)
{
    *fp_ref = fopen(file_path, mode);

    if(*fp_ref == NULL)
    {
        return FILE_ERROR;
    }

    return OK;
}

char *get_filename(const char *name, const char *extention)
{
    char *filename;
    filename = (char *)malloc((strlen(name) + strlen(extention)) * sizeof(char));
    if(filename == NULL)
    {
        exit(EXIT_FAILURE);
    }

    strcpy(filename, name);
    strcat(filename, extention);
}
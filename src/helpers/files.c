#include "files.h"
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

/* Converts word to special base 4 */
static char *convert_to_base4(word value);

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


errorCode set_input_file(assembler *assembler)
{
    TRY_THROW(get_input_file(get_filename(assembler->name, INPUT_EXT), &assembler->input_fp));
    return OK;
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

    return get_output_file(filename, &assembler->output_fp);
}
errorCode read_line(assembler *assembler, char **line_ref)
{
    return fgets_wrapper(assembler->input_fp, line_ref);
}

void write_address(assembler *assembler, long address_index, word value)
{
    printf("%ld\t%d\n", address_index, value); /* TODO: REMOVE */
    fprintf(assembler->output_fp, "%ld\t%s\n", address_index, convert_to_base4(value));
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
    TRY_THROW(get_file(filename, "r", fp_ref));
    return OK;
}

/* Do get_file with 'w' mode */
errorCode get_output_file(char *filename, FILE **fp_ref)
{
    TRY_THROW(get_file(filename, "w", fp_ref));
    return OK;
}

errorCode fgets_wrapper(FILE *fp, char **line_ref)
{
    static char buffer[MAX_STRING_LEN];
    size_t line_len;
    int i = 0;

    /* Delete the last string completly to prevent parsing issues */
    /* TODO: Maybe try to do without this and see what happens */
    for(; i < MAX_STRING_LEN; i++)
    {
        buffer[i] = '\0';
    }

    if(!fgets(buffer, LINE_BUFFER_LEN, fp))
    {
        return EOF_OCCURED;
    }

    line_len = strlen(buffer);
    if(buffer[line_len - 1] == '\n')
    {
        buffer[line_len - 1] = '\0';
    }
    else if(line_len == LINE_BUFFER_LEN - 1 && feof(fp) == FALSE)
    {
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
    filename = (char *)malloc((strlen(name) + strlen(extention + 1)) * sizeof(char));
    if(!filename)
    {
        exit(EXIT_FAILURE);
    }

    strcpy(filename, name);
    strcat(filename, extention);

    return filename;
}

char *convert_to_base4(word value)
{
    char *result;
    word mask;
    int i;

    /* TODO: Improve, could be done without reversing */

    result = (char *)malloc(sizeof(word) * sizeof(char));
    for(mask = 3, i = 0; (mask << ((sizeof(word) * 8) - WORD_SIZE)) != 0; mask <<= 2, i++)
    {
        switch ((value & mask) >> (i * 2))
        {
            case 0:
                *(result + i) = '*';
                break;
            case 1:
                *(result + i) = '#';
                break;
            case 2:
                *(result + i) = '%';
                break;
            case 3:
                *(result + i) = '!';
                break;
        }
    }
    *(result + i) = '\0';
    {
        char *begin;
        char *end;
        char temp;

        begin = result;
        end = result + i - 1;

        for(; begin <= end; begin++, end--)
        {
            temp = *begin;
            *begin = *end;
            *end = temp;
        }
    }
    return result;
}
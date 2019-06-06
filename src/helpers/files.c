#include "files.h"
#include "../commons.h"
#include <string.h>
#include <stdlib.h>

/* 
Description:        Extention to fopen to do validation
Parameters:
    - file_path:    The path of the file to open
    - mode:         fopen mode
    - fp_ref:       The reference to the FILE pointer
Return:             If error occured, error code, otherwise, OK
 */
errorCode fopen_wrapper(char *file_path, char *mode, FILE ** fp_ref);

/* 
Description:    Generic open file function
Parameters:
    - filename: The relative path of the file
    - mode:     fopen mode
    - fp_ref:   The reference to the FILE pointer
Return:         If error occured, error code, otherwise, OK
 */
errorCode get_file(char *filename, char *mode, FILE **fp_ref);

/* Do get_file with 'r' mode */
errorCode get_input_file(char *filename, FILE **fp_ref)
{
    errorCode res;
    FILE *fp;

    TRY_THROW(res, get_file(filename, "r", &fp));

    *fp_ref = fp;
    return OK;
}

/* Do get_file with 'w' mode */
errorCode get_output_file(char *filename, FILE **fp_ref)
{
    errorCode res;
    FILE *fp;

    TRY_THROW(res, get_file(filename, "w", &fp));

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
    errorCode res;
    FILE *fp;

    TRY_THROW(res, fopen_wrapper(filename, mode, &fp));
    
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
#ifndef FILES_HEADER
#define FILES_HEADER

#include <stdio.h>
#include "../errors.h"

/*
Description:    Open the input file to read and gets pointer to that file
Parameters: 
    - filename: The file name
    - fp_ref:   The file pointer reference
Return: If any error occured, error code, otherwise OK
 */
errorCode get_input_file(char *filename, FILE **fp_ref);

/*
Description:    Open the output file to write and gets pointer to that file
Parameters: 
    - filename: The file name
    - fp_ref:   The file pointer reference
Return: If any error occured, error code, otherwise OK
 */
errorCode get_output_file(char *filename, FILE **fp_ref);

/*
Description:    Extention for fgets that does validations 
Parameters: 
    - fp:       FILE pointer
    - line_ref: Reference to line string
Return: If any error occured, error code, otherwise OK
 */
errorCode fgets_wrapper(FILE *fp, char **line_ref);

#endif
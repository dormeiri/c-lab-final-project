#ifndef COMMONS_HEADER
#define COMMONS_HEADER

/* Boolean value */
typedef enum boolean
{
    /* False boolean value */
    FALSE = 0,

    /* True boolean value */
    TRUE = 1
} boolean;

/* DELIM_CHAR as a string */
#define DELIM ","

/* A character that represent delimiter between arguments */
#define DELIM_CHAR DELIM[0]

/* A string of white spaces */
#define WHITE_SPACE_STR " \t"

#define WHITE_SPACE_1 WHITE_SPACE_STR[0]
#define WHITE_SPACE_2 WHITE_SPACE_STR[1]

#define MAX_TAG_LEN 31

/* The max size of a buffer */
#define LINE_BUFFER_LEN 81

/* The max length of a string, the next 4 characters after the end of the line buffer are used for validations */
#define MAX_STRING_LEN (LINE_BUFFER_LEN + 4)

/* The max size of a token after cleaning whitespaces */
#define MAX_TOKEN_LEN 14

/* Its size is the size of MAX_STRING_LEN */
typedef unsigned char str_len_t;

#endif
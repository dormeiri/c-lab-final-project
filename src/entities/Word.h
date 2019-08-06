#ifndef WORD_HEADER
#define WORD_HEADER

#define WORD_SIZE 14       /*TODO r193 is reserved as well? */
/* (2^(WORD_SIZE-1) - 1) the msb as 0 represent positivity*/
#define MAX_NUM 8191
/* (2^(WORD_SIZE-1)) the msb as 1 represent negativity*/
#define MIN_NUM -8192

/*A, R, E is the 2 lsb's that indicates weather the value of the word is absolute, relocatable (changes according to the cell
that the word started to load in), or external (there's an external source that the value of the word is defined there) */
typedef enum
{
    A_ARE = 0,/*00 */
    R_ARE = 2,/*01 */
    E_ARE = 1 /*10 */
} Are;

typedef unsigned int Word;

/* word converter union creates logical break in a word to all of the ruqired components:
        -raw: the word in its raw form (without logical seperations)
        -operation word: two lsb are the A,R,E field, followed by two bits of the DESTINATION ADDRESSING TYPE, if exist.
            the next two bits are the SOURCE ADDRESSING TYPE, if exist
        -operand word: two lsb are the A,R,E field(the content may be immidate, represented by a symbol, or external), 
            followed by 12 bits to represent the data explicitly, reffer to a label, the external location it's defined in  */
typedef union
{
    Word raw;

    struct
    {
        Word are : 2;
        Word address_dest : 2;
        Word address_src : 2;
        Word op_code: 4;
    } operation_word;

    struct
    {
        Word are : 2;
        Word value : 12;
    } operand_word;

    struct
    {
        Word are : 2;
        Word dest : 3;
        Word source : 3;
    } register_word;
} word_converter;

#endif
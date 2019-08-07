#include "Word.h"
Word word_operation(Are are, AddressingType dest, AddressingType src, OperationType op_type)
{
    return (((((op_type << 2) | src) << 2) | dest) << 2) | are;
}
Word word_register(Word dest, Word src)
{
    return (((src << 3) | dest) << 2) | A_ARE;
}
Word word_push_are(Word word, Are are)
{
    return (word << 2) | are;
}
Word word_add_to_value(Word word, Word x)
{
    return ((word) + (x << 2)) | (word & 3); 
}

Word word_get_value(Word word)
{
    /* Word is unsigned */
    return word >> 2;
}

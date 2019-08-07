#ifndef IMAGE_LINE_HEADER
#define IMAGE_LINE_HEADER

#include "../helpers/Queue.h"
#include "../helpers/Error.h"
#include "Address.h"

typedef struct
{
    Queue *addresses;
} ImageLine;

ImageLine *image_line_new();

ErrorCode image_line_get_operation_word(OperationType op_type, ImageLine *il, Word *out);

void image_line_free(ImageLine *il);

#endif
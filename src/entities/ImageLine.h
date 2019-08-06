#ifndef IMAGE_LINE_HEADER
#define IMAGE_LINE_HEADER

#include "../helpers/Queue.h"
#include "Address.h"

typedef struct
{
    Queue *addresses;
} ImageLine;

ImageLine *image_line_new();
void image_line_free(ImageLine *il);

#endif
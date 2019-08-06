#include <stdlib.h>
#include "ImageLine.h"

ImageLine *image_line_new()
{
    ImageLine *result;
    if(!(result = (ImageLine *)malloc(sizeof(ImageLine))))
    {
        exit(EXIT_FAILURE);
    }

    result->addresses = queue_new(sizeof(Address));
    return result;
}
void image_line_free(ImageLine *il)
{
    if((il->addresses))
    {
        queue_free(il->addresses);
        free(il->addresses);
        il->addresses = NULL;
    }
}
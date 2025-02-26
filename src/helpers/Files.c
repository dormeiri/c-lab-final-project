#include "Files.h"
#include "../Commons.h"
#include <string.h>
#include <stdlib.h>

static void file_copy(FILE *source, FILE *dest);
static char *get_filename(const char *name, const char *extention);
static ErrorCode fgets_wrapper(FILE *fp, char **line_ref);
static ErrorCode get_file(char *filename, char *mode, FILE **fp_ref);
static char *convert_to_base4(Word value);

/****************/
/*    Public    */
/****************/

void files_remove_old_files(Assembler *as)
{
    char *filename;
    remove(filename = get_filename(as->name, OBJECT_EXT));
    if((filename)) free(filename);

    remove(filename = get_filename(as->name, ENTRY_EXT));
    if((filename)) free(filename);

    remove(filename = get_filename(as->name, EXTERN_EXT));
    if((filename)) free(filename);
}

void files_update_symbol_usage(Assembler *assembler, Symbol *symbol, SymbolUsage *sym_usage)
{
    fseek(assembler->output_fp, sym_usage->file_pos, SEEK_SET);
    files_write_address(assembler, sym_usage->address_index, symbol->value);
}

void files_frecopy(Assembler *assembler)
{
    FILE *fp = NULL;

    files_get_output(assembler, OBJECT_FILE, &fp);

    fprintf(fp, "%d %d\n", assembler->ic, assembler->dc);
    file_copy(assembler->output_fp, fp);
    fclose(fp);
}


ErrorCode files_get_input(Assembler *assembler, FILE **out)
{
    ErrorCode err;

    char *filename = get_filename(assembler->name, INPUT_EXT);

    err = get_file(filename, "r", out);

    free(filename);
    filename = NULL;
    
    return err;
}

ErrorCode files_get_output(Assembler *assembler, OutputFileType type, FILE **out)
{
    ErrorCode err;
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
            *out = tmpfile();
            if(out == NULL)
            {
                exit(EXIT_FAILURE);
            }
            return OK;
        
        default:
            filename = NULL;
            break;
    }

    err = get_file(filename, "w", out);

    free(filename);
    filename = NULL;

    return err;
}
ErrorCode files_read_line(Assembler *assembler, char **out)
{
    return fgets_wrapper(assembler->input_fp, out);
}

void files_write_address(Assembler *as, int address_index, Word value)
{
    fprintf(as->output_fp, "%04d %s\n", address_index, convert_to_base4(value));
}

/******************/
/*    Privates    */
/******************/

void file_copy(FILE *source, FILE *dest)
{
    char c;

    rewind(source);
    while((c = fgetc(source)) != EOF)
    {
        putc(c, dest);
    }
}

ErrorCode fgets_wrapper(FILE *fp, char **line_ref)
{
    char buffer[MAX_STRING_LEN] = { '\0' };
    size_t line_len;

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
    strcpy(*line_ref, buffer);

    return OK;
}

/* Save the result in fp_ref, return error on fail */
ErrorCode get_file(char *filename, char *mode, FILE **fp_ref)
{
    return (*fp_ref = fopen(filename, mode)) ? OK : FILE_ERROR;
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

char *convert_to_base4(Word value)
{
    char *result;
    Word mask;
    int i;

    /* TODO: Improve, could be done without reversing */

    result = (char *)malloc((sizeof(Word) + 1) * sizeof(char));
    for(mask = 3, i = 0; (mask << ((sizeof(Word) * 8) - WORD_SIZE)) != 0; mask <<= 2, i++)
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
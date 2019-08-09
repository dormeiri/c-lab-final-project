#include <string.h>
#include "StepOne.h"
#include "entities/Symbol.h"
#include "helpers/Files.h"
#include "helpers/Parsing.h"
#include "helpers/Validations.h"

#define TRY_THROW_S1(FUNC, INFO) {\
    ErrorCode RES;\
    if((RES = (FUNC)) != OK)\
    {\
        create_step_one_error(step_one, RES, INFO);\
        return FALSE;\
    }\
}

static StepOne *step_one_new(Assembler *assembler);
static void create_step_one_error(StepOne *step_one, ErrorCode ErrorCode, const char *info);
static Boolean step_one_line_algo(StepOne *step_one);
static void step_one_free_runtime_objs(StepOne *step_one);
static void step_one_free(StepOne *step_one);

static ErrorCode enqueue_string_address(StepOne *step_one, const char *str);
static ErrorCode enqueue_address(StepOne *step_one);

static ErrorCode append_line(StepOne *step_one);
static ErrorCode append_image_lines(StepOne *step_one);
static ErrorCode append_operation_line(StepOne *s1, OperationType op_type, ImageLine *il);
static void append_data_image(StepOne *step_one);

/*run step one is the controller of step one, it creates the step one entity, reads line after line, process it, and handles
        the release of all memory blocks allocated in this step, before returning to run assembler and continue to step two or
        stops due to errors in first step.
            Params:
                -assembler: an assembler struct that holds the file that is processed by the step.
(assembler holds the success key to indicate how to behave after this step ends.) */
void run_step_one(Assembler *assembler)
{
    StepOne *step_one;
    step_one = step_one_new(assembler);

    while(files_read_line(assembler, &step_one->curr_line) == OK)
    {
        if(!step_one_line_algo(step_one))
        {
            assembler->succeed = FALSE;
        }
        step_one->line_counter++;
        step_one_free_runtime_objs(step_one);
    }
    append_data_image(step_one);
    step_one_free(step_one);
    free(step_one);
    step_one = NULL;
}

/*Create step one takes an assembler struct and wraps it with step one struct to handle the specific
    requirements of this step (e.g. pointer to the current line that is parsed, lines counter)
        Params: 
            -assembler: the current file to parse wraped in an assembler struct
        return: step one struct, with the assembler includes IN it. */
StepOne *step_one_new(Assembler *assembler)
{
    StepOne *result;

    result = (StepOne *)malloc(sizeof(StepOne));
    if(!result)
    {
        exit(EXIT_FAILURE);
    }
    
    if(!(result->curr_line = (char *)malloc(sizeof(char) * MAX_STRING_LEN)))
    {
        exit(EXIT_FAILURE);
    }

    if(!(result->curr_line_copy = (char *)malloc(sizeof(char) * MAX_STRING_LEN)))
    {
        exit(EXIT_FAILURE);
    }

    result->assembler = assembler;
    result->line_counter = 1;
    result->data_image = queue_new(sizeof(Address));

    return result;
}

void create_step_one_error(StepOne *step_one, ErrorCode ErrorCode, const char *info)
{
    error_print(ErrorCode, step_one->line_counter, step_one->assembler->name, step_one->curr_line_copy, info);
}

/*step one line algo handles the logics of handling a single line, since the connection between lines is established 
    only in step two, the fist step looks at every line as a seperate entity and process it, leaving unprocessed declarions
    for later handle by step two.
        Params:
            -step one: pointer to step one struct consistent with the file that's currently handled
        return: true if the line processed with no errors false if any errors occured while parsing
             */
Boolean step_one_line_algo(StepOne *step_one)
{
    char *temp_str;
    ErrorCode res;
    Word temp_value;

    strcpy(step_one->curr_line_copy, step_one->curr_line);

    step_one->curr_statement = statement_new();
    TRY_THROW_S1(map_statement(step_one), NULL);

    if(step_one->curr_statement->tag)
    {
        TRY_THROW_S1(is_valid_tag(step_one->curr_statement->tag), step_one->curr_statement->tag);
        if(step_one->curr_statement->statement_type == EXTERN_KEY || step_one->curr_statement->statement_type == ENTRY_KEY)
        {
            create_step_one_error(step_one, ENT_EXT_TAG, step_one->curr_statement->tag);
        }
        else
        {
            SymbolProperty prop = statement_get_symbol_property(step_one->curr_statement);
            temp_str = step_one->curr_statement->tag;
            temp_value = (step_one->curr_statement->statement_type == OPERATION_KEY) ? step_one->assembler->ic : step_one->assembler->dc;
            TRY_THROW_S1(add_symbol_declaration(step_one->assembler->symbols_table, temp_str, prop, temp_value), temp_str);
        }
    }

    switch (step_one->curr_statement->statement_type)
    {
        case MACRO_KEY:
            TRY_THROW_S1(step_one->curr_statement->tag ? INVALID_COMB_LABEL_MACRO : OK, NULL);
            TRY_THROW_S1(parse_macro_statement(step_one, &temp_str, &temp_value), NULL);
            TRY_THROW_S1(add_symbol_declaration(step_one->assembler->symbols_table, temp_str, MACRO_SYM, temp_value), NULL);
            return TRUE;

        case EXTERN_KEY:
            get_label_arg(step_one, &temp_str);
            TRY_THROW_S1(add_symbol_declaration(step_one->assembler->symbols_table, temp_str, EXTERN_SYM, 0), NULL);
            break;

        case ENTRY_KEY:
            get_label_arg(step_one, &temp_str);
            TRY_THROW_S1(add_entry_declaration(step_one->assembler->symbols_table, temp_str), NULL);
            break;

        case DATA_KEY:
        case OPERATION_KEY:
            while((res = enqueue_address(step_one)) == OK);
            TRY_THROW_S1(res == MISSING_PARAM ? OK : res, NULL);
            TRY_THROW_S1(append_line(step_one), NULL);
            break;

        case STRING_KEY:
            {
                char *str;
                TRY_THROW_S1(get_string_arg(step_one, &str), NULL);
                enqueue_string_address(step_one, str);
                TRY_THROW_S1(append_line(step_one), NULL);
            }
            break;

        default:
            return TRUE;
            break;
    }
    return TRUE;
}

void step_one_free_runtime_objs(StepOne *step_one)
{
    if(step_one->curr_statement)
    {
        statement_free(step_one->curr_statement);
        step_one->curr_statement = NULL;
    }
}

void step_one_free(StepOne *step_one)
{
    if(step_one->curr_line)
    {
        free(step_one->curr_line);
        step_one->curr_line = NULL;
    }
    if(step_one->curr_line_copy)
    {
        free(step_one->curr_line_copy);
        step_one->curr_line_copy = NULL;
    }
    queue_free(step_one->data_image);
    free(step_one->data_image);
    step_one->data_image = NULL;
}


ErrorCode enqueue_string_address(StepOne *step_one, const char *str)
{
    Address *temp_address;

    TRY_THROW(step_one->data_image->counter == DATA_SIZE ? DATA_OVERFLOW : OK);

    do
    {
        temp_address = address_new();
        temp_address->symbol_name = NULL;
        temp_address->value = *str;
        temp_address->type = INSTANT;
        temp_address->index = step_one->assembler->dc++;
        enqueue(step_one->data_image, temp_address);
    } while ((*str++));
    return OK;
}

ErrorCode enqueue_address(StepOne *step_one)
{
    ErrorCode res;
    Address *curr_address;

    if((res = get_next_arg(step_one, &curr_address)) == OK)
    {
        if(step_one->curr_statement->statement_type == DATA_KEY)
        {
            TRY_THROW(step_one->assembler->dc == DATA_SIZE ? DATA_OVERFLOW : OK);
            curr_address->index = step_one->assembler->dc++;
            enqueue(step_one->data_image, curr_address);
        }
        else
        {
            curr_address->index = step_one->assembler->ic++;
            enqueue(step_one->curr_statement->image_line->addresses, curr_address);
        }
    }
    return res;
}

void step_one_add_symbol_usage(StepOne *step_one, Address *Address)
{
    if(Address->symbol_name)
    {
        SymbolTable *tab = step_one->assembler->symbols_table;
        long pos = ftell(step_one->assembler->output_fp);
        long linen = step_one->line_counter;
        const char *lines = step_one->curr_line_copy; /* TODO: Copy */
        long adrs_i = Address->index + DATA_SIZE; /* This is used by extern symbol in step two */
        add_symbol_usage(tab, Address->symbol_name, pos, linen, lines, adrs_i);
    }
}

/*append line habdles the parsing of an entire line (breakes line after the label, if exists, was extracted to operation
    and operand)
        Params:
            -step one: pointer to step one struct consistent with the file that's currently handled
        return: OK if line was appended properly, or error code of append operation*/
ErrorCode append_line(StepOne *step_one)
{
    Statement *statement;
    statement = step_one->curr_statement;

    if(step_one->curr_statement->operation_type != NONE_OP)
    {
        return append_operation_line(step_one, statement->operation_type, statement->image_line);
    }
    else
    {
        append_image_lines(step_one);
    }
    
    return OK;
}

ErrorCode append_operation_line(StepOne *s1, OperationType op_type, ImageLine *il)
{
    Word op_word;
    
    /* number of addresses may change while mapping image line addresses */
    s1->assembler->ic -= s1->curr_statement->image_line->addresses->counter;

    TRY_THROW(image_line_get_operation_word(op_type, il, &op_word));
    files_write_address(s1->assembler, DATA_SIZE + s1->assembler->ic, op_word);

    s1->assembler->ic += s1->curr_statement->image_line->addresses->counter + 1;
    append_image_lines(s1);

    return OK;
}

/*append image lines writes to the end of the tmp output file the parsed image lines in base "special four"
        Params:
            -step one: -step one: pointer to step one struct consistent with the file that's currently handled
        return: TODO if it returns ONLY 'OK' why not making it void?*/  
ErrorCode append_image_lines(StepOne *step_one)
{
    Address *curr_address;  
    while((curr_address = (Address *)dequeue(step_one->curr_statement->image_line->addresses)))
    {
        if(curr_address->symbol_name)
        {
            step_one_add_symbol_usage(step_one, curr_address);
        }
        if(curr_address->type == ARRAY)
        {
            files_write_address(step_one->assembler, DATA_SIZE + curr_address->index, 0);
            curr_address->index++;
            step_one->assembler->ic++;
        }

        files_write_address(step_one->assembler, DATA_SIZE + curr_address->index, curr_address->value);
    }
    return OK;
}

void append_data_image(StepOne *step_one)
{
    Address *temp;
    while((temp = dequeue(step_one->data_image)))
    {
        files_write_address(step_one->assembler, DATA_SIZE + temp->index + step_one->assembler->ic, temp->value);
    }
}
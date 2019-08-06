#include <string.h>
#include "StepOne.h"
#include "entities/Symbol.h"
#include "helpers/Files.h"
#include "helpers/Parsing.h"

#define DATA_SIZE 100

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
static boolean step_one_line_algo(StepOne *step_one);
static void step_one_free_runtime_objs(StepOne *step_one);
static void step_one_free(StepOne *step_one);

static ErrorCode enqueue_string_address(StepOne *step_one, const char *str);
static ErrorCode enqueue_address(StepOne *step_one);

static ErrorCode append_line(StepOne *step_one);
static ErrorCode append_image_lines(StepOne *step_one);
static ErrorCode append_operation(StepOne *step_one, OperationType operation_type, ImageLine *image_line);
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
    result->address_index = DATA_SIZE;
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
boolean step_one_line_algo(StepOne *step_one)
{
    char *temp_str;
    ErrorCode res;
    Word temp_value;

    
    strcpy(step_one->curr_line_copy, step_one->curr_line);

    step_one->curr_statement = statement_new();
    TRY_THROW_S1(map_statement(step_one), NULL);

    if(step_one->curr_statement->tag)
    {
        temp_str = step_one->curr_statement->tag;
        temp_value = step_one->address_index;
        /* TODO: Make it shorter */
        TRY_THROW_S1(add_symbol_declaration(step_one->assembler->symbols_table, temp_str, statement_get_symbol_property(step_one->curr_statement), temp_value, step_one->address_index), NULL);
    }

    switch (step_one->curr_statement->statement_type)
    {
        case MACRO_KEY:
            TRY_THROW_S1(step_one->curr_statement->tag ? INVALID_COMB_LABEL_MACRO : OK, NULL);
            TRY_THROW_S1(parse_macro_statement(step_one, &temp_str, &temp_value), NULL);
            TRY_THROW_S1(add_symbol_declaration(step_one->assembler->symbols_table, temp_str, MACRO_SYM, temp_value, step_one->address_index), NULL);
            return TRUE;

        case EXTERN_KEY:
            get_label_arg(step_one, &temp_str);
            TRY_THROW_S1(add_symbol_declaration(step_one->assembler->symbols_table, temp_str, EXTERN_SYM, 0, step_one->address_index), NULL);
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
        free(step_one->curr_line);
        step_one->curr_line = NULL;
    }
    queue_free(step_one->data_image);
    free(step_one->data_image);
}


ErrorCode enqueue_string_address(StepOne *step_one, const char *str)
{
    Address *temp_address;

    TRY_THROW(step_one->data_image->counter == DATA_SIZE ? DATA_OVERFLOW : OK);

    do
    {
        /* TODO: New address */
        if(!(temp_address = (Address *)malloc(sizeof(temp_address))))
        {
            exit(EXIT_FAILURE);
        }
        temp_address->symbol_name = NULL;
        temp_address->value = *str;
        temp_address->type = INSTANT;
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
            TRY_THROW(step_one->data_image->counter == DATA_SIZE ? DATA_OVERFLOW : OK);
            enqueue(step_one->data_image, curr_address);
        }
        else
        {
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
        const char *lines = step_one->curr_line_copy;
        long adrs_i = step_one->address_index;
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
        return append_operation(step_one, statement->operation_type, statement->image_line);
    }
    else
    {
        append_image_lines(step_one);
    }
    
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
            files_write_address(step_one->assembler, step_one->address_index++, 0);
        }

        files_write_address(step_one->assembler, step_one->address_index++, curr_address->value);
    }
    return OK;
}


/*append operation writes operations and operands to the tmp source code, it verifies the number of operands with 
    the operarion describes.
        Params:
            -step one: pointer to step one struct consistent with the file that's currently handled
            -operation type: enum represents the operation that's handled
            -image line: the image line pointer of the current statement that's habndled
        return: OK if there's a operation-operand match (number wise)
                     or indicates weather there are too many operands or less than requied.*/
ErrorCode append_operation(StepOne *step_one, OperationType operation_type, ImageLine *image_line)
{
    word_converter operation_word;
    Queue *raw_queue;
    Queue *verified_queue;
    char num_of_operands;
    Address *curr_address;
    Address *last_address;

    raw_queue = image_line->addresses;
    verified_queue = queue_new(sizeof(Address));

    operation_word.raw = 0;
    operation_word.operation_word.op_code = operation_type;

    num_of_operands = operation_operands(operation_type);

    switch (num_of_operands)
    {
        case 2:
            TRY_THROW((curr_address = (Address *)dequeue(raw_queue)) ? OK : MISSING_PARAM);
            TRY_THROW((operation_type == LEA_OP) && ((curr_address->type) != DIRECT)
                ? INVALID_ARGUMENT
                : OK);
            operation_word.operation_word.address_src = curr_address->type;
            curr_address->value = get_operand_word(curr_address);
            enqueue(verified_queue, last_address = curr_address);
            /* There is no break, it'll continue to case 1 */
        case 1:
            TRY_THROW((curr_address = (Address *)dequeue(raw_queue)) ? OK : MISSING_PARAM);
            operation_word.operation_word.address_dest = curr_address->type;
            curr_address->value = get_operand_word(curr_address);
            if(curr_address->type == REGISTER && last_address->type == REGISTER)
            {
                /* If two register found both in source and destination arguments, they'll share one word */
                word_converter register_converter;
                register_converter.raw = 0;
                register_converter.register_word.dest = curr_address->value;
                register_converter.register_word.source = last_address->value;
                last_address->value = register_converter.raw;
                free(curr_address); /* TODO: address_free */
            }
            else
            {
                /* If not both of the arguments are register, enqueue to the created queue */
                enqueue(verified_queue, curr_address);
            }
            
            break;
    }
    if(!IS_EMPTY_QUEUE(raw_queue)) 
    {
        return TOO_MANY_OPERANDS;
    }

    queue_free(raw_queue);
    free(raw_queue);
    image_line->addresses = verified_queue;

    files_write_address(step_one->assembler, step_one->address_index++, operation_word.raw);
    append_image_lines(step_one);

    return OK;
}

void append_data_image(StepOne *step_one)
{
    Address *temp;
    step_one->assembler->ic = step_one->address_index - DATA_SIZE;
    step_one->assembler->dc = step_one->data_image->counter;

    while((temp = dequeue(step_one->data_image)))
    {
        files_write_address(step_one->assembler, step_one->address_index++, temp->value);
    }
}

/*get operand word is a sub routine of append operation to handle operands convertion and classification
    Params:
        -operand: pointer to an Address struct contains an operand
    return: word struct cosistent with the operand.... */
Word get_operand_word(Address *operand)
{
    word_converter w;
    w.operand_word.value = operand->value;
    switch (operand->type)
    {
        case REGISTER:
        case INSTANT:
        default:
            w.operand_word.are = A_ARE;
            break;
    }
    w.operand_word.value = operand->value;

    return w.raw;
}

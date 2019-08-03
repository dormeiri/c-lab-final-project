#include <string.h>
#include "step_one.h"
#include "symbols.h"
#include "helpers/files.h"
#include "helpers/parsing.h"

#define DATA_SIZE 100

/* TODO: Organize, maybe split into smaller files */

static word get_operand_word(address *operand);
static char operation_operands(operationType op_type);
static ErrorCode append_image_lines(step_one *step_one);
static void free_step_one_objs();
static statement *create_statement();
static step_one *create_step_one(assembler *assembler);
static symbolProperty get_symbol_property(statement *statement);
static boolean step_one_line_algo(step_one *step_one);
static ErrorCode enqueue_string_address(step_one *step_one, const char *str);
static ErrorCode enqueue_address(step_one *step_one);
static void create_step_one_error(step_one *step_one, ErrorCode ErrorCode, const char *info);
/* Translate the current image_line of the current statement in step_one and append the addresses to the file */
static ErrorCode append_line(step_one *step_one);
static ErrorCode append_operation(step_one *step_one, operationType operation_type, image_line *image_line);
static void append_data_image(step_one *step_one);



/*run step one is the controller of step one, it creates the step one entity, reads line after line, process it, and handles
        the release of all memory blocks allocated in this step, before returning to run assembler and continue to step two or
        stops due to errors in first step.
            Params:
                -assembler: an assembler struct that holds the file that is processed by the step.
(assembler holds the success key to indicate how to behave after this step ends.) */
void run_step_one(assembler *assembler)
{
    step_one *step_one;
    step_one = create_step_one(assembler);

    while(files_read_line(assembler, &step_one->curr_line) == OK)
    {
        if(!step_one_line_algo(step_one))
        {
            assembler->succeed = FALSE;
        }
        step_one->line_counter++;
        free_step_one_objs(step_one);
    }

    append_data_image(step_one);
    fclose(assembler->input_fp);
    queue_free(step_one->data_image);
    free(step_one->data_image);
    free(step_one);
}

/*step one line algo handles the logics of handling a single line, since the connection between lines is established 
    only in step two, the fist step looks at every line as a seperate entity and process it, leaving unprocessed declarions
    for later handle by step two.
        Params:
            -step one: pointer to step one struct consistent with the file that's currently handled
        return: true if the line processed with no errors false if any errors occured while parsing
             */
boolean step_one_line_algo(step_one *step_one)
{
#define TRY_THROW_S1(FUNC, INFO) {\
    ErrorCode RES;\
    if((RES = (FUNC)) != OK)\
    {\
        create_step_one_error(step_one, RES, INFO);\
        return FALSE;\
    }\
}

    ErrorCode res;
    char *temp_str;
    word temp_value;

    temp_str = (char *)malloc(MAX_STRING_LEN * sizeof(char));
    step_one->curr_line_copy = strncpy(temp_str, step_one->curr_line, MAX_STRING_LEN);

    step_one->curr_statement = create_statement();
    TRY_THROW_S1(map_statement(step_one), NULL);

    if(step_one->curr_statement->tag)
    {
        temp_str = step_one->curr_statement->tag;
        temp_value = step_one->address_index;
        /* TODO: Make it shorter */
        TRY_THROW_S1(add_symbol_declaration(step_one->assembler->symbols_table, temp_str, get_symbol_property(step_one->curr_statement), temp_value, step_one->address_index), NULL);
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

ErrorCode enqueue_address(step_one *step_one)
{
    ErrorCode res;
    address *curr_address;

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

ErrorCode enqueue_string_address(step_one *step_one, const char *str)
{
    address *temp_address;

    TRY_THROW(step_one->data_image->counter == DATA_SIZE ? DATA_OVERFLOW : OK);

    do
    {
        if(!(temp_address = (address *)malloc(sizeof(temp_address))))
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


void create_step_one_error(step_one *step_one, ErrorCode ErrorCode, const char *info)
{
    error_print(ErrorCode, step_one->line_counter, step_one->assembler->name, step_one->curr_line_copy, info);
}
/*get symbol property handles the classification of symols according to its context, its a subroutin of add symbol declaration
    Params:
        -statement: the statement that's its symbol's property we wish to define
    return: the statement type the symbol represents*/
symbolProperty get_symbol_property(statement *statement)
{
    switch (statement->statement_type)
    {
        case MACRO_KEY: return MACRO_SYM;
        case OPERATION_KEY: return CODE_SYM;
        default: return DATA_SYM;
    }
}

void step_one_add_symbol_usage(step_one *step_one, address *address)
{
    if(address->symbol_name)
    {
        symbols_table *tab = step_one->assembler->symbols_table;
        long pos = ftell(step_one->assembler->output_fp);
        long linen = step_one->line_counter;
        const char *lines = step_one->curr_line_copy;
        long adrs_i = step_one->address_index;
        add_symbol_usage(tab, address->symbol_name, pos, linen, lines, adrs_i);
    }
}

/*append line habdles the parsing of an entire line (breakes line after the label, if exists, was extracted to operation
    and operand)
        Params:
            -step one: pointer to step one struct consistent with the file that's currently handled
        return: OK if line was appended properly, or error code of append operation*/
ErrorCode append_line(step_one *step_one)
{
    statement *statement;
    statement = step_one->curr_statement;
    if(step_one->curr_statement->operation_type != NONE_OP)
    /*TODO maybe by making append operation subroutine of append image line, we can make append line super thin (and maybe even 
            merge append line with append image line) */
    {
        return append_operation(step_one, statement->operation_type, statement->image_line);
    }
    else
    {
        append_image_lines(step_one);
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
ErrorCode append_operation(step_one *step_one, operationType operation_type, image_line *image_line)
{
    word_converter operation_word;
    Queue *raw_queue;
    Queue *verified_queue;
    char num_of_operands;
    address *curr_address;
    address *last_address;

    raw_queue = image_line->addresses;
    verified_queue = queue_new(sizeof(address));

    operation_word.raw = 0;
    operation_word.operation_word.op_code = operation_type;

    num_of_operands = operation_operands(operation_type);

    switch (num_of_operands)
    {
        case 2:
            TRY_THROW((curr_address = (address *)dequeue(raw_queue)) ? OK : MISSING_PARAM);
            TRY_THROW((operation_type == LEA_OP) && ((curr_address->type) != DIRECT)
                ? INVALID_ARGUMENT
                : OK);
            operation_word.operation_word.address_src = curr_address->type;
            curr_address->value = get_operand_word(curr_address);
            enqueue(verified_queue, last_address = curr_address);
            /* There is no break, it'll continue to case 1 */
        case 1:
            TRY_THROW((curr_address = (address *)dequeue(raw_queue)) ? OK : MISSING_PARAM);
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
/*get operand word is a sub routine of append operation to handle operands convertion and classification
    Params:
        -operand: pointer to an address struct contains an operand
    return: word struct cosistent with the operand.... 
    TDOD is it fully implemented?*/
word get_operand_word(address *operand)
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

void append_data_image(step_one *step_one)
{
    address *temp;
    step_one->assembler->ic = step_one->address_index - DATA_SIZE;
    step_one->assembler->dc = step_one->data_image->counter;

    while((temp = dequeue(step_one->data_image)))
    {
        files_write_address(step_one->assembler, step_one->address_index++, temp->value);
    }
}

/*append image lines writes to the end of the tmp output file the parsed image lines in base "special four"
        Params:
            -step one: -step one: pointer to step one struct consistent with the file that's currently handled
        return: TODO if it returns ONLY 'OK' why not making it void?*/  
ErrorCode append_image_lines(step_one *step_one)
{
    address *curr_address;  
    while((curr_address = (address *)dequeue(step_one->curr_statement->image_line->addresses)))
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

/*operation operands received an operation names and return a number between 0-2 to represent the number of
operands the operation works on.
    Params: 
        -op_type: the operation that needs to be matched with the number of operands.
    return: 0 if the operation needs no operands in order to operate(e.g. "stop" operation), 
            1 if the operation needs a single operand to operate (e.g. "not" operation) 
            2 if the operation needs two operands to operate (e.g. "mov" operation)  */
char operation_operands(operationType op_type)
{
    switch (op_type)
    {
        case MOV_OP:
        case CMP_OP:
        case ADD_OP:
        case SUB_OP:
        case LEA_OP: return 2;

        case NOT_OP:
        case CLR_OP:
        case INC_OP:
        case DEC_OP:
        case JMP_OP:
        case BNE_OP:
        case RED_OP:
        case PRN_OP:
        case JSR_OP: return 1;

        case RTS_OP:
        case STOP_OP: return 0;

        default: return -1; /* It should not get here */
    }
}

/*Create step one takes an assembler struct and wraps it with step one struct to handle the specific
    requirements of this step (e.g. pointer to the current line that is parsed, lines counter)
        Params: 
            -assembler: the current file to parse wraped in an assembler struct
        return: step one struct, with the assembler includes IN it. */
step_one *create_step_one(assembler *assembler)
{
    step_one *result;

    result = (step_one *)malloc(sizeof(step_one));
    if(!result)
    {
        exit(EXIT_FAILURE);
    }
    
    if(!(result->curr_line = (char *)malloc(sizeof(char) * MAX_STRING_LEN)))
    {
        exit(EXIT_FAILURE);
    }

    result->assembler = assembler;
    result->line_counter = 1;
    result->address_index = DATA_SIZE;

    result->data_image = queue_new(sizeof(address));

    return result;
}

/* create statement initialize a statement struct by allocating memory block for it and initializing the an empty 
    adrresses queue
        return: initialized statement pointer. */
statement *create_statement()
{
    statement *result;

    if(!(result = (statement *)malloc(sizeof(statement))))
    {
        exit(EXIT_FAILURE);
    }
    if(!(result->image_line = (image_line *)malloc(sizeof(image_line))))
    {
        exit(EXIT_FAILURE);
    }
    result->image_line->addresses = queue_new(sizeof(address));
    result->tag = NULL;
    result->args = NULL;

    return result;
}

void free_step_one_objs(step_one *step_one)
{
    /* args and tag is taken from curr_line, and curr_line is pointing to static buffer in fgets_wrapper, therefore, there is no need of freeing */

    if(step_one)
    {
        if(step_one->curr_statement)
        {
            if(step_one->curr_statement->image_line)
            {
                if(step_one->curr_statement->image_line->addresses)
                {
                    queue_free(step_one->curr_statement->image_line->addresses);
                    free(step_one->curr_statement->image_line->addresses);
                }

                free(step_one->curr_statement->image_line);
            }

            free(step_one->curr_statement);
        }
        
        if(step_one->curr_line_copy)
        {
            free(step_one->curr_line_copy);
        }
    }
}


/*TODO take out the "create_" functions to an "initialization" module */
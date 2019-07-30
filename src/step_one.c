#include <string.h>
#include "step_one.h"
#include "symbols.h"
#include "helpers/files.h"
#include "helpers/parsing.h"

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
    fclose(assembler->input_fp);
    free(step_one);
}

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
        TRY_THROW_S1(add_symbol_declaration(step_one->assembler->symbols_table, temp_str, get_symbol_property(step_one->curr_statement), temp_value), NULL);
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

        case OPERATION_KEY:
        case DATA_KEY:
            
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
        enqueue(step_one->curr_statement->image_line->addresses, curr_address);
    }
    return res;
}

ErrorCode enqueue_string_address(step_one *step_one, const char *str)
{
    address *temp_address;
    for(; *str; str++)
    {
        if(!(temp_address = (address *)malloc(sizeof(temp_address))))
        {
            exit(EXIT_FAILURE);
        }
        temp_address->value = *str;
        temp_address->type = INSTANT;
        enqueue(step_one->curr_statement->image_line->addresses, temp_address);
    }
    return OK;
}


void create_step_one_error(step_one *step_one, ErrorCode ErrorCode, const char *info)
{
    error_print(ErrorCode, step_one->line_counter, step_one->assembler->name, step_one->curr_line_copy, info);
}

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

ErrorCode append_line(step_one *step_one)
{
    statement *statement;
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


ErrorCode append_operation(step_one *step_one, operationType operation_type, image_line *image_line)
{
    word_converter w;
    QueueNode *nptr;
    char num_of_operands;
    address *curr_address;

    w.raw = 0;
    w.operation_word.op_code = operation_type;

    num_of_operands = operation_operands(operation_type);
    nptr = image_line->addresses->head;
    while(num_of_operands--)
    {
        TRY_THROW((nptr) ? OK : MISSING_PARAM);
        curr_address = ((address *)(nptr->data));
        if(w.operation_word.address_src)
        {
            w.operation_word.address_dest = curr_address->type;
        }
        else
        {
            w.operation_word.address_src = curr_address->type;
        }
        
        curr_address->value = get_operand_word(curr_address);
        nptr = nptr->next;
    }
    if(nptr) 
    {
        return TOO_MANY_OPERANDS;
    }

    files_write_address(step_one->assembler, step_one->address_index++, w.raw);
    append_image_lines(step_one);

    return OK;
}
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
    result->address_index = 100;

    return result;
}

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
    result->image_line->addresses = initilize_queue(sizeof(address));

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
                    free_queue(step_one->curr_statement->image_line->addresses);
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
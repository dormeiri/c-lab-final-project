#include <stdio.h>
#include <string.h>

#include "unit_test.h"

#define RESET   "\033[0m"
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */

#define GENERIC_EQUAL(TYPE,A,B) (*(TYPE *)A == *(TYPE *)B)
#define GENERIC_TOSTRING(OUTP, IN, TYPE, FRMT) sprintf(OUTP, FRMT, *(TYPE *)IN)
#define PRINT_TEST_RESULT(NAME, RES, A, B) printf("%s -> %s (Actual: %s, Expected: %s)\n", NAME, RES == SUCCESS ? (GREEN "SUCCESS" RESET) : (RED "ERROR" RESET), A, B);

void assert(char *name, void *actual, void *expected, typeCode type)
{
    char actual_str[BUFFER_SIZE];
    char expected_str[BUFFER_SIZE];
    
    testResult result;

    switch (type)
    {
        case CHAR:      
            result = GENERIC_EQUAL(char, expected, actual) ? SUCCESS : FAIL;
            sprintf(actual_str, "%c", *(char *)actual);
            sprintf(expected_str, "%c", *(char *)expected);
            break;
        case SHORT:     
            result = GENERIC_EQUAL(short, expected, actual) ? SUCCESS : FAIL;
            sprintf(actual_str, "%hd", *(short *)actual);
            sprintf(expected_str, "%hd", *(short *)expected);
            break;
        case USHORT:    
            result = GENERIC_EQUAL(unsigned short, expected, actual) ? SUCCESS : FAIL;
            sprintf(actual_str, "%hu", *(unsigned short *)actual);
            sprintf(expected_str, "%hu", *(unsigned short *)expected);
            break;
        case INT:       
            result = GENERIC_EQUAL(int, expected, actual) ? SUCCESS : FAIL;
            sprintf(actual_str, "%d", *(int *)actual);
            sprintf(expected_str, "%d", *(int *)expected);
            break;
        case UINT:      
            result = GENERIC_EQUAL(unsigned, expected, actual) ? SUCCESS : FAIL;
            sprintf(actual_str, "%u", *(unsigned *)actual);
            sprintf(expected_str, "%u", *(unsigned *)expected);
            break;
        case LONG:      
            result = GENERIC_EQUAL(long, expected, actual) ? SUCCESS : FAIL;
            sprintf(actual_str, "%ld", *(long *)actual);
            sprintf(expected_str, "%ld", *(long *)expected);
            break;
        case ULONG:     
            result = GENERIC_EQUAL(unsigned long, expected, actual) ? SUCCESS : FAIL;
            sprintf(actual_str, "%lu", *(unsigned long *)actual);
            sprintf(expected_str, "%lu", *(unsigned long *)expected);
            break;
        case STRING:    
            result = ((strcmp(*(char **)actual, *(char **)expected) == 0) ? SUCCESS : FAIL);
            strncpy(actual_str, *(char **)actual, 50);
            strncpy(expected_str, *(char **)expected, 50);
            break;

        default:
            fprintf(stderr,"Bad call to function 'assert'");
            return;
    }

    PRINT_TEST_RESULT(name, result, actual_str, expected_str);
}

#include <string.h>
#include <stdlib.h>

#include "unit_test.h"
#include "../../src//helpers/files.h"
#include "../../src/commons.h"

#define TEST_STR "12345 Success!"

void get_input_file_test();
void get_output_file_test();
void fgets_wrapper_test();
void fgets_wrapper_test_2();

int main()
{
    get_output_file_test();    
    get_input_file_test();
    fgets_wrapper_test();
    fgets_wrapper_test_2();

    return 0;
}

void get_output_file_test()
{
    static char *name = "get_output_file_test";
    static char *filename = "src/files_input/test.txt";

    FILE *fp;

    errorCode res_actual;
    errorCode res_expected;

    res_actual = get_output_file(filename, &fp);
    res_expected = OK;
    assert(name, &res_actual, &res_expected, INT);

    fprintf(fp, TEST_STR);
    fclose(fp);
}

void get_input_file_test()
{
    static char *name = "get_input_file_test";
    static char *filename = "src/files_input/test.txt";

    FILE *fp;

    errorCode res_actual;
    errorCode res_expected;

    char *expected;
    char *actual;

    char ch;
    int i;

    res_actual = get_input_file(filename, &fp);
    res_expected = OK;
    assert(name, &res_actual, &res_expected, INT);

    actual = (char *)malloc(BUFFER_SIZE * sizeof(char));
    i = 0;
    while((ch = fgetc(fp)) != EOF)
    {
        actual[i++] = ch;
    }
    actual[i++] = '\0';

    expected = (char *)malloc(BUFFER_SIZE * sizeof(char));
    strncpy(expected, TEST_STR, BUFFER_SIZE);
    assert(name, &actual, &expected, STRING);

    free(actual);
    free(expected);
    fclose(fp);
}

void fgets_wrapper_test()
{
    static char *name = "fgets_wrapper_test";
    static char *filename = "src/files_input/test2.txt";

    char *line;
    char *expected = (char *)malloc(BUFFER_SIZE * sizeof(char));

    FILE *fp;

    errorCode res_actual;
    errorCode res_expected;

    get_input_file(filename, &fp);

    fgets_wrapper(fp, &line);
    strncpy(expected, "", BUFFER_SIZE);
    assert(name, &line, &expected, STRING);

    fgets_wrapper(fp, &line);
    strncpy(expected, TEST_STR, BUFFER_SIZE);
    assert(name, &line, &expected, STRING);

    fgets_wrapper(fp, &line);
    strncpy(expected, "1", BUFFER_SIZE);
    assert(name, &line, &expected, STRING);

    fgets_wrapper(fp, &line);
    strncpy(expected, "", BUFFER_SIZE);
    assert(name, &line, &expected, STRING);

    res_actual = fgets_wrapper(fp, &line);
    strncpy(expected, "2", BUFFER_SIZE);
    assert(name, &line, &expected, STRING);
    res_expected = OK;
    assert(name, &res_actual, &res_expected, INT);

    res_actual = fgets_wrapper(fp, &line);
    res_expected = EOF_OCCURED;
    assert(name, &res_actual, &res_expected, INT);

    fclose(fp);
}


void fgets_wrapper_test_2()
{
    static char *name = "fgets_wrapper_test_2";
    static char *filename = "src/files_input/test3.txt";

    char *line;
    char *expected = (char *)calloc(BUFFER_SIZE, sizeof(char));

    FILE *fp;

    errorCode res_actual;
    errorCode res_expected;

    get_input_file(filename, &fp);

    res_actual = fgets_wrapper(fp, &line);
    strncpy(expected, "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff79", BUFFER_SIZE);
    assert(name, &line, &expected, STRING);

    res_expected = OK;
    assert(name, &res_actual, &res_expected, INT);

    res_actual = fgets_wrapper(fp, &line);
    assert(name, &line, &expected, STRING);

    res_expected = BUF_LEN_EXCEEDED;
    assert(name, &res_actual, &res_expected, INT);

    res_actual = fgets_wrapper(fp, &line);
    strncpy(expected, "", BUFFER_SIZE);
    assert(name, &line, &expected, STRING);

    res_expected = OK;
    assert(name, &res_actual, &res_expected, INT);

    res_actual = fgets_wrapper(fp, &line);
    res_expected = EOF_OCCURED;
    assert(name, &res_actual, &res_expected, INT);

    fclose(fp);
}
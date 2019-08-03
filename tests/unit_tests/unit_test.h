#define BUFFER_SIZE 80

typedef enum testResult
{
    FAIL = 0,
    SUCCESS = 1
} testResult;

typedef enum typeCode
{
    CHAR,
    SHORT,
    USHORT,
    INT,
    UINT,
    LONG,
    ULONG,
    STRING
} typeCode;

void assert(char *name, void *actual, void *expected, typeCode type);
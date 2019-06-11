CC = gcc
CFLAGS = -pedantic -Wall -ansi
DEPS = src/common.h src/commons.h src/common.h src/errors.h src/helpers/parsing.h src/helpers/validations.h src/helpers/queue.h src/helpers/files.h tests/unit_tests/unit_test.h
OBJ = src/errors.o src/helpers/parsing.o src/helpers/validations.o src/helpers/queue.o src/helpers/files.o
UTOBJ = tests/unit_tests/unit_test.o
ASOBJ = src/assembler.o

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

### Main execution file ###

bin/assembler: $(OBJ) $(ASOBJ)
	$(CC) $(CFLAGS) -g $^ -o $@
	rm $(OBJ) $(ASOBJ)

assembler: bin/assembler
: assembler

### Utilities commands ###

clean:
	rm $(OBJ) $(UTOBJ) $(ASOBJ) bin/assembler

run: assembler
	bin/assembler

debug: assembler
	gdb bin/assembler

### Unit tests make commands ###

tests/unit_tests/bin/parsing_test: $(OBJ) $(UTOBJ) tests/unit_tests/parsing_test.o
	$(CC) $(CFLAGS) -g $^ -o $@

parsing_test: tests/unit_tests/bin/parsing_test
	$^

tests/unit_tests/bin/validations_test: $(OBJ) $(UTOBJ) tests/unit_tests/bin/validations_test.o
	$(CC) $(CFLAGS) -g $^ -o $@

validations_test: tests/unit_tests/bin/validations_test
	$^

tests/unit_tests/bin/errors_test: $(OBJ) $(UTOBJ) tests/unit_tests/errors_test.o
	$(CC) $(CFLAGS) -g $^ -o $@

errors_test: tests/unit_tests/bin/errors_test
	$^

tests/unit_tests/bin/queue_test: $(OBJ) $(UTOBJ) tests/unit_tests/queue_test.o
	$(CC) $(CFLAGS) -g $^ -o $@

queue_test: tests/unit_tests/bin/queue_test
	$^

tests/unit_tests/bin/files_test: $(OBJ) $(UTOBJ) tests/unit_tests/files_test.o
	$(CC) $(CFLAGS) -g $^ -o $@

files_test: tests/unit_tests/bin/files_test
	$^

test_all:
	make parsing_test
	make validations_test
	make errors_test
	make queue_test
	make files_test
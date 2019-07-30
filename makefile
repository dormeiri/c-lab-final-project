CC = gcc
CFLAGS = -pedantic -Wall -ansi
DEPS = src/commons.h src/helpers/error.h src/assembler.h src/step_one.h src/step_two.h src/symbols.h src/helpers/parsing.h src/helpers/validations.h src/helpers/queue.h src/helpers/list.h src/helpers/files.h
OBJ = src/assembler.o src/helpers/error.o src/step_one.o src/step_two.o src/symbols.o src/helpers/parsing.o src/helpers/validations.o src/helpers/queue.o src/helpers/list.o src/helpers/files.o
UTOBJ = tests/unit_tests/unit_test.o

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

### Main execution file ###

assembler: $(OBJ) 
	$(CC) $(CFLAGS) -g $^ -o $@
	rm $(OBJ) $(ASOBJ)

assembler: assembler
: assembler

### Utilities commands ###

clean:
	rm $(OBJ) $(UTOBJ) $(ASOBJ) assembler

run: assembler
	assembler

debug: assembler
	gdb assembler

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
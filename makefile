CC = gcc
CFLAGS = -pedantic -Wall -ansi
DEPS = src/common.h src/errors.h src/helpers/parsing.h src/helpers/validations.h tests/unit_tests/unit_test.h
OBJ = src/helpers/parsing.o src/helpers/validations.o
UTOBJ = tests/unit_tests/unit_test.o
TESTSOBJ = tests/unit_tests/parsing_test.o tests/unit_tests/validations_test.o

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

# Main execution file

assembly: $(OBJ)
	$(CC) $(CFLAGS) -g $^ -o $@

# Utilities commands

clean:
	rm $(OBJ) $(UTOBJ) $(TESTSOBJ)

run: assembly
	./assembly

# Unit tests make commands

tests/unit_tests/parsing_test: $(OBJ) $(UTOBJ) tests/unit_tests/parsing_test.o
	$(CC) $(CFLAGS) -g $^ -o $@

parsing_test: tests/unit_tests/parsing_test
	./$^

tests/unit_tests/validations_test: $(OBJ) $(UTOBJ) tests/unit_tests/validations_test.o
	$(CC) $(CFLAGS) -g $^ -o $@

validations_test: tests/unit_tests/validations_test
	./$^

test_all:
	make parsing_test
	make validations_test
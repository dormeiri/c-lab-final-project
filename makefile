CC = gcc
CFLAGS = -pedantic -Wall -ansi
DEPS = src/common.h src/commons.h src/common.h src/errors.h src/helpers/parsing.h src/helpers/validations.h src/helpers/queue.h tests/unit_tests/unit_test.h
OBJ = src/errors.o src/helpers/parsing.o src/helpers/validations.o src/helpers/queue.o
UTOBJ = tests/unit_tests/unit_test.o

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

# Main execution file

assembly: $(OBJ)
	$(CC) $(CFLAGS) -g $^ -o $@

# Utilities commands

clean:
	rm $(OBJ) $(UTOBJ)

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

tests/unit_tests/errors_test: $(OBJ) $(UTOBJ) tests/unit_tests/errors_test.o
	$(CC) $(CFLAGS) -g $^ -o $@

errors_test: tests/unit_tests/errors_test
	./$^

tests/unit_tests/queue_test: $(OBJ) $(UTOBJ) tests/unit_tests/queue_test.o
	$(CC) $(CFLAGS) -g $^ -o $@

queue_test: tests/unit_tests/queue_test
	./$^

test_all:
	make parsing_test
	make validations_test
	make errors_test
	make queue_test
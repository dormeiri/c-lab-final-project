CC = gcc
CFLAGS = -pedantic -Wall -ansi
DEPS = src/Commons.h src/helpers/Error.h src/helpers/Parsing.h src/helpers/Validations.h src/helpers/Queue.h src/helpers/List.h src/helpers/Files.h src/entities/Word.h src/entities/Address.h src/entities/ImageLine.h src/entities/Statement.h src/StepOne.h src/StepTwo.h src/entities/Symbol.h src/Assembler.h
OBJ = src/helpers/Error.o src/helpers/Parsing.o src/helpers/Validations.o src/helpers/Queue.o src/helpers/List.o src/helpers/Files.o src/entities/Address.o src/entities/ImageLine.o src/entities/Statement.o src/StepOne.o src/StepTwo.o src/entities/Symbol.o src/Assembler.o
UTOBJ = tests/unit_tests/unit_test.o

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

assembler: $(OBJ) 
	$(CC) $(CFLAGS) -g $^ -o $@
	rm $(OBJ) $(ASOBJ)
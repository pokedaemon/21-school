CC=gcc
# delete all flags with -W before check
OPT=--std=c11

GCOV=-fprofile-arcs -ftest-coverage
LD=-lcheck -fprofile-arcs --coverage

# OPT+=-fsanitize=address -g -O1 -fno-omit-frame-pointer -Wno-error=deprecated-declarations
# LD+=-fsanitize=address

SRC=s21_arithmetic.c s21_compare.c \
s21_convert.c s21_other.c s21_util.c

OBJ=$(addprefix obj/,$(SRC:.c=.o))
OBJ_GCOV=$(addprefix gcov/,$(SRC:.c=.o))

.PHONY: all test s21_decimal.a

ifeq ($(shell uname), Linux)
LD+=-pthread -lgcov -lrt -lm -subunit
endif

ifeq ($(shell uname), Darwin)
BREW:=/opt/homebrew
OPT+=-I$(BREW)/include
LD+=-L$(BREW)/lib
endif

all: s21_decimal_gcov.a test gcov_report

re: clean all

test: gcov/s21_decimal_test.o s21_decimal_gcov.a
	$(CC) $^ -o $@ $(LD) $(GCOV)

s21_decimal.a: $(OBJ)
	ar -rcs $@ $(OBJ)

s21_decimal_gcov.a: $(OBJ_GCOV)
	ar -rcs $@ $(OBJ_GCOV)

obj/%.o: %.c
	$(CC) $(OPT) -c $< -o $@

gcov/%.o: %.c
	$(CC) $(OPT) $(GCOV) -c $< -o $@

gcov_report:
	$(CC) $(OPT) $(LD) $(GCOV) --coverage s21_decimal_test.c $(SRC) -o gcovreport \
	`pkg-config --cflags --libs check`; \
	./gcovreport; \
	lcov -t "gcovreport" \
	--exclude s21_decimal_test \
	--exclude math.h \
	-o gcovreport.info -c -d .; \
	genhtml -o coverage gcovreport.info;

clean:
	rm -f obj/*
	rm -f gcov/*
	rm -f *.a
	rm -f *.gcno
	rm -f *.gcda
	rm -f *.info
	rm -f *.gcov
	rm -f test gcovreport
	rm -rf *.dSYM
	rm -rf coverage/*

lint:
	clang-format -i *.c *.h

check:
	clang-format -n *.c *.h

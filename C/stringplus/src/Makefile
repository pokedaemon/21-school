CC=gcc
CFLAGS=-Wall -Wextra -Werror --std=c11

GCOVFLAGS=-fprofile-arcs -ftest-coverage
LDFLAGS=-lcheck -fprofile-arcs --coverage

ifeq ($(shell uname), Linux)
LDFLAGS+=-pthread -lgcov -lrt -lm
endif

ifeq ($(shell uname), Darwin)
HOMEBREW_PREFIX=/opt/homebrew
LDFLAGS+=-I$(HOMEBREW_PREFIX)/include
LDFLAGS+=-L$(HOMEBREW_PREFIX)/lib
endif

SRC=s21_string.c s21_sprintf.c

OBJS=$(addprefix obj/,$(SRC:.c=.o))
OBJS_GCOV=$(addprefix gcov/,$(SRC:.c=.o))

# Sanitizer options
# It's help with warning about deprecated sprintf because security breach of buffer overflow
# CFLAGS+=-fsanitize=address -g -O1 -fno-omit-frame-pointer -Wno-error=deprecated-declarations
# LDFLAGS+=-fsanitize=address
# But we don't give a fuck

.PHONY: re all gcov_report clean

all: s21_string_gcov.a test gcov_report

re: clean all

test: gcov/s21_string_test.o s21_string_gcov.a
	$(CC) $^ -o $@ $(LDFLAGS) $(GCOVFLAGS)

s21_string_gcov.a: $(OBJS_GCOV)
	ar -rcs $@ $(OBJS_GCOV)

s21_string.a: $(OBJS)
	ar -rcs $@ $(OBJS)

obj/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

gcov/%.o: %.c
	$(CC) $(CFLAGS) $(GCOVFLAGS) -c $< -o $@

gcov_report:
	$(CC) $(CFLAGS) --coverage s21_string_test.c $(SRC) -o gcovreport \
	`pkg-config --cflags --libs check`
	./gcovreport
	lcov -t "gcovreport" \
	--exclude s21_string_test \
	--exclude math.h \
	-o gcovreport.info -c -d .
	genhtml -o coverage gcovreport.info

lint:
	clang-format -i *.c *.h

lint-check:
	clang-format -n *.c *.h

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
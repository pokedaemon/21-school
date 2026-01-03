CC=gcc
OPT=-Werror -Wextra -Wall --std=c11
LD=-lcheck -fprofile-arcs --coverage
GCOV=-ftest-coverage

ifeq ($(shell uname), Darwin)
OPT+=-I/opt/homebrew/include 
LD+=-L/opt/homebrew/lib
endif

ifeq ($(shell uname), Linux)
LD+=-pthread -lgcov -lrt -lm -subunit
endif

SRC=s21_matrix.c

OBJ=$(addprefix obj/,$(SRC:.c=.o))
OBJ_GCOV=$(addprefix gcov/,$(SRC:.c=.o))

all: s21_matrix_gcov.a test gcov_report
 
test: gcov/s21_matrix_test.o s21_matrix_gcov.a
	$(CC) $^ -o $@ $(LD) $(GCOV)

s21_matrix.a: $(OBJ)
	ar -rcs $@ $(OBJ)

s21_matrix_gcov.a: $(OBJ_GCOV)
	ar -rcs $@ $(OBJ_GCOV)
 
gcov_report:
	$(CC) $(OPT) $(LD) $(GCOV) --coverage s21_matrix_test.c $(SRC) -o gcovreport \
	`pkg-config --cflags --libs check`; \
	./gcovreport; \
	lcov -t "gcovreport" \
	--exclude s21_matrix_test \
	--exclude math.h \
	-o gcovreport.info -c -d .; \
	genhtml -o coverage gcovreport.info;

obj/%.o: %.c
	$(CC) $(OPT) -c $< -o $@
	
gcov/%.o: %.c
	$(CC) $(OPT) $(GCOV) -c $< -o $@

lint:
	clang-format -i *.c *.h

check:
	clang-format -n *.c *.h
	cppcheck *.c *.h

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
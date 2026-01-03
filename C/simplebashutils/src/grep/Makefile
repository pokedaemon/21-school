CMP=gcc
OPT=-Werror -Wall -Wextra --std=c11

DBG_OPT=$(OPT) -D=GREP_DEBUG_BUILD

s21_grep: grep.c prelude.h
	$(CMP) $(OPT) grep.c -o s21_grep

debug: grep.c prelude.h
	$(CMP) $(DBG_OPT) grep.c -o s21_grep

tests: s21_grep
	./grep_test.sh
CMP=gcc
OPT=-Werror -Wextra -Wall --std=c11

ARGS=args/
OUT=s21_cat

s21_cat: cat.o util.o args.o
	$(CMP) $(OPT) *.o -o $(OUT)
	rm *.o

cat.o: cat.c args/args.h util.h
	$(CMP) $(OPT) cat.c -c

util.o: util.c util.h
	$(CMP) $(OPT) util.c -c

args.o: args/args.c args/args.h
	$(CMP) $(OPT) args/args.c -c

tests:
	./cat_test.sh


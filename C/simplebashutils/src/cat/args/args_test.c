#include "args.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

void print_args(args_t args) {
    printf("Small options: %.*s\n", (int)args.s_sm, args.sm);
    printf("Big options:\n");
    for (size_t i = 0; i < args.s_bg; ++i) printf("  %s\n", args.bg[i]);
    printf("Values:\n");
    for (size_t i = 0; i < args.s_values; ++i) printf("  %s\n", args.values[i]);
}

void test_split_case1() {
    char* argv[] = {"prog", "-abc", "--flag", "val1", "val2"};
    int argc = sizeof(argv) / sizeof(argv[0]);
    args_t args = args_init(argc, argv);

    assert(args.s_sm == 3);
    assert(strncmp(args.sm, "abc", 3) == 0);
    assert(args.s_bg == 1);
    assert(strcmp(args.bg[0], "flag") == 0);
    assert(args.s_values == 2);
    assert(strcmp(args.values[0], "val1") == 0);
    assert(strcmp(args.values[1], "val2") == 0);

    print_args(args);
}

void test_split_case2() {
    char* argv[] = {"prog", "--opt1", "--opt2", "-x", "-yz", "file.txt"};
    int argc = sizeof(argv) / sizeof(argv[0]);
    args_t args = args_init(argc, argv);

    assert(args.s_sm == 3);
    assert(strncmp(args.sm, "xyz", 3) == 0);
    assert(args.s_bg == 2);
    assert(strcmp(args.bg[0], "opt1") == 0);
    assert(strcmp(args.bg[1], "opt2") == 0);
    assert(args.s_values == 1);
    assert(strcmp(args.values[0], "file.txt") == 0);

    print_args(args);
}

int main() {
    test_split_case1();
    test_split_case2();

    printf("All tests passed.\n");
    return 0;
}
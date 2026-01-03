#ifndef ARGS_H
#define ARGS_H

#include <stdlib.h>

typedef struct _args {
    char* sm;
    char** bg;
    char** values;
    size_t s_sm;
    size_t s_bg;
    size_t s_values;
} args_t;

args_t args_init(int argc, char** argv);
void args_free(args_t* self);

#endif
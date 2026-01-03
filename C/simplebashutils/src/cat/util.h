#ifndef CAT_UTIL_H
#define CAT_UTIL_H

#include <stdbool.h>
#include <stdlib.h>

#include "args/args.h"

#define CAT_VERSION 0.1

enum cat_option {
    CAT_NUMBER_NON_BLANK,
    CAT_DISPLAY_END_OF_LINE,  // must have -v
    CAT_NUM_ALL_LINES,
    CAT_SQUEEZE_BLANK,
    CAT_DISPLAY_TABS,  // must have -v
    CAT_DISPLAY_NON_VISIBLE
};

void usage_page(void);
void bad_usage_page(void);
void version_page(void);

typedef struct _cargs {
    bool flags[8];
    char** filenames;
    size_t filenames_len;
} cat_args_t;

cat_args_t cat_from_args(args_t* args);

void cat(cat_args_t args);

#endif  // CAT_UTIL_H
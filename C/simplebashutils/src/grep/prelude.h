#ifndef GREP_PRELUDE_H
#define GREP_PRELUDE_H

#include <assert.h>
#include <ctype.h>   // for tolower()
#include <getopt.h>  // for getopt() btw
#include <regex.h>   // regexec(), regcomp()
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct grep_status {
    size_t lines;
    size_t files;
} grp_stat_t;

typedef struct grep_patterns {
    char** patterns;
    size_t length;
} grp_pat_t;

static const char* GREP_OPT_PATTERN = "e:ivclnhsf:o";

static bool insensitive = false;
static bool invert = false;
static bool olc = false;      // only lines count (for -c)
static bool ofc = false;      // only files count (for -l)
static bool with_ln = false;  // with matching line number (for -n)

// part 2
static bool no_print_fn = false;  // no print filenames with finded pattern
static bool ignore_msgs = false;  // ignore message about bad opening files
static bool only_match = false;   // print only matching strs

void regex_file(grp_stat_t* stat, regex_t* __restrict rxs, size_t rxs_len, char* __restrict filename,
                bool many_files);

void regex_file_matching(grp_stat_t* stat, regex_t* __restrict rxs, size_t rxs_len, char* __restrict filename,
                         bool many_files);

void grep_usage(void);
grp_pat_t process_option(int argc, char** argv);

#endif
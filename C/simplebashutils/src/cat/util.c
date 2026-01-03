#include "util.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "args/args.h"

cat_args_t cat_from_args(args_t* args) {
    cat_args_t cargs = {0};

    // small options
    for (size_t i = 0; i < args->s_sm; ++i) {
        switch (args->sm[i]) {
            case 'b':
                cargs.flags[CAT_NUMBER_NON_BLANK] = true;
                break;
            case 'e':
                cargs.flags[CAT_DISPLAY_END_OF_LINE] = true;
                break;
            case 'n':
                cargs.flags[CAT_NUM_ALL_LINES] = true;
                break;
            case 's':
                cargs.flags[CAT_SQUEEZE_BLANK] = true;
                break;
            case 't':
                cargs.flags[CAT_DISPLAY_TABS] = true;
                break;
            case 'E':
                cargs.flags[CAT_DISPLAY_END_OF_LINE] = true;
                cargs.flags[CAT_DISPLAY_NON_VISIBLE] = true;
                break;
            case 'T':
                cargs.flags[CAT_DISPLAY_TABS] = true;
                cargs.flags[CAT_DISPLAY_NON_VISIBLE] = true;
                break;
            case 'v':
                cargs.flags[CAT_DISPLAY_NON_VISIBLE] = true;
                break;
            default:
                bad_usage_page();
                exit(EXIT_FAILURE);
                break;
        }
    }

    // big options
    for (size_t i = 0; i < args->s_bg; ++i) {
        if (!strcmp(args->bg[i], "number"))
            cargs.flags[CAT_NUM_ALL_LINES] = true;
        else if (!strcmp(args->bg[i], "number-nonblank"))
            cargs.flags[CAT_NUMBER_NON_BLANK] = true;
        else if (!strcmp(args->bg[i], "squeeze-blank"))
            cargs.flags[CAT_SQUEEZE_BLANK] = true;
        else {
            bad_usage_page();
            args_free(args);
            exit(EXIT_FAILURE);
        }
    }

    if (cargs.flags[CAT_NUM_ALL_LINES] && cargs.flags[CAT_NUMBER_NON_BLANK])
        cargs.flags[CAT_NUM_ALL_LINES] = false;

    if (!cargs.flags[CAT_DISPLAY_NON_VISIBLE]) {
        cargs.flags[CAT_DISPLAY_END_OF_LINE] = false;
        cargs.flags[CAT_DISPLAY_TABS] = false;
    }

    cargs.filenames = args->values;
    cargs.filenames_len = args->s_values;

    return cargs;
}

inline void usage_page(void) {
    printf("Usage: s21_cat [-beEnstTv] [--number|number-nonblank|squeeze-blank] [file ...]\n");
}

inline void bad_usage_page(void) {
    printf("Bad option.\n");
    usage_page();
}

inline void version_page(void) { printf("s21_cat version %.1f\nSchool 21.\n", CAT_VERSION); }

size_t cat_decorate_out(char* line, bool flags[8], size_t line_number, bool* prev_was_blank) {
    char temp[BUFSIZ * 2] = {0};
    size_t len = strlen(line);

    bool is_blank = (len == 1 && line[0] == '\n');

    // Сжатие пустых строк (-s)
    if (flags[CAT_SQUEEZE_BLANK]) {
        if (is_blank) {
            if (*prev_was_blank)
                return line_number;  // пропускаем эту строку
            else
                *prev_was_blank = true;
        } else {
            *prev_was_blank = false;
        }
    }

    // Нумерация строк
    bool number_line = false;
    if (flags[CAT_NUMBER_NON_BLANK]) {
        if (!is_blank) number_line = true;
    } else if (flags[CAT_NUM_ALL_LINES]) {
        number_line = true;
    }

    // Показывать табуляцию (-T)
    if (flags[CAT_DISPLAY_TABS]) {
        char buf[BUFSIZ * 2] = {0};
        size_t pos = 0;
        for (size_t i = 0; i < strlen(line); ++i) {
            if (line[i] == '\t') {
                buf[pos++] = '^';
                buf[pos++] = 'I';
            } else {
                buf[pos++] = line[i];
            }
        }
        buf[pos] = '\0';
        strcpy(line, buf);
    }

    if (number_line) {
        sprintf(temp, "%6zu\t%s", line_number, line);
        strcpy(line, temp);
        line_number++;
    }

    // Показывать символ конца строки (-E)
    if (flags[CAT_DISPLAY_END_OF_LINE]) {
        len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '$';
            line[len] = '\n';
            line[len + 1] = '\0';
        }
    }

    return line_number;
}

void cat(cat_args_t args) {
    char buffer[BUFSIZ];
    bool prev_was_blank = false;

    if (args.filenames_len == 0) {
        // вывод из stdin
        size_t lines = 1;
        while (fgets(buffer, sizeof(buffer), stdin)) {
            lines = cat_decorate_out(buffer, args.flags, lines, &prev_was_blank);
            if (buffer[0] != '\0') fputs(buffer, stdout);
        }
    } else {
        for (size_t i = 0; i < args.filenames_len; ++i) {
            FILE* file = fopen(args.filenames[i], "r");
            if (!file) {
                fprintf(stderr, "s21_cat: %s: No such file or directory\n", args.filenames[i]);
                continue;
            }
            size_t lines = 1;
            prev_was_blank = false;
            while (fgets(buffer, sizeof(buffer), file) != NULL) {
                lines = cat_decorate_out(buffer, args.flags, lines, &prev_was_blank);
                if (buffer[0] != '\0') fputs(buffer, stdout);
            }
            fclose(file);
        }
    }
}
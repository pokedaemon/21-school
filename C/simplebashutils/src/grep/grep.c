#include "prelude.h"

// TODO: impl -f option

/**
 * Task: develop a grep utility
 * Part 1:
 * - support flags: -e -i -v -c -l -n
 * - only regex or pcre headers
 * Part 2:
 * - support more flags: -h -s -f -o
 * - only regex or pcre headers
 * Part 3:
 * - support all flags, included a pairs: -iv -in
 * - only regex or pcre headers
 */

int main(int argc, char** argv) {
    grp_pat_t pats = process_option(argc, argv);

    char** patterns = pats.patterns;
    size_t pats_len = pats.length;

    if (!pats_len) {
        patterns[pats_len++] = argv[optind++];
#ifdef GREP_DEBUG_BUILD
        fprintf(stdout, "pat = \"%s\"\n", patterns[pats_len - 1]);
#endif
    }

    regex_t* rxs = malloc(sizeof(regex_t) * pats_len);
    size_t rxs_len = pats_len;

    if (!rxs) {
        free(patterns);
        fprintf(stderr, "bad allocation\n");
        return EXIT_FAILURE;
    }

    for (size_t i = 0; i < pats_len; ++i) {
        int ret;
        if ((ret = regcomp(&rxs[i], patterns[i], REG_EXTENDED | (insensitive ? REG_ICASE : 0)))) {
            char err_buffer[BUFSIZ / 4];
            regerror(ret, &rxs[i], err_buffer, sizeof(err_buffer));
            fprintf(stderr, "regular expression compilation failed: %s\n", err_buffer);

            for (size_t j = 0; j < i; ++j) regfree(&rxs[j]);  // delete prev regexpr

            free(rxs);
            free(patterns);
            return EXIT_FAILURE;
        }
    }

    grp_stat_t stat = {.files = 0, .lines = 0};
    bool many_files = argc - optind > 1;

    if (only_match) {
        if (optind == argc) {
            regex_file_matching(&stat, rxs, rxs_len, NULL, many_files);
        } else {
            for (size_t i = optind; i < (size_t)argc; ++i) {
                size_t lines_before = stat.lines;
                regex_file_matching(&stat, rxs, rxs_len, argv[i], many_files);
                if (olc && many_files) fprintf(stdout, "%s:%zu\n", argv[i], stat.lines - lines_before);
                if (ofc && many_files) fprintf(stdout, "%s\n", argv[i]);
            }
        }
    } else {
        if (optind == argc) {
            regex_file(&stat, rxs, rxs_len, NULL, many_files);
        } else {
            for (size_t i = optind; i < (size_t)argc; ++i) {
                size_t lines_before = stat.lines;
                regex_file(&stat, rxs, rxs_len, argv[i], many_files);
                if (olc && many_files) fprintf(stdout, "%s:%zu\n", argv[i], stat.lines - lines_before);
                if (ofc && many_files) fprintf(stdout, "%s\n", argv[i]);
            }
        }
    }

    if (ofc && !many_files) {
        printf("%zu\n", stat.files);
        if (optind == argc) printf("(standart input)\n");
    } else if (olc && !many_files)
        fprintf(stdout, "%zu\n", stat.lines);

    for (size_t i = 0; i < rxs_len; ++i) {
        regfree(&rxs[i]);
    }
    free(rxs);
    free(patterns);

    return EXIT_SUCCESS;
}

void regex_file_matching(grp_stat_t* stat, regex_t* __restrict rxs, size_t rxs_len, char* __restrict filename,
                         bool many_files) {
    (void)(stat);
    FILE* file = NULL;
    if (!filename)
        file = stdin;
    else
        file = fopen(filename, "r");
    if (!file && !ignore_msgs) {
        fprintf(stderr, "%s: can't open file\n", filename);
        return;
    }
    char buffer[BUFSIZ];
    size_t lines = 0;
    while (fgets(buffer, BUFSIZ, file) != NULL) {
        ++lines;
        regmatch_t pmatch;
        size_t len = strlen(buffer);

        for (size_t i = 0; i < rxs_len; ++i) {
            int start = 0;
            while (start <= (int)len && regexec(&rxs[i], buffer + start, 1, &pmatch, 0) == 0) {
                int match_start = start + pmatch.rm_so;
                int match_end = start + pmatch.rm_eo;

                if (many_files && !no_print_fn) printf("%s:", filename);
                if (with_ln) printf("%zu:", lines);

                printf("%.*s\n", match_end - match_start, buffer + match_start);
                if (pmatch.rm_eo == 0) break;
                start = match_end;
            }
        }
    }
    if (file != stdin) fclose(file);
}

void regex_file(grp_stat_t* stat, regex_t* __restrict rxs, size_t rxs_len, char* __restrict filename,
                bool many_files) {
    size_t lines_count = 0;

    FILE* file = NULL;
    if (!filename)
        file = stdin;
    else
        file = fopen(filename, "r");

    if (!file && !ignore_msgs) {
        fprintf(stderr, "%s: can't open file\n", filename);
        return;
    }

    char buffer[BUFSIZ] = {0};
    size_t lines = 0;
    while (fgets(buffer, BUFSIZ, file) != NULL) {
        bool result = false;
        ++lines;
        for (size_t i = 0; i < rxs_len; ++i) {
            result |= regexec(&rxs[i], buffer, 0, NULL, 0) == 0;
        }
        if (invert) result = !result;
        if (result) ++lines_count;

        if (result) {
            if (!(ofc || olc)) {
                if (!no_print_fn && many_files) printf("%s:", filename);
                if (with_ln) {
                    printf("%zu:%s", lines, buffer);
                } else {
                    printf("%s", buffer);
                }
            }
        }
    }

    if (file != stdin) fclose(file);

    stat->lines += lines_count;
    stat->files += lines_count > 0 ? 1 : 0;
}

void grep_usage(void) {
    fprintf(stdout,
            "s21_grep - file pattern searcher of school 21\n"
            "Usage: s21_grep [-eivclnhsfo] [pattern] [file ...]\n"
            "Options:\n"
            "\t-e PATTERN\tSpecify pattern\n"
            "\t-i\t\tCase insensitive search\n"
            "\t-v\t\tInvert match\n"
            "\t-c\t\tCount matching lines\n"
            "\t-l\t\tPrint only names of files with matches\n"
            "\t-n\t\tPrint line numbers\n"
            "\t-h\t\tSuppress filename prefix\n"
            "\t-s\t\tSuppress error messages\n"
            "\t-f FILE\t\tObtain patterns from FILE (not implemented yet)\n"
            "\t-o\t\tPrint only matched parts of lines\n");
}

grp_pat_t process_option(int argc, char** argv) {
    char** patterns = malloc(sizeof(char*) * (argc - 1));
    size_t pats_len = 0;

    int ch;
    while ((ch = getopt(argc, argv, GREP_OPT_PATTERN)) != -1) {
        switch (ch) {
            case 'e':
                patterns[pats_len] = optarg;
                ++pats_len;
                break;
            case 'i':
                insensitive = true;
                break;
            case 'v':
                invert = true;
                break;
            case 'c':
                olc = true;
                break;
            case 'l':
                ofc = true;
                break;
            case 'n':
                with_ln = true;
                break;
            case 'h':
                no_print_fn = true;
                break;
            case 's':
                ignore_msgs = true;
                break;
            case 'f':
                fprintf(stderr, "option -f is not implemented yet");
                break;
            case 'o':
                only_match = true;
                break;
            case ':':
                fprintf(stderr, "option -%c require argument\n", optopt);
                break;
            case '?':
            default:
                grep_usage();
                free(patterns);
                exit(EXIT_FAILURE);
                break;
        }

#ifdef GREP_DEBUG_BUILD
        printf("option_get: %c\n", ch);
#endif
    }

    return (grp_pat_t){.patterns = patterns, .length = pats_len};
}
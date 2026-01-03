#include <stdio.h>
#include <string.h>

#include "args/args.h"
#include "util.h"

/**
 * Task: develop a cat utility
 * 1. include all flags as:
 * 1.1. -b --number-nonblank -> numerate only non empty lines
 * 1.2. -e(must have -v) -E -> display end-of-line as $
 * 1.3. -n --number -> numerate all lines
 * 1.4. -s --squeeze-blank -> squeeze all MULTIPLE adjacent empty lines
 * 1.5. -t(must have -v) -T -> display tabs as ^I
 *
 * 2. All sources, headers, and build files must included in src/cat/
 * 3. Execution file also must included in src/cat/ as s21_cat
 */

int main(int argc, char** argv) {
    if (argc == 2) {
        if (!strcmp(argv[1], "--version")) {
            version_page();
            return EXIT_SUCCESS;
        }
        if (!strcmp(argv[1], "--help")) {
            usage_page();
            return EXIT_SUCCESS;
        }
    }

    args_t args = args_init(argc, argv);

#ifdef CAT_DEBUG
    printf("s_bg = %zu\n", args.s_bg);
    printf("s_sm = %zu\n", args.s_sm);
    printf("s_values = %zu\n", args.s_values);
#endif

    cat_args_t cargs = cat_from_args(&args);

    cat(cargs);
    args_free(&args);  // after free don't use!

    return EXIT_SUCCESS;
}

#include "args.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define OPTION_SIZE 100
#define VALUES_SIZE OPTION_SIZE

args_t args_init(int argc, char** argv) {
    size_t small_c = 0, big_c = 0, values_c = 0;

    for (size_t i = 1; i < (size_t)argc; ++i) {
        if (argv[i][0] == '-' && argv[i][1] == '-')
            big_c++;
        else if (argv[i][0] == '-') {
            size_t len = strlen(argv[i]);
            small_c += len - 1;
        } else {
            values_c += (argc - i);
            break;
        }
    }

    /**
     * Block where we create ptrs and initialize mem in heap
     */
    // sm - small options ptr
    char* sm = malloc(sizeof(char) * small_c + 1);
    assert(sm);
    // bg - big options ptr
    char** bg = malloc(big_c * sizeof(char*) + big_c * OPTION_SIZE * sizeof(char));
    assert(bg);
    // vl - values btw
    char** vl = NULL;
    if (values_c > 0) {
        vl = malloc(values_c * sizeof(char*) + values_c * VALUES_SIZE * sizeof(char));
        assert(vl);
    }

    // YES, DRY is not for me
    char* ptr = (char*)(bg + big_c);
    for (size_t i = 0; i < big_c; ++i) {
        bg[i] = ptr + OPTION_SIZE * i;
    }
    ptr = (char*)(vl + values_c);
    for (size_t i = 0; i < values_c; ++i) {
        vl[i] = ptr + VALUES_SIZE * i;
    }
    // ------------------------------------------------------

    size_t start_values = 1, i_sml = 0, i_big = 0;
    // This is filling of ptrs
    for (size_t i = 1; i < (size_t)argc; ++i) {
        if (argv[i][0] == '-' && argv[i][1] == '-') {
            // big options
            strncpy(bg[i_big], argv[i] + 2, OPTION_SIZE - 1);
            bg[i_big][OPTION_SIZE - 1] = '\0';
            i_big++;
        } else if (argv[i][0] == '-') {
            // small options
            size_t len = strlen(argv[i]);
            for (size_t j = 1; j < len; ++j) sm[i_sml++] = argv[i][j];
        } else {
            start_values = i;
            break;
        }
    }

    size_t i_vls = 0;
    if (values_c != 0)
        for (size_t i = start_values; i < (size_t)argc; ++i) {
            // values
            strncpy(vl[i_vls], argv[i], VALUES_SIZE - 1);
            vl[i_vls][VALUES_SIZE - 1] = '\0';
            i_vls++;
        }

    sm[small_c] = '\0';

    // And returning
    return (args_t){
        .sm = sm,             // char*
        .bg = bg,             // char**
        .values = vl,         // char**
        .s_sm = small_c,      // size_t
        .s_bg = big_c,        // size_t
        .s_values = values_c  // size_t
    };
}

void args_free(args_t* self) {  // just free our 3 arrays
    free(self->bg);
    free(self->sm);
    free(self->values);
}
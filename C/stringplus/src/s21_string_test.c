#include "s21_string.h"

#include <check.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

SRunner *init_runner(const char *suite_name, const char *tcase_name,
                     const size_t count_tests, ...) {
    va_list tests;
    va_start(tests, count_tests);

    Suite *su = suite_create(suite_name);
    TCase *tc = tcase_create(tcase_name);

    for (size_t i = 0; i < count_tests; ++i) {
        TTest *test = va_arg(tests, TTest *);
        tcase_add_test(tc, test);
    }

    va_end(tests);

    suite_add_tcase(su, tc);

    return srunner_create(su);
}

START_TEST(test_insert_middle) {
    char *res = s21_insert("Hello", "X", 2);
    ck_assert_str_eq(res, "HeXllo");
    free(res);
}
END_TEST

START_TEST(test_insert_begin) {
    char *res = s21_insert("World", "Hello ", 0);
    ck_assert_str_eq(res, "Hello World");
    free(res);
}
END_TEST

START_TEST(test_insert_end) {
    char *res = s21_insert("Hello", " World", 5);
    ck_assert_str_eq(res, "Hello World");
    free(res);
}
END_TEST

START_TEST(test_insert_empty_src) {
    char *res = s21_insert("", "abc", 0);
    ck_assert_str_eq(res, "abc");
    free(res);
}
END_TEST

START_TEST(test_insert_empty_str) {
    char *res = s21_insert("abc", "", 1);
    ck_assert_str_eq(res, "abc");
    free(res);
}
END_TEST

START_TEST(test_insert_index_out_of_bounds) {
    char *res = s21_insert("abc", "X", 5);
    ck_assert_ptr_eq(res, S21_NULL);
}
END_TEST

START_TEST(test_insert_null_src) {
    char *res = s21_insert(S21_NULL, "abc", 0);
    ck_assert_ptr_eq(res, S21_NULL);
}
END_TEST

START_TEST(test_insert_null_str) {
    char *res = s21_insert("abc", S21_NULL, 0);
    ck_assert_ptr_eq(res, S21_NULL);
}
END_TEST

int insert_tests(void) {
    SRunner *runner = init_runner(
        "s21_insert_suite", "s21_insert_case", 8, test_insert_middle,
        test_insert_begin, test_insert_end, test_insert_empty_src,
        test_insert_empty_str, test_insert_index_out_of_bounds,
        test_insert_null_src, test_insert_null_str);

    srunner_run_all(runner, CK_VERBOSE);
    int failed = srunner_ntests_failed(runner);
    srunner_free(runner);
    return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}

void assert_memchr_eq(const void *str, int c, s21_size_t n) {
    void *s21_result = s21_memchr(str, c, n);
    void *std_result = memchr(str, c, n);

    if (s21_result == S21_NULL && std_result == NULL) return;

    if (s21_result != S21_NULL && std_result != NULL) {
        ck_assert_ptr_eq(s21_result, std_result);
        return;
    }

    ck_assert_msg(0, "s21_memchr and memchr differ: s21=%p, std=%p", s21_result,
                  std_result);
}

START_TEST(test_memchr_basic) {
    char str[] = "Hello, world!";

    assert_memchr_eq(str, 'H', 13);
    assert_memchr_eq(str, 'o', 13);
    assert_memchr_eq(str, '!', 13);
    assert_memchr_eq(str, 'z', 13);
    char buf[20] = "test";
    buf[4] = 'x';
    assert_memchr_eq(buf, '\0', 20);
}
END_TEST

START_TEST(test_memchr_edge_cases) {
    char str[] = "abc";

    ck_assert_ptr_eq(s21_memchr(str, 'a', 0), S21_NULL);
    ck_assert_ptr_eq(s21_memchr(NULL, 'a', 0), S21_NULL);

    assert_memchr_eq(str, '\0', 4);

    char empty[] = "";
    assert_memchr_eq(empty, '\0', 1);
    assert_memchr_eq(empty, 'a', 1);
}
END_TEST

START_TEST(test_memchr_non_string_data) {
    unsigned char data[] = {1, 2, 3, 4, 5, 6, 7, 8};
    s21_size_t n = sizeof(data);

    void *res = s21_memchr(data, 5, n);
    ck_assert_ptr_eq(res, &data[4]);

    ck_assert_ptr_eq(s21_memchr(data, 10, n), S21_NULL);
}
END_TEST

int memchr_tests(void) {
    SRunner *rnr =
        init_runner("memchr", "tests", 3, test_memchr_basic,
                    test_memchr_edge_cases, test_memchr_non_string_data);

    srunner_run_all(rnr, CK_VERBOSE);
    int failed = srunner_ntests_failed(rnr);
    srunner_free(rnr);

    return failed > 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}

void assert_memcmp_eq(const void *s1, const void *s2, s21_size_t n) {
    int s21_res = s21_memcmp(s1, s2, n);
    int std_res = memcmp(s1, s2, n);

    if (s21_res == 0 && std_res == 0) return;
    if (s21_res < 0 && std_res < 0) return;
    if (s21_res > 0 && std_res > 0) return;

    ck_assert_msg(0,
                  "s21_memcmp and memcmp differ:\n"
                  "  s21 = %d, std = %d\n"
                  "  n = %zu",
                  s21_res, std_res, (size_t)n);
}

START_TEST(test_memcmp_equal) {
    char a[] = "hello";
    char b[] = "hello";
    ck_assert_int_eq(s21_memcmp(a, b, 5), 0);
    assert_memcmp_eq(a, b, 5);
}
END_TEST

START_TEST(test_memcmp_different) {
    char a[] = "hello";
    char b[] = "hallo";

    int res = s21_memcmp(a, b, 5);
    ck_assert_int_gt(res, 0);
    assert_memcmp_eq(a, b, 5);

    res = s21_memcmp(b, a, 5);
    ck_assert_int_lt(res, 0);
    assert_memcmp_eq(b, a, 5);
}
END_TEST

START_TEST(test_memcmp_n_zero) {
    char a[] = "hello";
    char b[] = "world";
    ck_assert_int_eq(s21_memcmp(a, b, 0), 0);
    assert_memcmp_eq(a, b, 0);
}
END_TEST

START_TEST(test_memcmp_partial_equal) {
    char a[] = "helloX";
    char b[] = "helloY";

    ck_assert_int_eq(s21_memcmp(a, b, 5), 0);
    assert_memcmp_eq(a, b, 5);

    ck_assert_int_ne(s21_memcmp(a, b, 6), 0);
    assert_memcmp_eq(a, b, 6);
}
END_TEST

START_TEST(test_memcmp_non_string_data) {
    unsigned char data1[] = {0, 1, 2, 3, 255};
    unsigned char data2[] = {0, 1, 2, 3, 0};

    int res = s21_memcmp(data1, data2, 5);
    ck_assert_int_gt(res, 0);
    assert_memcmp_eq(data1, data2, 5);

    ck_assert_int_eq(s21_memcmp(data1, data2, 4), 0);
}
END_TEST

START_TEST(test_memcmp_first_byte_diff) {
    char a[] = "\xFF";
    char b[] = "\x00";

    int res = s21_memcmp(a, b, 1);
    ck_assert_int_gt(res, 0);
    assert_memcmp_eq(a, b, 1);
}
END_TEST

START_TEST(test_memcmp_signed_vs_unsigned) {
    signed char s1 = -1;
    signed char s2 = 1;

    int res = s21_memcmp(&s1, &s2, 1);
    ck_assert_int_gt(res, 0);
    assert_memcmp_eq(&s1, &s2, 1);
}
END_TEST

int memcmp_tests(void) {
    SRunner *rnr = init_runner(
        "memcmp", "tests", 7, test_memcmp_equal, test_memcmp_different,
        test_memcmp_n_zero, test_memcmp_partial_equal,
        test_memcmp_non_string_data, test_memcmp_first_byte_diff,
        test_memcmp_signed_vs_unsigned);

    srunner_run_all(rnr, CK_VERBOSE);
    int failed = srunner_ntests_failed(rnr);
    return failed > 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}

void assert_memcpy_eq(const void *src, s21_size_t n) {
    char *dest1 = malloc(n);
    char *dest2 = malloc(n);
    ck_assert_ptr_ne(dest1, NULL);
    ck_assert_ptr_ne(dest2, NULL);

    void *res1 = s21_memcpy(dest1, src, n);
    void *res2 = memcpy(dest2, src, n);

    ck_assert_ptr_eq(res1, dest1);
    ck_assert_ptr_eq(res2, dest2);

    ck_assert_int_eq(memcmp(dest1, dest2, n), 0);

    free(dest1);
    free(dest2);
}

START_TEST(test_memcpy_basic_string) {
    const char *src = "Hello, world!";
    s21_size_t n = strlen(src) + 1;

    char dest[32] = {0};
    void *res = s21_memcpy(dest, src, n);
    ck_assert_ptr_eq(res, dest);
    ck_assert_str_eq(dest, src);
}
END_TEST

START_TEST(test_memcpy_n_zero) {
    char dest[10] = "garbage";
    const char *src = "anything";

    void *res = s21_memcpy(dest, src, 0);
    ck_assert_ptr_eq(res, dest);
    ck_assert_str_eq(dest, "garbage");
}
END_TEST

START_TEST(test_memcpy_binary_data) {
    unsigned char src[] = {0, 1, 2, 127, 128, 255, 0, 42};
    s21_size_t n = sizeof(src);

    unsigned char dest[sizeof(src)] = {0};

    s21_memcpy(dest, src, n);
    ck_assert_int_eq(memcmp(dest, src, n), 0);
}
END_TEST

START_TEST(test_memcpy_large_data) {
    const s21_size_t n = 1024;
    unsigned char *src = malloc(n);
    unsigned char *dest = malloc(n);
    ck_assert_ptr_ne(src, NULL);
    ck_assert_ptr_ne(dest, NULL);

    for (s21_size_t i = 0; i < n; ++i) {
        src[i] = (unsigned char)(i % 256);
    }

    s21_memcpy(dest, src, n);
    ck_assert_int_eq(memcmp(dest, src, n), 0);

    free(src);
    free(dest);
}
END_TEST

START_TEST(test_memcpy_return_value) {
    char dest[10];
    const char *src = "test";
    void *res = s21_memcpy(dest, src, 5);
    ck_assert_ptr_eq(res, dest);
}
END_TEST

START_TEST(test_memcpy_with_null_n_zero) {
    void *res = s21_memcpy(NULL, NULL, 0);
    ck_assert_ptr_eq(res, NULL);
}
END_TEST

START_TEST(test_memcpy_single_byte) {
    char src = 0xFF;
    char dest = 0x00;
    s21_memcpy(&dest, &src, 1);
    ck_assert_int_eq(dest, 0xFF);
}
END_TEST

int memcpy_tests(void) {
    SRunner *rnr =
        init_runner("memcpy", "tests", 7, test_memcpy_basic_string,
                    test_memcpy_n_zero, test_memcpy_binary_data,
                    test_memcpy_large_data, test_memcpy_return_value,
                    test_memcpy_with_null_n_zero, test_memcpy_single_byte);

    srunner_run_all(rnr, CK_VERBOSE);
    int failed = srunner_ntests_failed(rnr);
    srunner_free(rnr);

    return failed > 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}

void assert_memset_eq(void *buf, int c, s21_size_t n) {
    (void)(buf);
    char *buf1 = malloc(n);
    char *buf2 = malloc(n);
    ck_assert_ptr_ne(buf1, NULL);
    ck_assert_ptr_ne(buf2, NULL);

    void *res1 = s21_memset(buf1, c, n);
    void *res2 = memset(buf2, c, n);

    ck_assert_ptr_eq(res1, buf1);
    ck_assert_ptr_eq(res2, buf2);

    ck_assert_int_eq(memcmp(buf1, buf2, n), 0);

    free(buf1);
    free(buf2);
}

START_TEST(test_memset_zero) {
    char buf[10] = "garbage";
    void *res = s21_memset(buf, 0, 10);
    ck_assert_ptr_eq(res, buf);
    for (int i = 0; i < 10; ++i) {
        ck_assert_int_eq(buf[i], 0);
    }
}
END_TEST

START_TEST(test_memset_value) {
    char buf[5];
    s21_memset(buf, 42, 5);
    for (int i = 0; i < 5; ++i) {
        ck_assert_int_eq(buf[i], 42);
    }
}
END_TEST

START_TEST(test_memset_n_zero) {
    char buf[10] = "unchanged";
    void *res = s21_memset(buf, 99, 0);
    ck_assert_ptr_eq(res, buf);
    ck_assert_str_eq(buf, "unchanged");
}
END_TEST

START_TEST(test_memset_negative_char) {
    char buf[3];
    s21_memset(buf, -1, 3);
    for (int i = 0; i < 3; ++i) {
        ck_assert_int_eq((unsigned char)buf[i], 255);
    }
    assert_memset_eq(buf, -1, 3);
}
END_TEST

START_TEST(test_memset_large_buffer) {
    const s21_size_t n = 1024;
    char *buf = malloc(n);
    ck_assert_ptr_ne(buf, NULL);

    s21_memset(buf, 0xAB, n);
    for (s21_size_t i = 0; i < n; ++i) {
        ck_assert_int_eq((unsigned char)buf[i], 0xAB);
    }

    free(buf);
}
END_TEST

START_TEST(test_memset_return_value) {
    char buf[1];
    void *res = s21_memset(buf, 100, 1);
    ck_assert_ptr_eq(res, buf);
}
END_TEST

START_TEST(test_memset_with_null_n_zero) {
    void *res = s21_memset(NULL, 42, 0);
    ck_assert_ptr_eq(res, NULL);
}
END_TEST

int memset_tests(void) {
    SRunner *rnr = init_runner(
        "memset", "tests", 7, test_memset_zero, test_memset_value,
        test_memset_n_zero, test_memset_negative_char, test_memset_large_buffer,
        test_memset_return_value, test_memset_with_null_n_zero);

    srunner_run_all(rnr, CK_VERBOSE);
    int failed = srunner_ntests_failed(rnr);
    srunner_free(rnr);

    return failed > 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}

START_TEST(test_size_type) {
    ck_assert_uint_eq(sizeof(size_t), sizeof(s21_size_t));
}
END_TEST

START_TEST(test_null_macro) { ck_assert_ptr_eq(NULL, S21_NULL); }
END_TEST

int prelude_tests(void) {
    SRunner *sr =
        init_runner("prelude", "tests", 2, test_size_type, test_null_macro);

    srunner_run_all(sr, CK_VERBOSE);
    int failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}

START_TEST(test_sprintf_char) {
    char buf1[100], buf2[100];
    int ret1 = s21_sprintf(buf1, "Char: %c", 'A');
    int ret2 = sprintf(buf2, "Char: %c", 'A');
    ck_assert_str_eq(buf1, buf2);
    ck_assert_int_eq(ret1, ret2);
}
END_TEST

START_TEST(test_sprintf_string) {
    char buf1[100], buf2[100];
    int ret1 = s21_sprintf(buf1, "Str: %s", "hello");
    int ret2 = sprintf(buf2, "Str: %s", "hello");
    ck_assert_str_eq(buf1, buf2);
    ck_assert_int_eq(ret1, ret2);
}
END_TEST

START_TEST(test_sprintf_decimal) {
    char buf1[100], buf2[100];
    int ret1 = s21_sprintf(buf1, "Num: %d", -42);
    int ret2 = sprintf(buf2, "Num: %d", -42);
    ck_assert_str_eq(buf1, buf2);
    ck_assert_int_eq(ret1, ret2);
}
END_TEST

START_TEST(test_sprintf_unsigned) {
    char buf1[100], buf2[100];
    int ret1 = s21_sprintf(buf1, "Unsigned: %u", 12345U);
    int ret2 = sprintf(buf2, "Unsigned: %u", 12345U);
    ck_assert_str_eq(buf1, buf2);
    ck_assert_int_eq(ret1, ret2);
}
END_TEST

START_TEST(test_sprintf_float) {
    char buf1[100], buf2[100];
    int ret1 = s21_sprintf(buf1, "Float: %f", 3.14159);
    int ret2 = sprintf(buf2, "Float: %f", 3.14159);
    ck_assert_str_eq(buf1, buf2);
    ck_assert_int_eq(ret1, ret2);
}
END_TEST

START_TEST(test_sprintf_percent) {
    char buf1[100], buf2[100];
    int ret1 = s21_sprintf(buf1, "Percent: %%");
    int ret2 = sprintf(buf2, "Percent: %%");
    ck_assert_str_eq(buf1, buf2);
    ck_assert_int_eq(ret1, ret2);
}
END_TEST

START_TEST(test_sprintf_flags) {
    char buf1[100], buf2[100];
    int ret1 = s21_sprintf(buf1, "%+d % d %-5d", 42, 42, 42);
    int ret2 = sprintf(buf2, "%+d % d %-5d", 42, 42, 42);
    ck_assert_str_eq(buf1, buf2);
    ck_assert_int_eq(ret1, ret2);
}
END_TEST

START_TEST(test_sprintf_width_precision) {
    char buf1[100], buf2[100];
    int ret1 = s21_sprintf(buf1, "%10d %.5d %10.3f", 42, 42, 3.14);
    int ret2 = sprintf(buf2, "%10d %.5d %10.3f", 42, 42, 3.14);
    ck_assert_str_eq(buf1, buf2);
    ck_assert_int_eq(ret1, ret2);
}
END_TEST

START_TEST(test_sprintf_length_hl) {
    char buf1[100], buf2[100];
    short sh = 123;
    long lg = 123456789L;
    int ret1 = s21_sprintf(buf1, "%hd %ld", sh, lg);
    int ret2 = sprintf(buf2, "%hd %ld", sh, lg);
    ck_assert_str_eq(buf1, buf2);
    ck_assert_int_eq(ret1, ret2);
}
END_TEST

int sprintf_tests(void) {
    SRunner *runner = init_runner(
        "s21_sprintf", "s21_sprintf", 9, test_sprintf_char, test_sprintf_string,
        test_sprintf_decimal, test_sprintf_unsigned, test_sprintf_float,
        test_sprintf_percent, test_sprintf_flags, test_sprintf_width_precision,
        test_sprintf_length_hl);
    srunner_run_all(runner, CK_VERBOSE);
    int failed = srunner_ntests_failed(runner);
    srunner_free(runner);
    return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}

void assert_strchr_eq(const char *str, int c) {
    char *s21_res = s21_strchr(str, c);
    char *std_res = strchr(str, c);

    if (s21_res == S21_NULL && std_res == NULL) return;
    if (s21_res != S21_NULL && std_res != NULL) {
        ck_assert_ptr_eq(s21_res, std_res);
        return;
    }
    ck_assert_msg(0,
                  "s21_strchr and strchr differ:\n"
                  "  str = \"%s\", c = %d (0x%02X)\n"
                  "  s21 = %p, std = %p",
                  str ? str : "(null)", c, (unsigned char)c, s21_res, std_res);
}

START_TEST(test_strchr_found) {
    const char *str = "Hello, world!";

    ck_assert_ptr_eq(s21_strchr(str, 'H'), str);
    ck_assert_ptr_eq(s21_strchr(str, 'o'), str + 4);
    ck_assert_ptr_eq(s21_strchr(str, '!'), str + 12);

    assert_strchr_eq(str, 'H');
    assert_strchr_eq(str, 'o');
    assert_strchr_eq(str, '!');
}
END_TEST

START_TEST(test_strchr_not_found) {
    const char *str = "Hello";

    ck_assert_ptr_eq(s21_strchr(str, 'x'), S21_NULL);
    ck_assert_ptr_eq(s21_strchr(str, 'X'), S21_NULL);
    assert_strchr_eq(str, 'z');
}
END_TEST

START_TEST(test_strchr_null_char) {
    const char *str = "Hello";

    char *res = s21_strchr(str, '\0');
    ck_assert_ptr_eq(res, str + 5);

    assert_strchr_eq(str, '\0');
}
END_TEST

START_TEST(test_strchr_empty_string) {
    const char *empty = "";

    ck_assert_ptr_eq(s21_strchr(empty, '\0'), empty);
    ck_assert_ptr_eq(s21_strchr(empty, 'a'), S21_NULL);

    assert_strchr_eq(empty, '\0');
    assert_strchr_eq(empty, 'a');
}
END_TEST

START_TEST(test_strchr_first_char_is_null) {
    const char *str = "\0hello";

    ck_assert_ptr_eq(s21_strchr(str, '\0'), str);
    ck_assert_ptr_eq(s21_strchr(str, 'h'), S21_NULL);

    assert_strchr_eq(str, '\0');
    assert_strchr_eq(str, 'h');
}
END_TEST

int strchr_tests(void) {
    SRunner *rnr =
        init_runner("strchr", "tests", 5, test_strchr_found,
                    test_strchr_not_found, test_strchr_null_char,
                    test_strchr_empty_string, test_strchr_first_char_is_null);

    srunner_run_all(rnr, CK_VERBOSE);
    int failed = srunner_ntests_failed(rnr);
    srunner_free(rnr);

    return failed > 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}

void assert_strcspn_eq(const char *str1, const char *str2) {
    s21_size_t s21_res = s21_strcspn(str1, str2);
    size_t std_res = strcspn(str1, str2);

    ck_assert_uint_eq(s21_res, (s21_size_t)std_res);
}

START_TEST(test_strcspn_basic) {
    const char *str1 = "hello123";
    const char *str2 = "0123456789";

    ck_assert_uint_eq(s21_strcspn(str1, str2), 5);
    assert_strcspn_eq(str1, str2);
}
END_TEST

START_TEST(test_strcspn_first_char_in_str2) {
    const char *str1 = "apple";
    const char *str2 = "a";

    ck_assert_uint_eq(s21_strcspn(str1, str2), 0);
    assert_strcspn_eq(str1, str2);
}
END_TEST

START_TEST(test_strcspn_no_match) {
    const char *str1 = "hello";
    const char *str2 = "123";

    ck_assert_uint_eq(s21_strcspn(str1, str2), 5);
    assert_strcspn_eq(str1, str2);
}
END_TEST

START_TEST(test_strcspn_empty_str1) {
    const char *str1 = "";
    const char *str2 = "abc";

    ck_assert_uint_eq(s21_strcspn(str1, str2), 0);
    assert_strcspn_eq(str1, str2);
}
END_TEST

START_TEST(test_strcspn_empty_str2) {
    const char *str1 = "hello";
    const char *str2 = "";

    ck_assert_uint_eq(s21_strcspn(str1, str2), 5);
    assert_strcspn_eq(str1, str2);
}
END_TEST

START_TEST(test_strcspn_str2_with_duplicates) {
    const char *str1 = "xyz123";
    const char *str2 = "aa11bb";

    ck_assert_uint_eq(s21_strcspn(str1, str2), 3);
    assert_strcspn_eq(str1, str2);
}
END_TEST

START_TEST(test_strcspn_special_chars) {
    const char *str1 = "hello\xFFworld";
    const char *str2 = "\xFF!";

    ck_assert_uint_eq(s21_strcspn(str1, str2), 5);
    assert_strcspn_eq(str1, str2);
}
END_TEST

START_TEST(test_strcspn_str1_has_null_in_middle) {
    char str1[] = "hel\0lo";
    const char *str2 = "o";

    ck_assert_uint_eq(s21_strcspn(str1, str2), 3);
    assert_strcspn_eq(str1, str2);
}
END_TEST

int strcspn_tests(void) {
    SRunner *rnr = init_runner(
        "strcspn", "tests", 8, test_strcspn_basic,
        test_strcspn_first_char_in_str2, test_strcspn_no_match,
        test_strcspn_empty_str1, test_strcspn_empty_str2,
        test_strcspn_str2_with_duplicates, test_strcspn_special_chars,
        test_strcspn_str1_has_null_in_middle);

    srunner_run_all(rnr, CK_VERBOSE);
    int failed = srunner_ntests_failed(rnr);
    srunner_free(rnr);

    return failed > 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}

START_TEST(test_strerror_zero) {
    ck_assert_str_eq(s21_strerror(0), strerror(0));
}
END_TEST

START_TEST(test_strerror_common_errors) {
    int errs[] = {EPERM, ENOENT, ESRCH, EINTR, EIO, EACCES, EINVAL, EMFILE};
    size_t n = sizeof(errs) / sizeof(errs[0]);
    for (size_t i = 0; i < n; ++i) {
        ck_assert_str_eq(s21_strerror(errs[i]), strerror(errs[i]));
    }
}
END_TEST

START_TEST(test_strerror_unknown_negative) {
    int err = -123;
    ck_assert_str_eq(s21_strerror(err), strerror(err));
}
END_TEST

START_TEST(test_strerror_unknown_large) {
    int err = 32767;
    ck_assert_str_eq(s21_strerror(err), strerror(err));
}
END_TEST

START_TEST(test_strerror_multiple_calls_stable) {
    int err = ENOENT;
    ck_assert_str_eq(s21_strerror(err), s21_strerror(err));
    ck_assert_str_eq(s21_strerror(999999), s21_strerror(999999));
}
END_TEST

int strerror_tests(void) {
    SRunner *runner = init_runner(
        "s21_strerror_suite", "s21_strerror_case", 5, test_strerror_zero,
        test_strerror_common_errors, test_strerror_unknown_negative,
        test_strerror_unknown_large, test_strerror_multiple_calls_stable);
    srunner_run_all(runner, CK_VERBOSE);
    int failed = srunner_ntests_failed(runner);
    srunner_free(runner);
    return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}

void assert_strlen_eq(const char *str) {
    s21_size_t s21_res = s21_strlen(str);
    size_t std_res = strlen(str);
    ck_assert_uint_eq(s21_res, (s21_size_t)std_res);
}

START_TEST(test_strlen_empty) {
    const char *str = "";
    ck_assert_uint_eq(s21_strlen(str), 0);
    assert_strlen_eq(str);
}
END_TEST

START_TEST(test_strlen_single_char) {
    const char *str = "a";
    ck_assert_uint_eq(s21_strlen(str), 1);
    assert_strlen_eq(str);
}
END_TEST

START_TEST(test_strlen_normal) {
    const char *str = "Hello, world!";
    ck_assert_uint_eq(s21_strlen(str), 13);
    assert_strlen_eq(str);
}
END_TEST

START_TEST(test_strlen_with_special_chars) {
    const char *str = "abc\x01\x02\xFF";
    ck_assert_uint_eq(s21_strlen(str), 6);
    assert_strlen_eq(str);
}
END_TEST

START_TEST(test_strlen_with_embedded_null) {
    char str[] = "hel\0lo";
    ck_assert_uint_eq(s21_strlen(str), 3);
    assert_strlen_eq(str);
}
END_TEST

START_TEST(test_strlen_long_string) {
    const char *str =
        "This is a very long string for testing purposes, "
        "to ensure that our strlen function works correctly "
        "even on longer inputs without any issues whatsoever.";
    s21_size_t len = s21_strlen(str);
    ck_assert_uint_eq(len, strlen(str));
    assert_strlen_eq(str);
}
END_TEST

int strlen_tests(void) {
    SRunner *rnr = init_runner(
        "strlen", "tests", 6, test_strlen_empty, test_strlen_single_char,
        test_strlen_normal, test_strlen_with_special_chars,
        test_strlen_with_embedded_null, test_strlen_long_string);

    srunner_run_all(rnr, CK_VERBOSE);
    int failed = srunner_ntests_failed(rnr);
    srunner_free(rnr);

    return failed > 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}

START_TEST(test_strncat_append_full) {
    char dest[20] = "Hello";
    const char *src = " world!";
    s21_size_t n = 10;

    char *res = s21_strncat(dest, src, n);
    ck_assert_ptr_eq(res, dest);
    ck_assert_str_eq(dest, "Hello world!");
}
END_TEST

START_TEST(test_strncat_append_partial) {
    char dest[20] = "Hi";
    const char *src = " there!";
    s21_size_t n = 3;

    s21_strncat(dest, src, n);
    ck_assert_str_eq(dest, "Hi th");
}
END_TEST

START_TEST(test_strncat_n_zero) {
    char dest[20] = "Hello";
    const char *src = " world!";

    s21_strncat(dest, src, 0);

    ck_assert_str_eq(dest, "Hello");
}
END_TEST

START_TEST(test_strncat_empty_dest) {
    char dest[20] = "";
    const char *src = "Start";

    s21_strncat(dest, src, 10);
    ck_assert_str_eq(dest, "Start");
}
END_TEST

START_TEST(test_strncat_empty_src) {
    char dest[20] = "Hello";
    const char *src = "";

    s21_strncat(dest, src, 5);
    ck_assert_str_eq(dest, "Hello");
}
END_TEST

START_TEST(test_strncat_with_null_bytes) {
    char dest[20] = "pre";
    char src[] = "a\xFF\x00end";
    s21_size_t n = 10;

    s21_strncat(dest, src, n);
    ck_assert_str_eq(dest, "prea\xFF");
}
END_TEST

int strncat_tests(void) {
    SRunner *rnr = init_runner("strncat", "tests", 6, test_strncat_append_full,
                               test_strncat_append_partial, test_strncat_n_zero,
                               test_strncat_empty_dest, test_strncat_empty_src,
                               test_strncat_with_null_bytes);

    srunner_run_all(rnr, CK_VERBOSE);
    int failed = srunner_ntests_failed(rnr);
    srunner_free(rnr);

    return failed > 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}

START_TEST(test_strncmp_equal) {
    ck_assert_int_eq(s21_strncmp("abc", "abc", 3), 0);
    ck_assert_int_eq(s21_strncmp("abc", "abc", 100), 0);
}
END_TEST

START_TEST(test_strncmp_n_zero) {
    ck_assert_int_eq(s21_strncmp("abc", "xyz", 0), 0);
    ck_assert_int_eq(s21_strncmp("", "", 0), 0);
}
END_TEST

START_TEST(test_strncmp_less_greater_basic) {
    ck_assert(s21_strncmp("abc", "abd", 3) < 0);
    ck_assert(s21_strncmp("abd", "abc", 3) > 0);
    ck_assert(s21_strncmp("a", "b", 1) < 0);
    ck_assert(s21_strncmp("b", "a", 1) > 0);
}
END_TEST

START_TEST(test_strncmp_partial_prefix) {
    ck_assert_int_eq(s21_strncmp("abc", "abd", 2), 0);
    ck_assert_int_eq(s21_strncmp("abc", "ab", 2), 0);
    ck_assert(s21_strncmp("abc", "ab", 3) > 0);
    ck_assert(s21_strncmp("ab", "abc", 3) < 0);
}
END_TEST

START_TEST(test_strncmp_length_limits) {
    ck_assert(s21_strncmp("ab", "abc", 10) < 0);
    ck_assert(s21_strncmp("abc", "ab", 10) > 0);
    ck_assert_int_eq(s21_strncmp("ab", "abc", 2), 0);
}
END_TEST

START_TEST(test_strncmp_empty_strings) {
    ck_assert_int_eq(s21_strncmp("", "", 1), 0);
    ck_assert(s21_strncmp("", "a", 1) < 0);
    ck_assert(s21_strncmp("a", "", 1) > 0);
}
END_TEST

START_TEST(test_strncmp_embedded_nulls) {
    const char a[] = {'a', 'b', '\0', 'c', 0};
    const char b[] = {'a', 'b', '\0', 'd', 0};
    ck_assert_int_eq(s21_strncmp(a, b, 4), 0);
    ck_assert_int_eq(s21_strncmp(a, b, 3), 0);
    ck_assert_int_eq(s21_strncmp(a, b, 2), 0);
}
END_TEST

START_TEST(test_strncmp_signed_chars) {
    const char x[] = {(char)0x80, 0};
    const char y[] = {(char)0x7F, 0};
    ck_assert(s21_strncmp(x, y, 1) > 0 || s21_strncmp(x, y, 1) < 0);
}
END_TEST

int strncmp_tests(void) {
    SRunner *runner = init_runner(
        "s21_strncmp", "tests", 8, test_strncmp_equal, test_strncmp_n_zero,
        test_strncmp_less_greater_basic, test_strncmp_partial_prefix,
        test_strncmp_length_limits, test_strncmp_empty_strings,
        test_strncmp_embedded_nulls, test_strncmp_signed_chars);
    srunner_run_all(runner, CK_VERBOSE);
    int failed = srunner_ntests_failed(runner);
    srunner_free(runner);
    return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}

void assert_strncpy_eq(const char *src, s21_size_t n) {
    char *dest1 = calloc(n, 1);
    char *dest2 = calloc(n, 1);
    ck_assert_ptr_ne(dest1, NULL);
    ck_assert_ptr_ne(dest2, NULL);

    char *res1 = s21_strncpy(dest1, src, n);
    char *res2 = strncpy(dest2, src, n);

    ck_assert_ptr_eq(res1, dest1);
    ck_assert_ptr_eq(res2, dest2);

    ck_assert_int_eq(memcmp(dest1, dest2, n), 0);

    free(dest1);
    free(dest2);
}

START_TEST(test_strncpy_src_shorter_than_n) {
    char dest[10] = {0};
    const char *src = "hi";

    s21_strncpy(dest, src, 10);

    ck_assert_str_eq(dest, "hi");
    ck_assert_int_eq(dest[0], 'h');
    ck_assert_int_eq(dest[1], 'i');
    for (int i = 2; i < 10; ++i) {
        ck_assert_int_eq(dest[i], '\0');
    }

    assert_strncpy_eq(src, 10);
}
END_TEST

START_TEST(test_strncpy_src_longer_than_n) {
    char dest[5] = "xxxxx";
    const char *src = "hello world";

    s21_strncpy(dest, src, 5);

    ck_assert_int_eq(dest[0], 'h');
    ck_assert_int_eq(dest[1], 'e');
    ck_assert_int_eq(dest[2], 'l');
    ck_assert_int_eq(dest[3], 'l');
    ck_assert_int_eq(dest[4], 'o');

    assert_strncpy_eq(src, 5);
}
END_TEST

START_TEST(test_strncpy_src_equal_to_n) {
    char dest[5] = "xxxxx";
    const char *src = "12345";

    s21_strncpy(dest, src, 5);

    for (int i = 0; i < 5; ++i) {
        ck_assert_int_eq(dest[i], src[i]);
    }
    assert_strncpy_eq(src, 5);
}
END_TEST

START_TEST(test_strncpy_n_zero) {
    char dest[5] = "hello";
    const char *src = "world";

    s21_strncpy(dest, src, 0);

    ck_assert_str_eq(dest, "hello");
}
END_TEST

START_TEST(test_strncpy_empty_src) {
    char dest[5] = "xxxxx";
    const char *src = "";

    s21_strncpy(dest, src, 5);

    for (int i = 0; i < 5; ++i) {
        ck_assert_int_eq(dest[i], '\0');
    }
    assert_strncpy_eq(src, 5);
}
END_TEST

START_TEST(test_strncpy_with_embedded_null) {
    char dest[10] = {0};
    char src[] = "ab\0cd";

    s21_strncpy(dest, src, 10);

    ck_assert_int_eq(dest[0], 'a');
    ck_assert_int_eq(dest[1], 'b');
    ck_assert_int_eq(dest[2], '\0');
    for (int i = 3; i < 10; ++i) {
        ck_assert_int_eq(dest[i], '\0');
    }
    assert_strncpy_eq(src, 10);
}
END_TEST

int strncpy_tests(void) {
    SRunner *rnr =
        init_runner("strncpy", "tests", 6, test_strncpy_src_shorter_than_n,
                    test_strncpy_src_longer_than_n, test_strncpy_src_equal_to_n,
                    test_strncpy_n_zero, test_strncpy_empty_src,
                    test_strncpy_with_embedded_null);

    srunner_run_all(rnr, CK_VERBOSE);
    int failed = srunner_ntests_failed(rnr);
    srunner_free(rnr);

    return failed > 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}

void assert_strpbrk_eq(const char *str1, const char *str2) {
    char *s21_res = s21_strpbrk(str1, str2);
    char *std_res = strpbrk(str1, str2);

    if (s21_res == S21_NULL && std_res == NULL) return;
    if (s21_res != S21_NULL && std_res != NULL) {
        ck_assert_ptr_eq(s21_res, std_res);
        return;
    }
    ck_assert_msg(0,
                  "s21_strpbrk and strpbrk differ:\n"
                  "  str1 = \"%s\", str2 = \"%s\"\n"
                  "  s21 = %p, std = %p",
                  str1 ? str1 : "(null)", str2 ? str2 : "(null)", s21_res,
                  std_res);
}

START_TEST(test_strpbrk_found) {
    const char *str1 = "hello world";
    const char *str2 = "aeiou";

    ck_assert_ptr_eq(s21_strpbrk(str1, str2), str1 + 1);
    assert_strpbrk_eq(str1, str2);
}
END_TEST

START_TEST(test_strpbrk_found_at_end) {
    const char *str1 = "xyz!";
    const char *str2 = "!?";

    ck_assert_ptr_eq(s21_strpbrk(str1, str2), str1 + 3);
    assert_strpbrk_eq(str1, str2);
}
END_TEST

START_TEST(test_strpbrk_not_found) {
    const char *str1 = "hello";
    const char *str2 = "123";

    ck_assert_ptr_eq(s21_strpbrk(str1, str2), S21_NULL);
    assert_strpbrk_eq(str1, str2);
}
END_TEST

START_TEST(test_strpbrk_empty_str1) {
    const char *str1 = "";
    const char *str2 = "a";

    ck_assert_ptr_eq(s21_strpbrk(str1, str2), S21_NULL);
    assert_strpbrk_eq(str1, str2);
}
END_TEST

START_TEST(test_strpbrk_empty_str2) {
    const char *str1 = "hello";
    const char *str2 = "";

    ck_assert_ptr_eq(s21_strpbrk(str1, str2), S21_NULL);
    assert_strpbrk_eq(str1, str2);
}
END_TEST

START_TEST(test_strpbrk_str2_with_duplicates) {
    const char *str1 = "hello";
    const char *str2 = "aaeeoo";

    ck_assert_ptr_eq(s21_strpbrk(str1, str2), str1 + 1);
    assert_strpbrk_eq(str1, str2);
}
END_TEST

START_TEST(test_strpbrk_high_ascii) {
    char str1[] = "test\xFend";
    const char *str2 = "\xF$";

    ck_assert_ptr_eq(s21_strpbrk(str1, str2), str1 + 4);
    assert_strpbrk_eq(str1, str2);
}
END_TEST

START_TEST(test_strpbrk_first_char_matches) {
    const char *str1 = "apple";
    const char *str2 = "a";

    ck_assert_ptr_eq(s21_strpbrk(str1, str2), str1);
    assert_strpbrk_eq(str1, str2);
}
END_TEST

START_TEST(test_strpbrk_null_args) {
    ck_assert_ptr_eq(s21_strpbrk(NULL, "a"), S21_NULL);
    ck_assert_ptr_eq(s21_strpbrk("a", NULL), S21_NULL);
    ck_assert_ptr_eq(s21_strpbrk(NULL, NULL), S21_NULL);
}
END_TEST

int strpbrk_tests(void) {
    SRunner *rnr =
        init_runner("strpbrk", "tests", 9, test_strpbrk_found,
                    test_strpbrk_found_at_end, test_strpbrk_not_found,
                    test_strpbrk_empty_str1, test_strpbrk_empty_str2,
                    test_strpbrk_str2_with_duplicates, test_strpbrk_high_ascii,
                    test_strpbrk_first_char_matches, test_strpbrk_null_args);

    srunner_run_all(rnr, CK_VERBOSE);
    int failed = srunner_ntests_failed(rnr);
    srunner_free(rnr);

    return failed > 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}

void assert_strrchr_eq(const char *str, int c) {
    char *s21_res = s21_strrchr(str, c);
    char *std_res = strrchr(str, c);

    if (s21_res == S21_NULL && std_res == NULL) return;
    if (s21_res != S21_NULL && std_res != NULL) {
        ck_assert_ptr_eq(s21_res, std_res);
        return;
    }
    ck_assert_msg(0,
                  "s21_strrchr and strrchr differ:\n"
                  "  str = \"%s\", c = %d (0x%02X)\n"
                  "  s21 = %p, std = %p",
                  str ? str : "(null)", c, (unsigned char)c, s21_res, std_res);
}

START_TEST(test_strrchr_single_occurrence) {
    const char *str = "hello";
    ck_assert_ptr_eq(s21_strrchr(str, 'e'), str + 1);
    assert_strrchr_eq(str, 'e');
}
END_TEST

START_TEST(test_strrchr_multiple_occurrences) {
    const char *str = "banana";
    ck_assert_ptr_eq(s21_strrchr(str, 'a'), str + 5);
    assert_strrchr_eq(str, 'a');
}
END_TEST

START_TEST(test_strrchr_not_found) {
    const char *str = "hello";
    ck_assert_ptr_eq(s21_strrchr(str, 'x'), S21_NULL);
    assert_strrchr_eq(str, 'x');
}
END_TEST

START_TEST(test_strrchr_null_char) {
    const char *str = "hello";
    char *res = s21_strrchr(str, '\0');
    ck_assert_ptr_eq(res, str + 5);
    assert_strrchr_eq(str, '\0');
}
END_TEST

START_TEST(test_strrchr_empty_string) {
    const char *str = "";
    ck_assert_ptr_eq(s21_strrchr(str, '\0'), str);
    ck_assert_ptr_eq(s21_strrchr(str, 'a'), S21_NULL);
    assert_strrchr_eq(str, '\0');
    assert_strrchr_eq(str, 'a');
}
END_TEST

START_TEST(test_strrchr_char_at_end) {
    const char *str = "hello!";
    ck_assert_ptr_eq(s21_strrchr(str, '!'), str + 5);
    assert_strrchr_eq(str, '!');
}
END_TEST

START_TEST(test_strrchr_null_str) {
    ck_assert_ptr_eq(s21_strrchr(NULL, 'a'), S21_NULL);
}
END_TEST

int strrchr_tests(void) {
    SRunner *rnr =
        init_runner("strrchr", "tests", 7, test_strrchr_single_occurrence,
                    test_strrchr_multiple_occurrences, test_strrchr_not_found,
                    test_strrchr_null_char, test_strrchr_empty_string,
                    test_strrchr_char_at_end, test_strrchr_null_str);

    srunner_run_all(rnr, CK_VERBOSE);
    int failed = srunner_ntests_failed(rnr);
    srunner_free(rnr);

    return failed > 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}

void assert_strstr_eq(const char *haystack, const char *needle) {
    char *s21_res = s21_strstr(haystack, needle);
    char *std_res = strstr(haystack, needle);

    if (s21_res == S21_NULL && std_res == NULL) return;
    if (s21_res != S21_NULL && std_res != NULL) {
        ck_assert_ptr_eq(s21_res, std_res);
        return;
    }
    ck_assert_msg(0,
                  "s21_strstr and strstr differ:\n"
                  "  haystack = \"%s\", needle = \"%s\"\n"
                  "  s21 = %p, std = %p",
                  haystack ? haystack : "(null)", needle ? needle : "(null)",
                  s21_res, std_res);
}

START_TEST(test_strstr_found_beginning) {
    const char *h = "hello world";
    const char *n = "hello";
    ck_assert_ptr_eq(s21_strstr(h, n), h);
    assert_strstr_eq(h, n);
}
END_TEST

START_TEST(test_strstr_found_middle) {
    const char *h = "hello world";
    const char *n = "lo w";
    ck_assert_ptr_eq(s21_strstr(h, n), h + 3);
    assert_strstr_eq(h, n);
}
END_TEST

START_TEST(test_strstr_found_at_end) {
    const char *h = "hello";
    const char *n = "lo";
    ck_assert_ptr_eq(s21_strstr(h, n), h + 3);
    assert_strstr_eq(h, n);
}
END_TEST

START_TEST(test_strstr_needle_empty) {
    const char *h = "anything";
    const char *n = "";
    ck_assert_ptr_eq(s21_strstr(h, n), h);
    assert_strstr_eq(h, n);
}
END_TEST

START_TEST(test_strstr_haystack_empty) {
    const char *h = "";
    const char *n = "a";
    ck_assert_ptr_eq(s21_strstr(h, n), S21_NULL);
    assert_strstr_eq(h, n);
}
END_TEST

START_TEST(test_strstr_both_empty) {
    const char *h = "";
    const char *n = "";
    ck_assert_ptr_eq(s21_strstr(h, n), h);
    assert_strstr_eq(h, n);
}
END_TEST

START_TEST(test_strstr_not_found) {
    const char *h = "hello";
    const char *n = "world";
    ck_assert_ptr_eq(s21_strstr(h, n), S21_NULL);
    assert_strstr_eq(h, n);
}
END_TEST

START_TEST(test_strstr_needle_longer_than_haystack) {
    const char *h = "hi";
    const char *n = "hello";
    ck_assert_ptr_eq(s21_strstr(h, n), S21_NULL);
    assert_strstr_eq(h, n);
}
END_TEST

START_TEST(test_strstr_special_chars) {
    char h[] = "test\xFF\x00end";
    const char *n = "\xFF";
    ck_assert_ptr_eq(s21_strstr(h, n), h + 4);
    assert_strstr_eq(h, n);
}
END_TEST

int strstr_tests(void) {
    SRunner *rnr = init_runner(
        "strstr", "tests", 9, test_strstr_found_beginning,
        test_strstr_found_middle, test_strstr_found_at_end,
        test_strstr_needle_empty, test_strstr_haystack_empty,
        test_strstr_both_empty, test_strstr_not_found,
        test_strstr_needle_longer_than_haystack, test_strstr_special_chars);

    srunner_run_all(rnr, CK_VERBOSE);
    int failed = srunner_ntests_failed(rnr);
    srunner_free(rnr);

    return failed > 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}

START_TEST(test_strtok_basic) {
    char str[] = "a,b,c";
    char *token = s21_strtok(str, ",");
    ck_assert_str_eq(token, "a");
    token = s21_strtok(S21_NULL, ",");
    ck_assert_str_eq(token, "b");
    token = s21_strtok(S21_NULL, ",");
    ck_assert_str_eq(token, "c");
    token = s21_strtok(S21_NULL, ",");
    ck_assert_ptr_eq(token, S21_NULL);
}
END_TEST

START_TEST(test_strtok_multiple_delims) {
    char str[] = "a;b,c";
    char *token = s21_strtok(str, ";,");
    ck_assert_str_eq(token, "a");
    token = s21_strtok(S21_NULL, ";,");
    ck_assert_str_eq(token, "b");
    token = s21_strtok(S21_NULL, ";,");
    ck_assert_str_eq(token, "c");
    token = s21_strtok(S21_NULL, ";,");
    ck_assert_ptr_eq(token, S21_NULL);
}
END_TEST

START_TEST(test_strtok_consecutive_delims) {
    char str[] = "a,,b";
    char *token = s21_strtok(str, ",");
    ck_assert_str_eq(token, "a");
    token = s21_strtok(S21_NULL, ",");
    ck_assert_str_eq(token, "b");
    token = s21_strtok(S21_NULL, ",");
    ck_assert_ptr_eq(token, S21_NULL);
}
END_TEST

START_TEST(test_strtok_only_delims) {
    char str[] = ",,,";
    char *token = s21_strtok(str, ",");
    ck_assert_ptr_eq(token, S21_NULL);
}
END_TEST

START_TEST(test_strtok_empty_string) {
    char str[] = "";
    char *token = s21_strtok(str, ",");
    ck_assert_ptr_eq(token, S21_NULL);
}
END_TEST

START_TEST(test_strtok_empty_delim) {
    char str[] = "abc";
    char *token = s21_strtok(str, "");
    ck_assert_str_eq(token, "abc");
    token = s21_strtok(S21_NULL, "");
    ck_assert_ptr_eq(token, S21_NULL);
}
END_TEST

START_TEST(test_strtok_null_str) {
    char *token = s21_strtok(S21_NULL, ",");
    ck_assert_ptr_eq(token, S21_NULL);
}
END_TEST

START_TEST(test_strtok_null_delim) {
    char str[] = "abc";
    char *token = s21_strtok(str, S21_NULL);
    ck_assert_ptr_eq(token, S21_NULL);
}
END_TEST

int strtok_tests(void) {
    SRunner *runner = init_runner(
        "strtok", "tests", 8, test_strtok_basic, test_strtok_multiple_delims,
        test_strtok_consecutive_delims, test_strtok_only_delims,
        test_strtok_empty_string, test_strtok_empty_delim, test_strtok_null_str,
        test_strtok_null_delim);
    srunner_run_all(runner, CK_VERBOSE);
    int failed = srunner_ntests_failed(runner);
    srunner_free(runner);
    return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}

START_TEST(test_to_lower_basic) {
    char *res = s21_to_lower("ABCxyz123");
    ck_assert_str_eq(res, "abcxyz123");
    free(res);
}
END_TEST

START_TEST(test_to_lower_empty) {
    char *res = s21_to_lower("");
    ck_assert_str_eq(res, "");
    free(res);
}
END_TEST

START_TEST(test_to_lower_all_lower) {
    char *res = s21_to_lower("hello");
    ck_assert_str_eq(res, "hello");
    free(res);
}
END_TEST

START_TEST(test_to_lower_mixed) {
    char *res = s21_to_lower("AbCdEf");
    ck_assert_str_eq(res, "abcdef");
    free(res);
}
END_TEST

START_TEST(test_to_lower_null) {
    char *res = s21_to_lower(S21_NULL);
    ck_assert_ptr_eq(res, S21_NULL);
}
END_TEST

int tolower_tests(void) {
    SRunner *runner = init_runner("s21_to_lower_suite", "s21_to_lower_case", 5,
                                  test_to_lower_basic, test_to_lower_empty,
                                  test_to_lower_all_lower, test_to_lower_mixed,
                                  test_to_lower_null);

    srunner_run_all(runner, CK_VERBOSE);
    int failed = srunner_ntests_failed(runner);
    srunner_free(runner);
    return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}

START_TEST(test_to_upper_basic) {
    char *res = s21_to_upper("abcXYZ123");
    ck_assert_str_eq(res, "ABCXYZ123");
    free(res);
}
END_TEST

START_TEST(test_to_upper_empty) {
    char *res = s21_to_upper("");
    ck_assert_str_eq(res, "");
    free(res);
}
END_TEST

START_TEST(test_to_upper_all_upper) {
    char *res = s21_to_upper("HELLO");
    ck_assert_str_eq(res, "HELLO");
    free(res);
}
END_TEST

START_TEST(test_to_upper_mixed) {
    char *res = s21_to_upper("aBcDeF");
    ck_assert_str_eq(res, "ABCDEF");
    free(res);
}
END_TEST

START_TEST(test_to_upper_null) {
    char *res = s21_to_upper(S21_NULL);
    ck_assert_ptr_eq(res, S21_NULL);
}
END_TEST

int toupper_tests(void) {
    SRunner *runner = init_runner("s21_to_upper_suite", "s21_to_upper_case", 5,
                                  test_to_upper_basic, test_to_upper_empty,
                                  test_to_upper_all_upper, test_to_upper_mixed,
                                  test_to_upper_null);

    srunner_run_all(runner, CK_VERBOSE);
    int failed = srunner_ntests_failed(runner);
    srunner_free(runner);
    return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}

START_TEST(test_trim_null_inputs) {
    ck_assert_ptr_eq(s21_trim(S21_NULL, "abc"), S21_NULL);
    ck_assert_ptr_eq(s21_trim("hello", S21_NULL), S21_NULL);
}
END_TEST

START_TEST(test_trim_no_trim_needed) {
    char *res = s21_trim("hello", "xyz");
    ck_assert_str_eq(res, "hello");
    free(res);
}
END_TEST

START_TEST(test_trim_leading) {
    char *res = s21_trim("+++hello", "+");
    ck_assert_str_eq(res, "hello");
    free(res);
}
END_TEST

START_TEST(test_trim_trailing) {
    char *res = s21_trim("world***", "*");
    ck_assert_str_eq(res, "world");
    free(res);
}
END_TEST

START_TEST(test_trim_both_sides) {
    char *res = s21_trim("...hi...", ".");
    ck_assert_str_eq(res, "hi");
    free(res);
}
END_TEST

START_TEST(test_trim_inner_not_removed) {
    char *res = s21_trim("--a--b--", "-");
    ck_assert_str_eq(res, "a--b");
    free(res);
}
END_TEST

START_TEST(test_trim_all_chars) {
    char *res = s21_trim("*****", "*");
    ck_assert_str_eq(res, "");
    free(res);
}
END_TEST

START_TEST(test_trim_multiple_chars) {
    char *res = s21_trim(" \t\nHello World!\n\t ", " \t\n");
    ck_assert_str_eq(res, "Hello World!");
    free(res);
}
END_TEST

START_TEST(test_trim_empty_string) {
    char *res = s21_trim("", "abc");
    ck_assert_str_eq(res, "");
    free(res);
}
END_TEST

START_TEST(test_trim_no_chars_to_trim) {
    char *res = s21_trim("abc", "");
    ck_assert_str_eq(res, "abc");
    free(res);
}
END_TEST

int trim_tests(void) {
    SRunner *runner = init_runner(
        "s21_trim_suite", "s21_trim_case", 10, test_trim_null_inputs,
        test_trim_no_trim_needed, test_trim_leading, test_trim_trailing,
        test_trim_both_sides, test_trim_inner_not_removed, test_trim_all_chars,
        test_trim_multiple_chars, test_trim_empty_string,
        test_trim_no_chars_to_trim);

    srunner_run_all(runner, CK_VERBOSE);
    int failed = srunner_ntests_failed(runner);
    srunner_free(runner);
    return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}

int main(void) {
    trim_tests();
    insert_tests();
    memchr_tests();
    memcmp_tests();
    memcpy_tests();
    memset_tests();
    strchr_tests();
    strlen_tests();
    strstr_tests();
    strtok_tests();
    prelude_tests();
    sprintf_tests();
    strcspn_tests();
    strncat_tests();
    strncmp_tests();
    strncpy_tests();
    strpbrk_tests();
    strrchr_tests();
    tolower_tests();
    toupper_tests();
    strerror_tests();

    return EXIT_SUCCESS;
}

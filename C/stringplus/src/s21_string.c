#include "s21_string.h"

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

char s21_new_register(char symb, int mode) {
    int step = 32;
    if (mode == 1) {
        if (symb >= 'A' && symb <= 'Z') {
            symb = symb + step;
        }
    } else if (mode == 0) {
        if (symb >= 'a' && symb <= 'z') {
            symb = symb - step;
        }
    }
    return symb;
}

void *s21_memchr(const void *str, int c, s21_size_t n) {
    const unsigned char *p = (const unsigned char *)str;
    for (s21_size_t i = 0; i < n; ++i) {
        if (p[i] == (unsigned char)c) {
            return (void *)(p + i);
        }
    }
    return S21_NULL;
}

int s21_memcmp(const void *str1, const void *str2, s21_size_t n) {
    const unsigned char *p1 = (const unsigned char *)str1;
    const unsigned char *p2 = (const unsigned char *)str2;

    for (s21_size_t i = 0; i < n; ++i) {
        if (p1[i] != p2[i]) {
            return p1[i] - p2[i];
        }
    }
    return 0;
}

void *s21_memcpy(void *dest, const void *src, s21_size_t n) {
    char *p_dest = (char *)dest;
    const char *p_src = (const char *)src;

    for (s21_size_t i = 0; i < n; ++i) {
        p_dest[i] = p_src[i];
    }

    return dest;
}

void *s21_memset(void *str, int c, s21_size_t n) {
    char *p_str = (char *)str;

    for (s21_size_t i = 0; i < n; ++i) {
        p_str[i] = (unsigned char)c;
    }

    return str;
}

char *s21_strncat(char *dest, const char *src, s21_size_t n) {
    s21_size_t len = s21_strlen(dest);
    s21_size_t i = 0;

    while (i < n && src[i] != '\0') {
        dest[len + i] = src[i];
        i++;
    }
    dest[len + i] = '\0';

    return dest;
}

char *s21_strchr(const char *str, int c) {
    while (*str) {
        if (*str == (char)c) return (char *)str;
        str++;
    }
    if (c == '\0') return (char *)str;
    return S21_NULL;
}

int s21_strncmp(const char *str1, const char *str2, s21_size_t n) {
    const unsigned char *s1 = (const unsigned char *)str1;
    const unsigned char *s2 = (const unsigned char *)str2;

    if (n == 0) return 0;

    while (n-- > 0 && *s1 && (*s1 == *s2)) {
        if (n == 0) return 0;
        s1++;
        s2++;
    }

    return (n == (s21_size_t)-1) ? 0 : (*s1 - *s2);
}

char *s21_strncpy(char *dest, const char *src, s21_size_t n) {
    if (n == 0) {
        if (dest) dest[0] = '\0';
        return dest;
    }
    s21_size_t i = 0;
    for (; i < n && src[i] != '\0'; ++i) dest[i] = src[i];
    for (; i < n; ++i) dest[i] = '\0';
    return dest;
}

s21_size_t s21_strcspn(const char *str1, const char *str2) {
    s21_size_t str1_len = s21_strlen(str1);
    s21_size_t str2_len = s21_strlen(str2);

    for (s21_size_t i = 0; i < str1_len; ++i) {  // O(n*m)
        for (s21_size_t j = 0; j < str2_len; ++j) {
            if (str1[i] == str2[j]) {
                return i;
            }
        }
    }

    return str1_len;
}

char *s21_strerror(int errnum) {
    static char unknown[64];
    if (errnum >= 0 && errnum < S21_ERRLIST_LEN) {
        const char *msg = s21_errlist[errnum];
        if (msg && *msg) return (char *)msg;
    }
    snprintf(unknown, sizeof(unknown), S21_UNKNOWN_FMT, errnum);
    return unknown;
}

s21_size_t s21_strlen(const char *str) {
    s21_size_t len = 0;
    while (*str != '\0') {
        len++;
        str++;
    }
    return len;
}

char *s21_strpbrk(const char *s1, const char *s2) {
    if (!s1 || !s2) return S21_NULL;

    for (; *s1; s1++) {
        unsigned char c = (unsigned char)*s1;
        if (s21_strchr(s2, c) != S21_NULL) return (char *)s1;
    }
    return S21_NULL;
}

char *s21_strrchr(const char *str, int c) {
    char *res = S21_NULL;

    if (str) {
        if (c == '\0') {
            res = (char *)(str + s21_strlen(str));
        } else {
            s21_size_t i = 0;
            while (str[i] != '\0') {
                if (str[i] == (char)c) {
                    res = (char *)(str + i);
                }
                i++;
            }
        }
    }

    return res;
}

char *s21_strstr(const char *haystack, const char *needle) {
    char *res = S21_NULL;
    s21_size_t hlen = 0, nlen = 0, i, j;
    int found;

    if (haystack && needle) {
        hlen = s21_strlen(haystack);
        nlen = s21_strlen(needle);

        if (nlen == 0) {
            res = (char *)haystack;
        } else if (nlen <= hlen) {
            for (i = 0; i <= hlen - nlen && res == S21_NULL; i++) {
                found = 1;
                for (j = 0; j < nlen; j++) {
                    if (haystack[i + j] != needle[j]) {
                        found = 0;
                        break;
                    }
                }
                if (found) {
                    res = (char *)(haystack + i);
                }
            }
        }
    }

    return res;
}

char *s21_strtok(char *str, const char *delim) {
    static char *next = S21_NULL;
    if (delim == S21_NULL) return S21_NULL;

    if (str != S21_NULL) {
        next = str;
    } else if (next == S21_NULL) {
        return S21_NULL;
    }

    if (delim[0] == '\0') {
        if (next == S21_NULL || *next == '\0') return S21_NULL;
        char *ret = next;
        next = S21_NULL;
        return ret;
    }

    while (*next && s21_strchr(delim, *next)) next++;
    if (*next == '\0') {
        next = S21_NULL;
        return S21_NULL;
    }

    char *start = next;
    while (*next && s21_strchr(delim, *next) == S21_NULL) next++;

    if (*next) {
        *next = '\0';
        next++;
    } else {
        next = S21_NULL;
    }

    return start;
}

void *s21_to_upper(const char *str) {
    void *result = S21_NULL;
    if (str != S21_NULL) {
        s21_size_t len = s21_strlen(str);
        if (len >= (s21_size_t)SIZE_MAX - 1) return S21_NULL;
        char *res = malloc(len + 1);
        if (res != S21_NULL) {
            for (s21_size_t i = 0; i < len; i++) {
                res[i] = (char)s21_new_register(str[i], 0);
            }
            res[len] = '\0';
            result = res;
        }
    }
    return result;
}

void *s21_to_lower(const char *str) {
    void *result = S21_NULL;
    if (str != S21_NULL) {
        s21_size_t len = s21_strlen(str);
        if (len >= (s21_size_t)SIZE_MAX - 1) return S21_NULL;
        char *res = malloc(len + 1);
        if (res != S21_NULL) {
            for (s21_size_t i = 0; i < len; i++) {
                res[i] = (char)s21_new_register(str[i], 1);
            }
            res[len] = '\0';
            result = res;
        }
    }
    return result;
}

void *s21_insert(const char *src, const char *str, s21_size_t start_index) {
    if (!src || !str) {
        return S21_NULL;
    }

    s21_size_t src_len = s21_strlen(src);
    s21_size_t str_len = s21_strlen(str);

    if (start_index > src_len) return S21_NULL;

    char *res = (char *)malloc(src_len + str_len + 1);
    if (!res) return S21_NULL;

    if (start_index > 0) s21_memcpy(res, src, start_index);
    if (str_len > 0) s21_memcpy(res + start_index, str, str_len);

    if (src_len - start_index > 0)
        s21_memcpy(res + start_index + str_len, src + start_index,
                   src_len - start_index);

    res[src_len + str_len] = '\0';
    return res;
}

void *s21_trim(const char *src, const char *trim_chars) {
    void *result = S21_NULL;
    if (src != S21_NULL && trim_chars != S21_NULL) {
        const char *start = src;
        while (*start && s21_strchr(trim_chars, *start)) {
            start++;
        }
        if (*start == '\0') {
            char *empty = (char *)malloc(1);
            if (empty != S21_NULL) {
                empty[0] = '\0';
                result = empty;
            }
        } else {
            const char *end = src + s21_strlen(src) - 1;
            while (end > start && s21_strchr(trim_chars, *end)) {
                end--;
            }
            s21_size_t len = (s21_size_t)(end - start + 1);
            char *res = (char *)malloc(len + 1);
            if (res != S21_NULL) {
                for (s21_size_t i = 0; i < len; i++) {
                    res[i] = start[i];
                }
                res[len] = '\0';
                result = res;
            }
        }
    }
    return result;
}
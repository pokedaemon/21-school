#include <math.h>
#include <stdarg.h>
#include <stdbool.h>

#include "s21_string.h"

typedef struct format {
    char specifier;
    char length;
    bool flag_plus;
    bool flag_minus;
    bool flag_space;
    bool flag_zero;
    int width;
    int precision;
    char type;
} s21_format;

int isdig(char c) { return c >= '0' && c <= '9'; }

s21_format s21_parse_format(const char **fmt_ptr) {
    (void)(s21_errlist);
    const char *fmt = *fmt_ptr;
    s21_format f = {0};
    f.width = 0;
    f.precision = -1;

    for (;; ++fmt) {
        if (*fmt == '+')
            f.flag_plus = true;
        else if (*fmt == '-')
            f.flag_minus = true;
        else if (*fmt == ' ')
            f.flag_space = true;
        else if (*fmt == '0')
            f.flag_zero = true;
        else
            break;
    }
    if (f.flag_plus) f.flag_space = false;

    if (isdig(*fmt)) {
        while (isdig(*fmt)) {
            f.width = f.width * 10 + (*fmt - '0');
            ++fmt;
        }
    }

    if (*fmt == '.') {
        ++fmt;
        f.precision = 0;
        while (isdig(*fmt)) {
            f.precision = f.precision * 10 + (*fmt - '0');
            ++fmt;
        }
    }

    if (*fmt == 'h') {
        if (*(fmt + 1) == 'h') {
            f.length = 'H';
            fmt += 2;
        } else {
            f.length = 'h';
            ++fmt;
        }
    } else if (*fmt == 'l') {
        if (*(fmt + 1) == 'l') {
            f.length = 'L';
            fmt += 2;
        } else {
            f.length = 'l';
            ++fmt;
        }
    }

    f.specifier = *fmt ? *fmt++ : 0;

    switch (f.specifier) {
        case 'd':
        case 'i':
            f.type = 'i';
            break;
        case 'u':
            f.type = 'u';
            break;
        case 'f':
            f.type = 'f';
            break;
        case 'c':
            f.type = 'c';
            break;
        case 's':
            f.type = 's';
            break;
        case '%':
            f.type = '%';
            break;
        default:
            f.type = 0;
            break;
    }

    *fmt_ptr = fmt;
    return f;
}

char *s21_pad(char *dst, int width, int len, int left_align, char padch) {
    int pad = width > len ? width - len : 0;
    if (!left_align)
        while (pad--) *dst++ = padch;
    return dst;
}

void s21_int_to_str(long long v, char *buf) {
    char tmp[64];
    int n = 0, neg = 0;

    if (v == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return;
    }
    if (v < 0) {
        neg = 1;
        v = -v;
    }

    while (v) {
        tmp[n++] = (char)('0' + (v % 10));
        v /= 10;
    }

    int k = 0;
    if (neg) buf[k++] = '-';
    while (n) buf[k++] = tmp[--n];
    buf[k] = '\0';
}

static char *s21_print_int(char *dst, long long v, const s21_format f) {
    int negative = (v < 0);
    unsigned long long uv =
        negative ? (unsigned long long)(-v) : (unsigned long long)v;

    char digits[64];
    if (uv == 0) {
        digits[0] = '0';
        digits[1] = '\0';
    } else {
        char tmp[64];
        int n = 0;
        while (uv) {
            tmp[n++] = (char)('0' + (uv % 10));
            uv /= 10;
        }
        for (int i = 0; i < n; ++i) digits[i] = tmp[n - 1 - i];
        digits[n] = '\0';
    }

    if (f.precision == 0 && digits[0] == '0' && digits[1] == '\0')
        digits[0] = '\0';

    char sign = 0;
    if (negative)
        sign = '-';
    else if (f.flag_plus)
        sign = '+';
    else if (f.flag_space)
        sign = ' ';

    int dig_len = (int)s21_strlen(digits);
    int need_zeros = (f.precision > dig_len) ? (f.precision - dig_len) : 0;

    char core[128];
    int pos = 0;
    if (sign) core[pos++] = sign;
    for (int i = 0; i < need_zeros; ++i) core[pos++] = '0';
    s21_strncpy(core + pos, digits, sizeof(core) - pos - 1);
    core[sizeof(core) - 1] = '\0';
    pos += (int)s21_strlen(digits);
    core[pos] = '\0';

    int core_len = (int)s21_strlen(core);
    int use_zero_pad = (f.precision < 0) && f.flag_zero && !f.flag_minus;
    char padch = use_zero_pad ? '0' : ' ';

    if (padch == '0' && sign) {
        int total_pad = (f.width > core_len) ? (f.width - core_len) : 0;
        *dst++ = sign;
        while (total_pad-- > 0) *dst++ = '0';
        for (int i = 1; core[i]; ++i) *dst++ = core[i];
    } else {
        dst = s21_pad(dst, f.width, core_len, f.flag_minus, padch);
        for (int i = 0; core[i]; ++i) *dst++ = core[i];
        if (f.flag_minus) dst = s21_pad(dst, f.width, core_len, 0, ' ');
    }
    return dst;
}
static char *s21_print_uint(char *dst, unsigned long long v,
                            const s21_format f) {
    char digits[64];
    if (v == 0) {
        digits[0] = '0';
        digits[1] = '\0';
    } else {
        char tmp[64];
        int n = 0;
        while (v) {
            tmp[n++] = (char)('0' + (v % 10));
            v /= 10;
        }
        for (int i = 0; i < n; ++i) digits[i] = tmp[n - 1 - i];
        digits[n] = '\0';
    }

    if (f.precision == 0 && digits[0] == '0' && digits[1] == '\0')
        digits[0] = '\0';

    int len = (int)s21_strlen(digits);
    int need_zeros = (f.precision > len) ? (f.precision - len) : 0;

    char core[128];
    int pos = 0;
    for (int i = 0; i < need_zeros; ++i) core[pos++] = '0';
    s21_strncpy(core + pos, digits, sizeof(core) - pos - 1);
    core[sizeof(core) - 1] = '\0';
    pos += (int)s21_strlen(digits);
    core[pos] = '\0';

    int core_len = (int)s21_strlen(core);
    char padch = (!f.flag_minus && f.flag_zero && f.precision < 0) ? '0' : ' ';

    dst = s21_pad(dst, f.width, core_len, f.flag_minus, padch);
    for (int i = 0; core[i]; ++i) *dst++ = core[i];
    if (f.flag_minus) dst = s21_pad(dst, f.width, core_len, 0, ' ');

    return dst;
}
static char *s21_print_float(char *dst, double v, const s21_format f) {
    int negative = signbit(v);
    if (negative) v = -v;

    int prec = (f.precision >= 0) ? f.precision : 6;
    if (prec > 18) prec = 18;

    long long int_part = (long long)v;
    double frac_part = v - (double)int_part;

    if (prec == 0) {
        if (frac_part >= 0.5) int_part++;
    }

    long long frac_int = 0;
    if (prec > 0) {
        long long mul = 1;
        for (int i = 0; i < prec; ++i) mul *= 10;
        double shifted = frac_part * mul + 0.5;
        frac_int = (long long)shifted;
        if (frac_int >= mul) {
            frac_int = 0;
            int_part += 1;
        }
    }

    char buf_int[64], buf_frac[64];
    s21_int_to_str(int_part, buf_int);

    if (prec > 0) {
        s21_int_to_str(frac_int, buf_frac);
        int len = (int)s21_strlen(buf_frac);
        if (len < prec) {
            char tmp[64];
            int padz = prec - len;
            for (int i = 0; i < padz; ++i) tmp[i] = '0';
            s21_strncpy(tmp + padz, buf_frac, len + 1);
            s21_strncpy(buf_frac, tmp, prec + 1);
        }
    }

    char core[128];
    int pos = 0;
    char sign = 0;
    if (negative)
        sign = '-';
    else if (f.flag_plus)
        sign = '+';
    else if (f.flag_space)
        sign = ' ';
    if (sign) core[pos++] = sign;

    s21_strncpy(core + pos, buf_int, sizeof(core) - pos);
    pos += (int)s21_strlen(buf_int);

    if (prec > 0) {
        core[pos++] = '.';
        s21_strncpy(core + pos, buf_frac, sizeof(core) - pos);
        pos += (int)s21_strlen(buf_frac);
    }
    core[pos] = '\0';

    int core_len = (int)s21_strlen(core);
    int pad = (f.width > core_len) ? (f.width - core_len) : 0;

    if (!f.flag_minus && f.flag_zero && pad > 0) {
        if (sign) {
            *dst++ = sign;
            core_len--;
        }
        while (pad-- > 0) *dst++ = '0';
        const char *p = sign ? core + 1 : core;
        while (*p) *dst++ = *p++;
    } else {
        dst = s21_pad(dst, f.width, core_len, f.flag_minus, ' ');
        for (int i = 0; core[i]; ++i) *dst++ = core[i];
        if (f.flag_minus) dst = s21_pad(dst, f.width, core_len, 0, ' ');
    }
    return dst;
}

int s21_sprintf(char *out, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    char *dst = out;
    const char *src = fmt;

    while (*src) {
        if (*src != '%') {
            *dst++ = *src++;
        } else if (*++src == '%') {
            *dst++ = '%';
            ++src;
        } else {
            s21_format f = s21_parse_format(&src);
            switch (f.type) {
                case 'i': {
                    long long v = 0;
                    if (f.length == 'L')
                        v = va_arg(ap, long long);
                    else if (f.length == 'l')
                        v = (long long)va_arg(ap, long);
                    else if (f.length == 'h')
                        v = (short)va_arg(ap, int);
                    else
                        v = (long long)va_arg(ap, int);
                    dst = s21_print_int(dst, v, f);
                    break;
                }
                case 'u': {
                    unsigned long long v;
                    if (f.length == 'L')
                        v = va_arg(ap, unsigned long long);
                    else if (f.length == 'l')
                        v = (unsigned long long)va_arg(ap, unsigned long);
                    else if (f.length == 'h')
                        v = (unsigned short)va_arg(ap, unsigned int);
                    else
                        v = (unsigned long long)va_arg(ap, unsigned int);

                    dst = s21_print_uint(dst, v, f);
                    break;
                }
                case 'f': {
                    double v;
                    if (f.length == 'L') {
                        long double vl = va_arg(ap, long double);
                        v = (double)vl;
                    } else {
                        v = va_arg(ap, double);
                    }
                    dst = s21_print_float(dst, v, f);
                    break;
                }
                case 'c': {
                    int c = va_arg(ap, int);
                    dst = s21_pad(dst, f.width, 1, f.flag_minus, ' ');
                    *dst++ = (char)c;
                    if (f.flag_minus) dst = s21_pad(dst, f.width, 1, 0, ' ');
                    break;
                }
                case 's': {
                    const char *s = va_arg(ap, const char *);
                    if (!s) s = "(null)";
                    int len = (int)s21_strlen(s);
                    if (f.precision >= 0 && f.precision < len)
                        len = f.precision;
                    dst = s21_pad(dst, f.width, len, f.flag_minus, ' ');
                    for (int i = 0; i < len; ++i) *dst++ = s[i];
                    if (f.flag_minus) dst = s21_pad(dst, f.width, len, 0, ' ');
                    break;
                }
                case '%': {
                    *dst++ = '%';
                    break;
                }
                default:
                    *dst++ = '%';
                    break;
            }
        }
    }
    *dst = '\0';
    va_end(ap);
    return (int)(dst - out);
}
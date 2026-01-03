#include <ctype.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "s21_decimal.h"

int s21_from_int_to_decimal(int src, s21_decimal* dst) {
    if (!dst) return S21_CONVERT_ERROR;
    for (int i = 0; i < 4; i++) dst->bits[i] = 0;

    unsigned int mag;
    int sign = 0;

    if (src < 0) {
        sign = 1;
        mag = (unsigned int)(-(long long)src);
    } else {
        mag = (unsigned int)src;
    }

    dst->bits[0] = mag;
    if (sign) dst->bits[3] |= (1u << 31);
    return S21_OK;
}

int s21_from_float_to_decimal(float src, s21_decimal* dst) {
    if (!dst) return S21_CONVERT_ERROR;
    for (int i = 0; i < 4; i++) dst->bits[i] = 0;

    if (isnan(src) || isinf(src)) return S21_CONVERT_ERROR;
    if (src == 0.0f) {
        if (signbit(src)) dst->bits[3] |= (1u << 31);
        return S21_OK;
    }

    int sign = src < 0;
    if (sign) src = -src;

    char buf[50];
    snprintf(buf, sizeof(buf), "%.9g", src);

    long double val = strtold(buf, NULL);
    int scale = 0;

    char* dot = strchr(buf, '.');
    if (dot) {
        scale = strlen(dot + 1);
        for (int i = 0; i < scale; i++) val *= 10.0L;
    }

    val = roundl(val);

    unsigned int mag[3] = {0, 0, 0};
    unsigned long long low = (unsigned long long)fmodl(val, 0x100000000ULL);
    unsigned long long high = (unsigned long long)(val / 0x100000000ULL);

    mag[0] = (unsigned int)(low & 0xFFFFFFFFu);
    mag[1] = (unsigned int)(high & 0xFFFFFFFFu);
    mag[2] = (unsigned int)((high >> 32) & 0xFFFFFFFFu);

    dst->bits[0] = mag[0];
    dst->bits[1] = mag[1];
    dst->bits[2] = mag[2];
    dst->bits[3] = (scale << 16);
    if (sign) dst->bits[3] |= (1u << 31);

    return S21_OK;
}

int s21_from_decimal_to_int(s21_decimal src, int* dst) {
    if (!dst) return S21_CONVERT_ERROR;
    *dst = 0;

    int sign = (src.bits[3] >> 31) & 1;
    int scale = (src.bits[3] >> 16) & 0xFF;

    unsigned int mag[3] = {src.bits[0], src.bits[1], src.bits[2]};

    while (scale-- > 0) {
        unsigned long long carry = 0;
        for (int i = 2; i >= 0; i--) {
            unsigned long long cur = (carry << 32) | mag[i];
            mag[i] = (unsigned int)(cur / 10ull);
            carry = cur % 10ull;
        }
    }

    if (mag[1] || mag[2]) return S21_CONVERT_ERROR;

    unsigned int v = mag[0];
    if (!sign) {
        if (v > (unsigned int)INT_MAX) return S21_CONVERT_ERROR;
        *dst = (int)v;
    } else {
        if (v > (unsigned int)INT_MAX) return S21_CONVERT_ERROR;
        *dst = -(int)v;
    }

    return S21_OK;
}

int s21_from_decimal_to_float(s21_decimal src, float* dst) {
    if (!dst) return S21_CONVERT_ERROR;

    int sign = (src.bits[3] >> 31) & 1;
    int scale = (src.bits[3] >> 16) & 0xFF;

    long double value = 0.0L;
    value += src.bits[0];
    value += (long double)src.bits[1] * 4294967296.0L;
    value += (long double)src.bits[2] * 18446744073709551616.0L;

    while (scale-- > 0) value /= 10.0L;
    if (sign) value = -value;

    *dst = (float)value;
    return S21_OK;
}

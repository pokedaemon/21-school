#include <stdint.h>

#include "s21_decimal.h"

static void s21_copy_mantissa(const s21_decimal* dec, uint32_t dest[3]) {
    dest[0] = (uint32_t)dec->bits[0];
    dest[1] = (uint32_t)dec->bits[1];
    dest[2] = (uint32_t)dec->bits[2];
}

static int s21_compare_mantissa(const uint32_t a[3], const uint32_t b[3]) {
    for (int i = 2; i >= 0; --i) {
        if (a[i] > b[i]) {
            return 1;
        }
        if (a[i] < b[i]) {
            return -1;
        }
    }
    return 0;
}

static int s21_mantissa_multiply_by_10(uint32_t parts[3]) {
    unsigned long long carry = 0ULL;
    for (int i = 0; i < 3; ++i) {
        unsigned long long product =
            (unsigned long long)parts[i] * 10ULL + carry;
        parts[i] = (uint32_t)product;
        carry = product >> 32;
    }
    return carry != 0ULL;
}

int s21_is_equal(s21_decimal lhs, s21_decimal rhs) {
    if (s21_is_zero_units(&lhs) && s21_is_zero_units(&rhs)) {
        return 1;
    }
    int sign_lhs = s21_get_sign(&lhs);
    int sign_rhs = s21_get_sign(&rhs);
    if (sign_lhs != sign_rhs) {
        return 0;
    }
    int scale_lhs = s21_get_scale(&lhs);
    int scale_rhs = s21_get_scale(&rhs);
    uint32_t mant_lhs[3];
    uint32_t mant_rhs[3];
    s21_copy_mantissa(&lhs, mant_lhs);
    s21_copy_mantissa(&rhs, mant_rhs);
    if (scale_lhs < scale_rhs) {
        int diff = scale_rhs - scale_lhs;
        for (int i = 0; i < diff; ++i) {
            if (s21_mantissa_multiply_by_10(mant_lhs)) {
                return 0;
            }
        }
    } else if (scale_rhs < scale_lhs) {
        int diff = scale_lhs - scale_rhs;
        for (int i = 0; i < diff; ++i) {
            if (s21_mantissa_multiply_by_10(mant_rhs)) {
                return 0;
            }
        }
    }
    return s21_compare_mantissa(mant_lhs, mant_rhs) == 0;
}

int s21_is_less(s21_decimal lhs, s21_decimal rhs) {
    if (s21_is_equal(lhs, rhs)) {
        return 0;
    }
    int sign_lhs = s21_get_sign(&lhs);
    int sign_rhs = s21_get_sign(&rhs);
    if (sign_lhs != sign_rhs) {
        return sign_lhs > sign_rhs;
    }
    int scale_lhs = s21_get_scale(&lhs);
    int scale_rhs = s21_get_scale(&rhs);
    uint32_t mant_lhs[3];
    uint32_t mant_rhs[3];
    s21_copy_mantissa(&lhs, mant_lhs);
    s21_copy_mantissa(&rhs, mant_rhs);
    int overflow_lhs = 0;
    int overflow_rhs = 0;
    if (scale_lhs < scale_rhs) {
        int diff = scale_rhs - scale_lhs;
        for (int i = 0; i < diff; ++i) {
            if (s21_mantissa_multiply_by_10(mant_lhs)) {
                overflow_lhs = 1;
                break;
            }
        }
    } else if (scale_rhs < scale_lhs) {
        int diff = scale_lhs - scale_rhs;
        for (int i = 0; i < diff; ++i) {
            if (s21_mantissa_multiply_by_10(mant_rhs)) {
                overflow_rhs = 1;
                break;
            }
        }
    }
    if (overflow_lhs || overflow_rhs) {
        if (overflow_lhs && !overflow_rhs) {
            return sign_lhs == 1;
        }
        if (overflow_rhs && !overflow_lhs) {
            return sign_lhs == 0;
        }
    }
    int cmp = s21_compare_mantissa(mant_lhs, mant_rhs);
    if (sign_lhs == 0) {
        return cmp < 0;
    }
    return cmp > 0;
}

int s21_is_greater(s21_decimal lhs, s21_decimal rhs) {
    return !s21_is_less(lhs, rhs) && !s21_is_equal(lhs, rhs);
}

int s21_is_not_equal(s21_decimal lhs, s21_decimal rhs) {
    return !s21_is_equal(lhs, rhs);
}

int s21_is_less_or_equal(s21_decimal lhs, s21_decimal rhs) {
    return s21_is_less(lhs, rhs) || s21_is_equal(lhs, rhs);
}

int s21_is_greater_or_equal(s21_decimal lhs, s21_decimal rhs) {
    return s21_is_greater(lhs, rhs) || s21_is_equal(lhs, rhs);
}

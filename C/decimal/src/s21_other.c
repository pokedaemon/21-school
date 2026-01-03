#include "s21_decimal.h"

int s21_floor(s21_decimal value, s21_decimal* result) {
    if (result != NULL) {
        memset(result, 0, sizeof(s21_decimal));
        s21_decimal one = {{1}};
        s21_decimal mod_res = {0};
        int sign = s21_get_sign(&value);
        s21_delete_sign(&value);
        s21_mod(value, one, &mod_res);
        s21_truncate(value, &value);
        if (sign && !s21_is_zero_units(&mod_res) &&
            !s21_is_zero_units(&value)) {
            s21_sum(value, one, result);
            s21_set_sign(result, 1);
        } else {
            *result = value;
            if (sign) {
                s21_set_sign(result, 1);
            }
        }
    } else {
        return S21_CONVERT_ERROR;
    }
    return S21_OK;
}

int s21_round(s21_decimal value, s21_decimal* result) {
    if (result == NULL) {
        return S21_CONVERT_ERROR;
    }
    memset(result, 0, sizeof(s21_decimal));
    s21_decimal five = {{5}};
    s21_decimal one = {{1}};
    s21_decimal mod_result = {0};
    int sign = s21_get_sign(&value);
    s21_delete_sign(&value);
    s21_set_power(&five, 1);
    s21_truncate(value, result);
    s21_mod(value, one, &mod_result);
    if (!s21_is_less(mod_result, five)) {
        s21_sum(*result, one, result);
    }
    if (sign) {
        s21_set_sign(result, 1);
    }
    return S21_OK;
}

int s21_truncate(s21_decimal value, s21_decimal* result) {
    s21_decimal ten = {{10}};
    int power = s21_get_power(value);
    int sign = s21_get_sign(&value);
    s21_delete_sign(&value);
    s21_set_power(&value, 0);
    if (power) {
        while (power) {
            memset(result, 0, sizeof(s21_decimal));
            result->bits[0] = 1;
            s21_int_div(value, ten, result);
            value = *result;
            power--;
        }
    } else {
        *result = value;
    }
    if (sign) {
        s21_set_sign(result, 1);
    }
    return S21_OK;
}

int s21_negate(s21_decimal value, s21_decimal* result) {
    result->bits[0] = value.bits[0];
    result->bits[1] = value.bits[1];
    result->bits[2] = value.bits[2];
    result->bits[3] = value.bits[3];

    if (!s21_get_sign(&value)) {
        s21_set_sign(result, 1);
    } else {
        s21_delete_sign(result);
    }

    return S21_OK;
}

int count_digits(long double v) {
    int cnt = 0;
    unsigned long long x = (unsigned long long)v;
    do {
        cnt++;
        x /= 10;
    } while (x);
    return cnt;
}

int mul_96(unsigned int m[3], int mul) {
    unsigned long long carry = 0;
    for (int i = 0; i < 3; i++) {
        unsigned long long cur = (unsigned long long)m[i] * mul + carry;
        m[i] = (unsigned int)(cur & 0xFFFFFFFFu);
        carry = cur >> 32;
    }
    return carry != 0;
}

int add_96(unsigned int m[3], int add) {
    unsigned long long cur = (unsigned long long)m[0] + add;
    m[0] = (unsigned int)(cur & 0xFFFFFFFFu);
    unsigned long long carry = cur >> 32;

    for (int i = 1; i < 3 && carry; i++) {
        cur = (unsigned long long)m[i] + carry;
        m[i] = (unsigned int)(cur & 0xFFFFFFFFu);
        carry = cur >> 32;
    }
    return carry != 0;
}
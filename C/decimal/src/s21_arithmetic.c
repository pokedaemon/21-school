#include "s21_decimal.h"

static s21_decimal s21_binary_mod(s21_decimal value_1, s21_decimal value_2) {
    s21_decimal result_sub = {0};
    s21_decimal result = {0};
    s21_decimal temp_value_2 = value_2;
    if (s21_is_less(value_1, value_2)) {
        return value_1;
    } else if (s21_is_equal(value_1, value_2)) {
        return result_sub;
    }
    while (s21_find_first_one_bit(value_1) >
           s21_find_first_one_bit(temp_value_2)) {
        s21_lshift(&temp_value_2);
    }
    if (s21_is_less(value_1, temp_value_2)) {
        s21_rshift(&temp_value_2);
    }
    s21_sub(value_1, temp_value_2, &result_sub);
    result = s21_binary_mod(result_sub, value_2);
    return result;
}

int s21_mod(s21_decimal value_1, s21_decimal value_2, s21_decimal* result) {
    memset(result, 0, sizeof(s21_decimal));
    if (s21_is_zero_units(&value_2)) {
        return S21_NAN;
    }
    int sign = s21_get_sign(&value_1);
    s21_delete_sign(&value_1);
    s21_delete_sign(&value_2);
    int pow_1 = s21_get_power(value_1);
    int pow_2 = s21_get_power(value_2);
    int is_over = s21_normalization(&value_1, &value_2);
    if (is_over && s21_is_zero_units(&value_2)) {
        return S21_OK;
    }
    s21_set_power(&value_1, 0);
    s21_set_power(&value_2, 0);
    *result = s21_binary_mod(value_1, value_2);
    if (sign) {
        s21_set_sign(result, 1);
    }
    int max = pow_1 > pow_2 ? pow_1 : pow_2;
    s21_set_power(result, max);
    if (is_over) {
        s21_truncate(*result, result);
    }
    return S21_OK;
}

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal* result) {
    int return_value = S21_OK;
    memset(result, 0, sizeof(s21_decimal));
    int is_over;

    int sign_1 = s21_get_sign(&value_1);
    int sign_2 = s21_get_sign(&value_2);

    sign_1 ? s21_delete_sign(&value_1) : 0;
    sign_2 ? s21_delete_sign(&value_2) : 0;
    int pow_1 = s21_get_power(value_1);
    int pow_2 = s21_get_power(value_2);
    s21_normalization(&value_1, &value_2);
    if (sign_1 && sign_2) {
        is_over = s21_sum(value_1, value_2, result);
        if (is_over) {
            return S21_NEGATIVE_INF_REPORT;
        }
        s21_set_sign(result, 1);
    } else if (!sign_1 && !sign_2) {
        is_over = s21_sum(value_1, value_2, result);
        if (is_over) {
            return S21_POSITIVE_INF_REPORT;
        }
    } else if (!sign_1 && sign_2) {
        s21_sub(value_1, value_2, result);
        if (s21_comparison(value_1, value_2) == VAIUE2_GREATER) {
            s21_set_sign(result, 1);
        }
    } else if (sign_1 && !sign_2) {
        s21_sub(value_1, value_2, result);
        if (s21_comparison(value_1, value_2) == VAIUE1_GREATER) {
            s21_set_sign(result, 1);
        }
    }
    int max_pow = pow_1 > pow_2 ? pow_1 : pow_2;
    s21_set_power(result, max_pow);

    return return_value;
}

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal* result) {
    int return_value = S21_OK;
    memset(result, 0, sizeof(s21_decimal));

    int sign_1 = s21_get_sign(&value_1);
    int sign_2 = s21_get_sign(&value_2);

    sign_1 ? s21_delete_sign(&value_1) : 0;
    sign_2 ? s21_delete_sign(&value_2) : 0;
    int pow_1 = s21_get_power(value_1);
    int pow_2 = s21_get_power(value_2);

    s21_normalization(&value_1, &value_2);
    s21_decimal add_code_1;
    s21_decimal add_code_2;

    if (!sign_1 && !sign_2) {
        if (s21_comparison(value_1, value_2) == VAIUE2_GREATER) {
            add_code_1 = s21_add_code(value_1);
            s21_sum(value_2, add_code_1, result);
            s21_set_sign(result, 1);
        } else {
            add_code_2 = s21_add_code(value_2);
            s21_sum(value_1, add_code_2, result);
        }
    } else if (sign_1 && sign_2) {
        if (s21_comparison(value_1, value_2) == VAIUE1_GREATER) {
            add_code_2 = s21_add_code(value_2);
            s21_sum(add_code_2, value_1, result);
            s21_set_sign(result, 1);
        } else {
            add_code_1 = s21_add_code(value_1);
            s21_sum(add_code_1, value_2, result);
        }
    } else if (!sign_1 && sign_2) {
        s21_sum(value_1, value_2, result);

    } else if (sign_1 && !sign_2) {
        int is_over = s21_sum(value_1, value_2, result);
        if (is_over) {
            return S21_NEGATIVE_INF_REPORT;
        }
        s21_set_sign(result, 1);
    }
    int max_pow = pow_1 > pow_2 ? pow_1 : pow_2;
    s21_set_power(result, max_pow);

    return return_value;
}

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal* result) {
    int result_value = S21_OK;
    memset(result, 0, sizeof(s21_decimal));

    int sign_1 = s21_get_sign(&value_1);
    int sign_2 = s21_get_sign(&value_2);

    int need_sign = (!sign_1 && sign_2) || (sign_1 && !sign_2);
    s21_delete_sign(&value_1);
    s21_delete_sign(&value_2);
    int pow1 = s21_get_power(value_1);
    int pow2 = s21_get_power(value_2);
    s21_set_power(&value_1, 0);
    s21_set_power(&value_2, 0);
    s21_decimal temp_result = {0};

    int is_over = s21_multiply(value_1, value_2, &temp_result);
    int count = 0;

    if (is_over && (pow1 + pow2) > 0) {
        s21_decimal ten = {{10}};
        while (is_over && count < (pow1 + pow2)) {
            s21_decimal div_res = {{1}};
            s21_decimal delim = count % 2 ? value_2 : value_1;
            s21_int_div(delim, ten, &div_res);
            if (count % 2) {
                value_2 = div_res;
            } else {
                value_1 = div_res;
            }
            is_over = s21_multiply(value_1, value_2, &temp_result);
            count++;
        }
    }
    if (is_over && need_sign) {
        return S21_NEGATIVE_INF_REPORT;
    } else if (is_over && !need_sign) {
        return S21_POSITIVE_INF_REPORT;
    }
    *result = temp_result;
    s21_set_power(result, pow1 + pow2 - count);
    if (need_sign) {
        s21_set_sign(result, 1);
    }
    return result_value;
}

static s21_decimal s21_binary_div(s21_decimal value_1, s21_decimal value_2,
                                  s21_decimal* result) {
    static unsigned int pre_digit = 0;
    s21_decimal temp_result = {{1}};
    s21_decimal result_sub = {0};
    s21_decimal one = {{1}};
    s21_decimal temp_value_2 = value_2;
    if (s21_is_less(value_1, value_2)) {
        s21_decimal ten = {{10}};
        if (!s21_is_zero_units(&value_1)) {
            int is_over = 0;
            int power = s21_get_power(*result);
            if (power == 28) {
                is_over = 1;
            } else {
                is_over = s21_multiply(*result, ten, result);
            }
            power += 1;
            s21_multiply(value_1, ten, &value_1);
            if (is_over) {
                s21_int_div(value_1, value_2, &temp_result);
                s21_decimal pre_digit_dec = {{pre_digit}};
                *result =
                    s21_base_bank_round(temp_result, pre_digit_dec, *result);
                return result_sub;
            }
            s21_set_power(result, power);
            pre_digit = 0;
            s21_binary_div(value_1, value_2, result);
        }
        return result_sub;
    } else if (s21_is_equal(value_1, value_2)) {
        s21_sum(*result, one, result);
        return one;
    }
    while (s21_find_first_one_bit(value_1) >
           s21_find_first_one_bit(temp_value_2)) {
        s21_lshift(&temp_value_2);
        s21_lshift(&temp_result);
    }
    if (s21_is_less(value_1, temp_value_2)) {
        s21_rshift(&temp_value_2);
        s21_rshift(&temp_result);
    }
    s21_sum(*result, temp_result, result);
    s21_sub(value_1, temp_value_2, &result_sub);
    pre_digit += temp_result.bits[0];
    s21_binary_div(result_sub, value_2, result);
    return *result;
}

static s21_decimal s21_normalize_after_div(s21_decimal result_dif, int power_1,
                                           int power_2) {
    int difference;
    s21_decimal ten = {{10}};
    if (power_1 > power_2) {
        difference = power_1 - power_2;
        int result_power = s21_get_power(result_dif) + difference;
        if (result_power > 28) {
            result_power = 28;
        }
        s21_set_power(&result_dif, result_power);
    } else if (power_1 < power_2) {
        difference = power_2 - power_1;
        int result_power = s21_get_power(result_dif) - difference;
        result_power = result_power < 0 ? 0 : result_power;
        s21_set_power(&result_dif, 0);
        while (difference) {
            s21_multiply(result_dif, ten, &result_dif);
            difference--;
        }
        s21_set_power(&result_dif, result_power);
    }
    return result_dif;
}

int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal* result) {
    int result_value = S21_OK;
    memset(result, 0, sizeof(s21_decimal));

    int sign_1 = s21_get_sign(&value_1);
    int sign_2 = s21_get_sign(&value_2);
    int result_sign = 0;
    s21_decimal one = {{1}};

    s21_delete_sign(&value_1);
    s21_delete_sign(&value_2);
    int pow1 = s21_get_power(value_1);
    int pow2 = s21_get_power(value_2);

    if ((!sign_1 && sign_2) || (sign_1 && !sign_2)) {
        result_sign = 1;
    }

    if (s21_is_zero_units(&value_2)) {
        return S21_DIV_BY_ZERO;
    } else if (s21_is_equal(value_2, one)) {
        *result = value_1;
        result_sign ? s21_set_sign(result, 1) : 0;
        return result_value;
    }
    int is_over = s21_check_over(value_1, value_2);
    if (is_over) {
        if ((!sign_1 && !sign_2) || (sign_1 && sign_2)) {
            return S21_POSITIVE_INF_REPORT;
        } else if ((!sign_1 && sign_2) || (sign_1 && !sign_2)) {
            return S21_NEGATIVE_INF_REPORT;
        }
    }
    s21_set_power(&value_1, 0);
    s21_set_power(&value_2, 0);
    pow1 > pow2 ? s21_set_power(result, pow1) : 0;
    s21_binary_div(value_1, value_2, result);
    *result = s21_normalize_after_div(*result, pow1, pow2);

    if (result_sign) {
        s21_set_sign(result, 1);
    }
    return result_value;
}

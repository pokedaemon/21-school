#include <stdio.h>

#include "s21_decimal.h"

int s21_get_bit(s21_decimal number, int position) {
    return (number.bits[position / 32] >> (position % 32)) & 1U;
}

void s21_set_bit(s21_decimal* number, int position) {
    int mask = 1U << (position % 32);
    number->bits[position / 32] |= mask;
}

void s21_delete_sign(s21_decimal* number) {
    int mask = ~(1U << SIGN);
    number->bits[3] &= mask;
}

int s21_lshift(s21_decimal* value) {
    int is_over = s21_get_bit(*value, 95);
    value->bits[2] <<= 1U;
    if (s21_get_bit(*value, 63)) {
        s21_set_bit(value, 64);
    }
    value->bits[1] <<= 1U;
    if (s21_get_bit(*value, 31)) {
        s21_set_bit(value, 32);
    }
    value->bits[0] <<= 1U;
    return is_over;
}

void s21_rshift(s21_decimal* value) {
    value->bits[0] >>= 1U;
    if (s21_get_bit(*value, 32)) {
        s21_set_bit(value, 31);
    }
    value->bits[1] >>= 1U;
    if (s21_get_bit(*value, 64)) {
        s21_set_bit(value, 63);
    }
    value->bits[2] >>= 1U;
}

s21_decimal s21_inversion(s21_decimal number) {
    s21_decimal result;
    result.bits[0] = ~number.bits[0];
    result.bits[1] = ~number.bits[1];
    result.bits[2] = ~number.bits[2];
    return result;
}

int s21_comparison(s21_decimal value_1, s21_decimal value_2) {
    int result = EQUAL;
    for (int i = 95; i >= 0; i--) {
        if (s21_get_bit(value_1, i) > s21_get_bit(value_2, i)) {
            result = VAIUE1_GREATER;
            break;
        } else if (s21_get_bit(value_1, i) < s21_get_bit(value_2, i)) {
            result = VAIUE2_GREATER;
            break;
        }
    }
    return result;
}

int s21_get_power(s21_decimal value) {
    int power = 0;
    int result = 0;
    for (int i = 0; i < END_POWER - BEGIN_POWER; i++) {
        int temp = s21_get_bit(value, 96 + i + BEGIN_POWER);
        power = (1U << i) * temp;
        result += power;
    }
    return result;
}

static void s21_delete_power(s21_decimal* value) {
    int sign = s21_get_sign(value);
    value->bits[3] = 0;
    if (sign) {
        s21_set_sign(value, 1);
    }
}

void s21_set_power(s21_decimal* value, int pow) {
    s21_delete_power(value);
    for (int i = BEGIN_POWER; i <= END_POWER && pow; i++) {
        if (pow % 2 == 1) {
            s21_set_bit(value, 96 + i);
        }
        pow /= 2;
    }
}

int s21_count_shift(s21_decimal* value, int count) {
    int is_over = 0;
    for (int i = 0; i < count; i++) {
        is_over = s21_lshift(value);
        if (is_over) {
            break;
        }
    }
    return is_over;
}

int s21_find_first_one_bit(s21_decimal src) {
    int position = -1;
    for (int i = 95; i >= 0; i--) {
        if (s21_get_bit(src, i)) {
            position = i;
            break;
        }
    }
    return position;
}

void s21_copy_decimal(s21_decimal* cpy, s21_decimal* dst) {
    memcpy((void*)dst, (void*)cpy, sizeof(s21_decimal));
}

int s21_get_sign(const s21_decimal* val) { return val->bits[3] >> 31; }

void s21_set_sign(s21_decimal* val, int sign) {
    if (sign)
        val->bits[3] |= (1u << 31);
    else
        val->bits[3] &= ~(1u << 31);
}

int s21_get_scale(const s21_decimal* val) {
    return (val->bits[3] >> 16) & 0xFF;
}

void s21_set_scale(s21_decimal* val, int scale) {
    val->bits[3] &= ~(0xFF << 16);
    val->bits[3] |= ((uint32_t)scale & 0xFFu) << 16;
}

void s21_set_zero_units(s21_decimal* val) {
    val->bits[0] = 0;
    val->bits[1] = 0;
    val->bits[2] = 0;
}

int s21_sum(s21_decimal value_1, s21_decimal value_2, s21_decimal* result) {
    int result_value = S21_OK;
    int temp = 0;
    s21_decimal temp_res = {0};
    for (int i = 0; i < 96; i++) {
        int value1_bit = s21_get_bit(value_1, i);
        int value2_bit = s21_get_bit(value_2, i);
        int compare = value1_bit || value2_bit;
        if (value1_bit && value2_bit) {
            if (temp) {
                s21_set_bit(&temp_res, i);
            }
            temp = 1;
        }
        if (!temp && compare) {
            s21_set_bit(&temp_res, i);
        } else if (temp && !compare) {
            s21_set_bit(&temp_res, i);
            temp = 0;
        }
        if (i == 95 && temp == 1) {
            result_value = 1;
        }
    }

    s21_set_power(&temp_res, s21_get_power(*result));
    *result = temp_res;
    return result_value;
}

void s21_plus_one(s21_decimal value, s21_decimal* result) {
    s21_decimal one = {0};
    one.bits[0] = 1;
    s21_sum(value, one, result);
}

s21_decimal s21_add_code(s21_decimal value) {
    s21_decimal result = {0};
    s21_decimal add_code = s21_inversion(value);
    s21_plus_one(add_code, &result);
    return result;
}

int s21_multiply(s21_decimal value_1, s21_decimal value_2,
                 s21_decimal* result) {
    int is_over = S21_OK;
    int sum_over = S21_OK;
    s21_decimal temp = {0};
    s21_decimal result_tmp = {0};
    int count = 0;
    while (!s21_is_zero_units(&value_2)) {
        memset(&temp, 0, sizeof(s21_decimal));
        int bit_2 = s21_get_bit(value_2, 0);
        for (int i = 0; i < 96; i++) {
            int bit_1 = s21_get_bit(value_1, i);
            if (bit_1 && bit_2) {
                s21_set_bit(&temp, i);
            }
        }
        is_over = s21_count_shift(&temp, count);
        sum_over = s21_sum(result_tmp, temp, &result_tmp);
        s21_rshift(&value_2);
        if (is_over || sum_over) {
            break;
        }
        count++;
    }
    if (!is_over && !sum_over) {
        *result = result_tmp;
    }
    return is_over || sum_over;
}

int s21_is_zero_units(s21_decimal* val) {
    return val->bits[0] == 0 && val->bits[1] == 0 && val->bits[2] == 0;
}

s21_decimal s21_int_div(s21_decimal value_1, s21_decimal value_2,
                        s21_decimal* result) {
    s21_decimal result_sub = {0};
    s21_decimal one = {{1}};
    s21_decimal temp_value_2 = value_2;
    if (s21_is_less(value_1, value_2)) {
        *result = result_sub;
        return result_sub;
    } else if (s21_is_equal(value_1, value_2)) {
        return one;
    }
    while (s21_find_first_one_bit(value_1) >
           s21_find_first_one_bit(temp_value_2)) {
        s21_lshift(&temp_value_2);
        s21_lshift(result);
    }
    if (s21_is_less(value_1, temp_value_2)) {
        s21_rshift(&temp_value_2);
        s21_rshift(result);
    }
    s21_sub(value_1, temp_value_2, &result_sub);
    one = s21_int_div(result_sub, value_2, &one);
    s21_sum(*result, one, result);
    return *result;
}

s21_decimal s21_get_pre_digit(s21_decimal value) {
    s21_decimal ten = {{10}};
    s21_decimal div_result = {{1}};
    s21_decimal pre_digit = {0};
    s21_int_div(value, ten, &div_result);
    s21_mod(div_result, ten, &pre_digit);
    return pre_digit;
}

s21_decimal s21_base_bank_round(s21_decimal last_decimal, s21_decimal pre_digit,
                                s21_decimal result) {
    s21_decimal two = {{2}};
    s21_decimal five = {{5}};
    s21_decimal is_even = {0};
    if (s21_is_equal(last_decimal, five)) {
        s21_mod(pre_digit, two, &is_even);
        if (!s21_is_zero_units(&is_even)) {
            s21_plus_one(result, &result);
        }
    } else if (s21_is_less(five, last_decimal)) {
        s21_plus_one(result, &result);
    }
    return result;
}

s21_decimal s21_bank_rounding(s21_decimal value) {
    s21_decimal ten = {{10}};
    s21_decimal last_decimal = {0};
    s21_decimal result = {{1}};

    s21_mod(value, ten, &last_decimal);
    s21_decimal pre_digit = s21_get_pre_digit(value);
    s21_int_div(value, ten, &result);

    return s21_base_bank_round(last_decimal, pre_digit, result);
}

int s21_normalization(s21_decimal* value_1, s21_decimal* value_2) {
    int check_over = S21_OK;
    int power_1 = s21_get_power(*value_1);
    int power_2 = s21_get_power(*value_2);
    s21_set_power(value_1, 0);
    s21_set_power(value_2, 0);

    int biggest_power = power_1 > power_2 ? power_1 : power_2;
    int min_power = power_1 < power_2 ? power_1 : power_2;
    s21_decimal num_normalize = power_1 < power_2 ? *value_1 : *value_2;
    s21_decimal small = power_1 < power_2 ? *value_2 : *value_1;
    s21_decimal result = {0};
    s21_decimal ten = {{10}};

    while (min_power != biggest_power) {
        check_over = s21_multiply(num_normalize, ten, &result);
        if (check_over) {
            break;
        }
        min_power++;
        num_normalize = result;
        memset(&result, 0, sizeof(s21_decimal));
    }
    if (check_over) {
        while (min_power < biggest_power) {
            small = s21_bank_rounding(small);
            min_power++;
        }
    }
    if (power_1 < power_2) {
        *value_1 = num_normalize;
        *value_2 = small;
    } else {
        *value_2 = num_normalize;
        *value_1 = small;
    }
    s21_set_power(value_1, biggest_power);
    s21_set_power(value_2, biggest_power);
    return check_over;
}

int s21_check_over(s21_decimal value_1, s21_decimal value_2) {
    int check_over = S21_OK;
    int power_1 = s21_get_power(value_1);
    int power_2 = s21_get_power(value_2);
    s21_set_power(&value_1, 0);
    s21_set_power(&value_2, 0);

    int biggest_power = power_1 > power_2 ? power_1 : power_2;
    int min_power = power_1 < power_2 ? power_1 : power_2;
    s21_decimal num_normalize = power_1 < power_2 ? value_1 : value_2;
    s21_decimal small = power_1 < power_2 ? value_2 : value_1;
    s21_decimal result = {0};
    s21_decimal ten = {{10}};

    while (min_power != biggest_power) {
        check_over = s21_multiply(num_normalize, ten, &result);
        if (check_over) {
            break;
        }
        min_power++;
        num_normalize = result;
        memset(&result, 0, sizeof(s21_decimal));
    }

    if (check_over) {
        while (min_power < biggest_power) {
            small = s21_bank_rounding(small);
            min_power++;
        }
        if (s21_is_zero_units(&small)) {
            return check_over;
        }
    }
    return S21_OK;
}

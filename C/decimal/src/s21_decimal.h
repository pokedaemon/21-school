#ifndef S21_DECIMAL_H
#define S21_DECIMAL_H

#include <stdint.h>
#include <string.h>

typedef struct s21_decimal {
    uint32_t bits[4];
} s21_decimal;

/*
    Arithmetic
*/

#define S21_POSITIVE_INF 1.0 / 0.0
#define S21_NEGATIVE_INF -1.0 / 0.0

#define MASK 0x80000000
#define EXP_MASK 0x800000
#define MAN_MASK 0x400000

#define S21_OK (0)
#define S21_POSITIVE_INF_REPORT (1)
#define S21_NEGATIVE_INF_REPORT (2)
#define S21_DIV_BY_ZERO (3)

#define S21_NAN (1)

enum { EQUAL, VAIUE1_GREATER, VAIUE2_GREATER };
enum { BEGIN_POWER = 16, END_POWER = 23, SIGN = 31 };

int s21_add(s21_decimal lhs, s21_decimal rhs, s21_decimal* res);
int s21_sub(s21_decimal lhs, s21_decimal rhs, s21_decimal* res);
int s21_mul(s21_decimal lhs, s21_decimal rhs, s21_decimal* res);
int s21_div(s21_decimal lhs, s21_decimal rhs, s21_decimal* res);

/*
    Compare
*/

#define S21_TRUE (1)
#define S21_FALSE (0)

int s21_is_less(s21_decimal lhs, s21_decimal rhs);
int s21_is_less_or_equal(s21_decimal lhs, s21_decimal rhs);
int s21_is_greater(s21_decimal lhs, s21_decimal rhs);
int s21_is_greater_or_equal(s21_decimal lhs, s21_decimal rhs);
int s21_is_equal(s21_decimal lhs, s21_decimal rhs);
int s21_is_not_equal(s21_decimal lhs, s21_decimal rhs);

/*
    Converting
*/

#define S21_CONVERT_ERROR (1)

int s21_from_int_to_decimal(int src, s21_decimal* dst);
int s21_from_float_to_decimal(float src, s21_decimal* dst);
int s21_from_decimal_to_int(s21_decimal src, int* dst);
int s21_from_decimal_to_float(s21_decimal src, float* dst);

/*
    Other
*/

#define S21_CALCULATION_ERROR (1)

int s21_floor(s21_decimal value, s21_decimal* res);
int s21_round(s21_decimal value, s21_decimal* res);
int s21_truncate(s21_decimal value, s21_decimal* res);
int s21_negate(s21_decimal value, s21_decimal* res);

/*
    Utility
 */

void s21_copy_decimal(s21_decimal* cpy, s21_decimal* dst);
int s21_get_sign(const s21_decimal* val);
void s21_set_sign(s21_decimal* val, int sign);
int s21_get_scale(const s21_decimal* val);
void s21_set_scale(s21_decimal* val, int scale);
void s21_set_zero_units(s21_decimal* val);
int s21_is_zero_units(s21_decimal* val);
void s21_delete_sign(s21_decimal* number);
s21_decimal s21_add_code(s21_decimal value);
int s21_get_power(s21_decimal value);
void s21_set_power(s21_decimal* value, int pow);
void s21_rshift(s21_decimal* value);
int s21_lshift(s21_decimal* value);
int s21_mod(s21_decimal value_1, s21_decimal value_2, s21_decimal* result);
int s21_find_first_one_bit(s21_decimal src);
int s21_sum(s21_decimal value_1, s21_decimal value_2, s21_decimal* result);
s21_decimal s21_bank_rounding(s21_decimal value);
int s21_normalization(s21_decimal* value_1, s21_decimal* value_2);
int s21_comparison(s21_decimal value_1, s21_decimal value_2);
s21_decimal s21_int_div(s21_decimal value_1, s21_decimal value_2,
                        s21_decimal* result);
int s21_multiply(s21_decimal value_1, s21_decimal value_2, s21_decimal* result);
int s21_check_over(s21_decimal value_1, s21_decimal value_2);
s21_decimal s21_base_bank_round(s21_decimal last_decimal, s21_decimal pre_digit,
                                s21_decimal result);

#endif  // S21_DECIMAL_H

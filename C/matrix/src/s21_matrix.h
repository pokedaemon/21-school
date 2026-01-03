#ifndef S21_MATRIX_H
#define S21_MATRIX_H

#define S21_OK 0
/* Incorrect matrix */
#define S21_ERROR 1
/* Mismatched matrix sizes, matrix for which calculations cannot be performed,
 * etc. */
#define S21_CALCULATION_ERROR 2

#define SUCCESS 1
#define FAILURE 0

typedef struct matrix_struct {
    double** matrix;  // fcking data
    int rows;
    int columns;
} matrix_t;

int s21_create_matrix(int rows, int columns, matrix_t* result);
int s21_eq_matrix(matrix_t* A, matrix_t* B);
int s21_sum_matrix(matrix_t* A, matrix_t* B, matrix_t* result);
int s21_sub_matrix(matrix_t* A, matrix_t* B, matrix_t* result);
int s21_mult_number(matrix_t* A, double number, matrix_t* result);
int s21_mult_matrix(matrix_t* A, matrix_t* B, matrix_t* result);
int s21_transpose(matrix_t* A, matrix_t* result);
int s21_calc_complements(matrix_t* A, matrix_t* result);
int s21_determinant(matrix_t* A, double* result);
int s21_inverse_matrix(matrix_t* A, matrix_t* result);

/* No errors */
void s21_remove_matrix(matrix_t* A);

void s21_fill_determinate_matrix(double** matrix_input, double** matrix_temp,
                                 int skip_row, int skip_column, int size);
double s21_found_determinant(matrix_t A, int size);

#endif
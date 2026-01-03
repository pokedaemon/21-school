#include "s21_matrix.h"

// TODO: allocate memory for result in each function where it needed

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define S21_EPSILON 1E-6

static int s21_eq_sizes_matrix(matrix_t* lhs, matrix_t* rhs) {
    if (!lhs || !rhs) return (int)false;
    return lhs->columns == rhs->columns && lhs->rows == rhs->rows;
}

static int s21_eq_double(double lhs, double rhs) {
    return (int)(fabs(lhs - rhs) <= S21_EPSILON);
}

static bool s21_is_matrix_exists(matrix_t* A) {
    return (A && A->rows > 0 && A->columns > 0 && A->matrix);
}

int s21_create_matrix(int rows, int columns, matrix_t* result) {
    int res = S21_OK;
    // if result is already allocated -> free memory and allocate again
    // sure we can optimize if rows n cols equals rows n cols of result then
    // don't allocate but i don't care.just free
    if (rows <= 0 || columns <= 0) res = S21_ERROR;
    if (result->matrix && res != S21_ERROR) s21_remove_matrix(result);

    if (res == S21_OK) {
        // OPTI: can allocate with one call, not 1 + rows calls
        double** data = (double**)malloc(sizeof(double*) * rows);
        if (!data) return S21_ERROR;

        // TODO: if rows or columns are negative return failure
        for (ptrdiff_t i = 0; i < rows; i++) {
            // TODO: S21_ERROR if malloc return NULL
            data[i] = (double*)calloc(columns, sizeof(double));
        }

        result->matrix = data;
        result->columns = columns;
        result->rows = rows;
    }

    return res;
}

int s21_eq_matrix(matrix_t* A, matrix_t* B) {
    int res = SUCCESS;
    if (!s21_eq_sizes_matrix(A, B)) res = FAILURE;
    if (!s21_is_matrix_exists(A) || !(s21_is_matrix_exists(B))) res = FAILURE;

    if (res == SUCCESS) {
        int rows = A->rows;
        int cols = A->columns;
        for (ptrdiff_t i = 0; i < rows; i++) {
            for (ptrdiff_t j = 0; j < cols; j++) {
                if (!s21_eq_double(A->matrix[i][j], B->matrix[i][j])) {
                    res = FAILURE;
                    break;  // YES it's break for optimization plz don't fuck my
                            // brains about 7 rules
                }
            }
        }
    }

    return res;
}

int s21_sum_matrix(matrix_t* A, matrix_t* B, matrix_t* result) {
    int res = S21_OK;
    if (!s21_eq_sizes_matrix(A, B)) res = S21_CALCULATION_ERROR;
    if (!result || !A || !B || !s21_is_matrix_exists(A) ||
        !s21_is_matrix_exists(B))
        res = S21_ERROR;

    if (res == S21_OK) {
        int rows = A->rows;
        int cols = A->columns;
        if (s21_create_matrix(rows, cols, result) == S21_ERROR) {
            res = S21_ERROR;
            return res;
        }

        for (ptrdiff_t i = 0; i < rows; i++) {
            for (ptrdiff_t j = 0; j < cols; j++) {
                result->matrix[i][j] = A->matrix[i][j] + B->matrix[i][j];
            }
        }
    }

    return res;
}

int s21_sub_matrix(matrix_t* A, matrix_t* B, matrix_t* result) {
    int res = S21_OK;
    if (!s21_eq_sizes_matrix(A, B)) res = S21_CALCULATION_ERROR;
    if (!result || !A || !B || !s21_is_matrix_exists(A) ||
        !s21_is_matrix_exists(B))
        res = S21_ERROR;

    if (res == S21_OK) {
        int rows = A->rows;
        int cols = A->columns;
        if (s21_create_matrix(rows, cols, result) == S21_ERROR) {
            res = S21_ERROR;
            return res;
        }

        for (ptrdiff_t i = 0; i < rows; i++) {
            for (ptrdiff_t j = 0; j < cols; j++) {
                result->matrix[i][j] = A->matrix[i][j] - B->matrix[i][j];
            }
        }
    }

    return res;
}

int s21_mult_number(matrix_t* A, double number, matrix_t* result) {
    int res = S21_OK;
    if (!A || !result) res = S21_ERROR;

    if (res == S21_OK) {
        int rows = A->rows;
        int cols = A->columns;
        if (s21_create_matrix(rows, cols, result) == S21_ERROR) {
            res = S21_ERROR;
            return res;
        }

        for (ptrdiff_t i = 0; i < rows; i++) {
            for (ptrdiff_t j = 0; j < cols; j++) {
                result->matrix[i][j] = number * A->matrix[i][j];
            }
        }
    }

    return res;
}

int s21_mult_matrix(matrix_t* A, matrix_t* B, matrix_t* result) {
    int res = S21_OK;

    if (s21_is_matrix_exists(A) && s21_is_matrix_exists(B) && result) {
        if (A->columns == B->rows) {
            res = s21_create_matrix(A->rows, B->columns, result);
            if (!res) {
                for (int i = 0; i < A->rows; i++) {
                    for (int j = 0; j < B->columns; j++) {
                        result->matrix[i][j] = 0;
                        for (int m = 0; m < B->rows; m++) {
                            result->matrix[i][j] +=
                                A->matrix[i][m] * B->matrix[m][j];
                        }
                    }
                }
            }
        } else {
            res = S21_CALCULATION_ERROR;
        }
    } else {
        res = S21_ERROR;
    }

    return res;
}

int s21_transpose(matrix_t* A, matrix_t* result) {
    int res = S21_OK;
    if (!s21_is_matrix_exists(A)) res = S21_ERROR;

    if (res == S21_OK) {
        if ((res = s21_create_matrix(A->columns, A->rows, result)) != S21_OK)
            return res;
        int rows = A->rows;
        int cols = A->columns;

        for (ptrdiff_t i = 0; i < rows; i++) {
            for (ptrdiff_t j = 0; j < cols; j++) {
                result->matrix[j][i] = A->matrix[i][j];
            }
        }
    }

    return res;
}

int s21_calc_complements(matrix_t* A, matrix_t* result) {
    int res = S21_OK;
    int size = A->rows;

    if (s21_is_matrix_exists(A)) {
        if (A->rows == A->columns && result) {
            res = s21_create_matrix(size, size, result);

            if (!res && size == 1) {
                result->matrix[0][0] = A->matrix[0][0];
            } else if (!res) {
                matrix_t matrix_temp = {0};
                if (s21_create_matrix(size - 1, size - 1, &matrix_temp))
                    return S21_OK;

                for (int i = 0; i < size; i++) {
                    for (int j = 0; j < size; j++) {
                        s21_fill_determinate_matrix(
                            A->matrix, matrix_temp.matrix, i, j, size);
                        double determinant = 0.0;
                        s21_determinant(&matrix_temp, &determinant);
                        result->matrix[i][j] = pow(-1, i + j) * determinant;
                    }
                }

                s21_remove_matrix(&matrix_temp);
            }
        } else {
            res = S21_CALCULATION_ERROR;
        }
    } else {
        res = S21_ERROR;
    }

    return res;
}

int s21_determinant(matrix_t* A, double* result) {
    int res = S21_OK;
    int size = A->rows;

    if (s21_is_matrix_exists(A)) {
        if (A->rows == A->columns && result) {
            if (A->columns == 1) {
                *result = A->matrix[0][0];
            } else {
                *result = s21_found_determinant(*A, size);
            }
        } else {
            res = S21_CALCULATION_ERROR;
        }
    } else {
        res = S21_ERROR;
    }

    return res;
}

double s21_found_determinant(matrix_t A, int size) {
    double determinant = 0.0;

    if (size == 2) {
        determinant = (A.matrix[0][0] * A.matrix[1][1]) -
                      (A.matrix[0][1] * A.matrix[1][0]);
    } else {
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                matrix_t matrix_temp = {0};
                if (s21_create_matrix(size - 1, size - 1, &matrix_temp) ==
                    S21_ERROR)
                    return S21_OK;

                s21_fill_determinate_matrix(A.matrix, matrix_temp.matrix, i, j,
                                            size);
                determinant += pow(-1, j) * A.matrix[0][j] *
                               s21_found_determinant(matrix_temp, size - 1);

                s21_remove_matrix(&matrix_temp);
            }
        }
    }

    return determinant;
}

void s21_fill_determinate_matrix(double** matrix_input, double** matrix_temp,
                                 int skip_row, int skip_column, int size) {
    int last_elem_in_column = size - 1;
    for (int row = 0, i = 0, j = 0; row < size; row++) {
        for (int column = 0; column < size; column++) {
            if (row == skip_row || column == skip_column) continue;

            matrix_temp[i][j++] = matrix_input[row][column];

            if (j == last_elem_in_column) {
                j = 0;
                i++;
            }
        }
    }
}

int s21_inverse_matrix(matrix_t* A, matrix_t* result) {
    if (!s21_is_matrix_exists(A) || !result) return S21_ERROR;

    if (A->rows != A->columns) return S21_CALCULATION_ERROR;

    double det = 0.0;
    int res = s21_determinant(A, &det);
    if (res != S21_OK) return res;

    if (fabs(det) <= S21_EPSILON) return S21_CALCULATION_ERROR;

    if (A->rows == 1) {
        if (s21_create_matrix(1, 1, result) != S21_OK) return S21_ERROR;
        result->matrix[0][0] = 1.0 / A->matrix[0][0];
        return S21_OK;
    }

    matrix_t A_calc = {0};
    res = s21_calc_complements(A, &A_calc);
    if (res != S21_OK) return res;

    matrix_t A_tr = {0};
    res = s21_transpose(&A_calc, &A_tr);
    if (res != S21_OK) {
        s21_remove_matrix(&A_calc);
        return res;
    }

    res = s21_mult_number(&A_tr, 1.0 / det, result);

    s21_remove_matrix(&A_calc);
    s21_remove_matrix(&A_tr);

    return res;
}

void s21_remove_matrix(matrix_t* A) {
    if (A) {
        if (A->matrix) {
            for (ptrdiff_t i = 0; i < A->rows; i++) {
                free((void*)A->matrix[i]);
                A->matrix[i] = NULL;
            }
            free((void*)A->matrix);
        }
        A->matrix = NULL;  // important for check in create!
        A->columns = 0;
        A->rows = 0;
    }
}
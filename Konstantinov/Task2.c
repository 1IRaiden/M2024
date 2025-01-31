#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <math.h>


typedef double MatrixItem;

struct Matrix {
    size_t cols;
    size_t rows;
    MatrixItem* data;
};

const struct Matrix MATRIX_NULL = { .cols = 0, .rows = 0, .data = NULL };


void matrix_error_message()
{
    printf("Error! Check your actions!\n");
}


struct Matrix matrix_init(const size_t rows, const size_t cols)
{
    if (rows == 0 || cols == 0) {
        return MATRIX_NULL;
    }

    if (rows >= SIZE_MAX / sizeof(MatrixItem) / cols) return MATRIX_NULL;

    struct Matrix A = { .cols = cols, .rows = rows, .data = NULL };
    A.data = (double*)malloc(A.cols * A.rows * sizeof(double));

    if (A.data == NULL) {
        return MATRIX_NULL;
        matrix_error_message();
    }
    return A;
}


struct Matrix matrix_make_ident(size_t rows, size_t cols)
{
    struct Matrix I = matrix_init(rows, cols);
    if (I.data == NULL) {
        return MATRIX_NULL;
    }
    for (size_t idx = 0; idx < rows * cols; idx++) {
        if (idx % (rows + 1) == 0) {
            I.data[idx] = 1.;
        }
        else {
            I.data[idx] = 0;
        }
    }
    return I;
}


struct Matrix matrix_create(const size_t rows, const size_t cols, const double* values)
{
    struct Matrix A = matrix_init(rows, cols);
    if (A.data == NULL) return MATRIX_NULL;
    memcpy(A.data, values, rows * cols * sizeof(double));
    return A;
}


void matrix_free(struct Matrix* A)
{
    if (A->data != NULL || A->cols != 0 || A->rows != 0) {

        A->cols = 0;
        A->rows = 0;
        free(A->data);
        A->data = NULL;

    }

}


void matrix_print(const struct Matrix A)
{
    if (A.data != NULL || A.cols != 0 || A.rows != 0) {
        for (size_t row = 0; row < A.rows; ++row) {
            printf("[ ");
            for (size_t col = 0; col < A.cols; ++col) {
                printf("%4.2f ", A.data[A.cols * row + col]);
            }
            printf("]\n");
        }
        printf("\n");
    }
    else {
        matrix_error_message();
    }
}


struct Matrix matrix_sum(const struct Matrix A, const struct Matrix B)
{
    if (A.cols != B.cols || A.rows != B.rows) return MATRIX_NULL;

    struct Matrix C = matrix_init(A.cols, A.rows);
    for (size_t idx = 0; idx < C.cols * C.rows; ++idx) {
        C.data[idx] = A.data[idx] + B.data[idx];
    }
    return C;
}


struct Matrix matrix_sub(const struct Matrix A, const struct Matrix B)
{
    if (A.cols != B.cols || A.rows != B.rows) return MATRIX_NULL;

    struct Matrix C = matrix_init(A.cols, A.rows);
    for (size_t idx = 0; idx < C.cols * C.rows; ++idx) {
        C.data[idx] = A.data[idx] - B.data[idx];
    }
    return C;
}


struct Matrix matrix_mult(const struct Matrix A, const struct Matrix B)
{
    if (A.cols != B.rows) return MATRIX_NULL;
    struct Matrix C = matrix_init(A.cols, B.rows);

    if (C.data == NULL) {
        return MATRIX_NULL;
    }

    for (size_t rowA = 0; rowA < A.rows; ++rowA) {
        for (size_t colsB = 0; colsB < B.cols; ++colsB) {
            C.data[rowA * (A.cols) + colsB] = 0;
            for (size_t idx = 0; idx < A.cols; ++idx) {
                C.data[rowA * (A.cols) + colsB] += (A.data[(rowA * A.cols) + idx]) * (B.data[(idx * B.cols) + colsB]);
            }
        }
    }

    return C;
}


double matrix_det(struct Matrix* A) {
    if (A->cols != A->rows) {
        matrix_error_message();
        return NAN;
    }

    if (A->cols == 1) {
        return A->data[0];
    }

    if (A->cols == 2) {
        double matr_det = (A->data[0]) * (A->data[3]) - (A->data[1]) * (A->data[2]);
        return matr_det;
    }

    if (A->cols == 3) {
        double matr_det = (A->data[0]) * (A->data[4]) * (A->data[8]) + (A->data[1]) * (A->data[5]) * (A->data[6]) + (A->data[3]) * (A->data[7]) * (A->data[2]);
        matr_det -= ((A->data[2]) * (A->data[4]) * (A->data[6]) + (A->data[1]) * (A->data[3]) * (A->data[8]) + (A->data[0]) * (A->data[5]) * (A->data[7]));
        return matr_det;
    }
    matrix_error_message();
    return NAN;
}


struct Matrix sum_for_e(const size_t deg_acc, const struct Matrix A)
{
    struct Matrix E = matrix_init(A.rows, A.cols);

    if (E.data == NULL) {
        return MATRIX_NULL;
    }

    if (deg_acc == 1) {
        struct Matrix E = matrix_make_ident(A.cols, A.rows);
        return E;
    }

    if (deg_acc == 2) {
        return A;
    }

    if (deg_acc > 2) {
        E = A;
        for (size_t id = 2; id < deg_acc; ++id) {
            struct Matrix buf = E;
            E = matrix_mult(buf, A);
            for (size_t idx = 0; idx < E.rows * E.cols; ++idx) {
                E.data[idx] /= (id);
            }
            matrix_free(&buf);
        }
    }
    return E;
}


struct Matrix matrix_exp(struct Matrix* A, const size_t accuracy)
{
    if (A->cols != A->rows) {
        matrix_error_message();
        return MATRIX_NULL;
    }

    struct Matrix E = matrix_init(A->rows, A->cols);

    if (E.data == NULL) {
        return MATRIX_NULL;
    }
    struct Matrix matrix_transfer;

    for (size_t deg_acc = 1; deg_acc <= accuracy; ++deg_acc) {
        matrix_transfer = sum_for_e(deg_acc, *A);
        struct Matrix buf1 = E;
        E = matrix_sum(buf1, matrix_transfer);
        matrix_free(&buf1);
        matrix_free(&matrix_transfer);
    }

    return E;
}


int main()
{
    struct Matrix A, B, C, E, N;

    double deter;

    A = matrix_create(3, 3, (double[]) { 3., 2., 1., 1., 6., 7., 2., 6., 8. });
    B = matrix_create(3, 3, (double[]) { 1., 0., 0., 1., 7., 2., 6., 6., 8. });
    E = matrix_create(2, 2, (double[]) { 3., 2., 1., 1. });

    matrix_print(A);
    matrix_print(B);
    matrix_add(B, A);
    matrix_print(B);

    C = matrix_mult(A, B);
    matrix_print(C);

    N = matrix_make_ident(3, 3);
    matrix_print(N);

    matrix_print(A);
    A = matrix_exp(&A, 3);
    matrix_print(A);

    matrix_free(&E);
    matrix_free(&A);
    matrix_free(&B);
    matrix_free(&C);
    matrix_free(&N);

    return 0;
}

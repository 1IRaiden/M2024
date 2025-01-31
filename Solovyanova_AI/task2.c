#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <locale.h>


enum ErrorType { ERROR };

void matrix_error(enum ErrorType type, const char *message) 
{
    if (type == ERROR) {
        printf("ОШИБКА! %s\n", message);
    }
}

struct Matrix {
    size_t cols;
    size_t rows;
    double *data;
};

typedef struct Matrix Matrix;

Matrix matrix_locate(size_t rows, size_t cols) // выделение памяти
{
    Matrix matrix;
    matrix.rows = rows;
    matrix.cols = cols;

    if (rows * cols == 0) {
        matrix_error(ERROR, "Нулевые размеры матрицы");  //проверка, что если столбец или строка не равны 0
        matrix.cols = 0;
        matrix.rows = 0;
        matrix.data = NULL; 
        return matrix;  
    }

    if(SIZE_MAX / cols / rows < sizeof(double)) {   //проверка занимаемой памяти для каждого элемента
        matrix_error(ERROR, "Матрица превышает допустимый размер");  //переполнение памяти
        matrix.cols = 0;
        matrix.rows = 0;
        matrix.data = NULL;
        return matrix;
    }

    matrix.data = (double *)malloc(rows * cols * sizeof(double));

    if (matrix.data == NULL) {
        matrix_error(ERROR, "Ошибка выделения памяти");
        matrix.cols = 0;
        matrix.rows = 0;
    }
    
    return matrix;
}

//освобождение памяти для хранения матрицы
void matrix_free(Matrix matrix) 
{
    free(matrix.data); 
    matrix.data = NULL;
    matrix.cols = 0;
    matrix.rows = 0;
}


void matrix_print(Matrix matrix) 
{
    for (size_t row = 0; row < matrix.rows; row++) {
        for (size_t col = 0; col < matrix.cols; col++) {
            printf("%f ", matrix.data[row * matrix.cols + col]);
        }
        printf("\n");
    }
}

Matrix matrix_add(Matrix A, Matrix B) 
{
    if (A.rows != B.rows || A.cols != B.cols) {
        matrix_error(ERROR, "Размеры матриц не совпадают");
        Matrix emptyMatrix = matrix_locate(0, 0);
        return emptyMatrix;
    }

    Matrix result = matrix_locate(A.rows, A.cols);
    if (result.cols == 0 && result.rows == 0) {
        return result;
    }

    for (size_t ind = 0; ind < A.rows * A.cols; ind++) {
        result.data[ind] = A.data[ind] + B.data[ind];
    }
    return result;
}

Matrix matrix_sub(Matrix A, Matrix B) 
{
    if (A.rows != B.rows || A.cols != B.cols) {
        matrix_error(ERROR, "Размеры матриц не совпадают");
        Matrix emptyMatrix = matrix_locate(0, 0);
        return emptyMatrix;
    }

    Matrix result = matrix_locate(A.rows, A.cols);
    if (result.cols == 0 && result.rows == 0) {
        return result;
    }

    for (size_t ind = 0; ind < A.rows * A.cols; ind++) {
        result.data[ind] = A.data[ind] - B.data[ind];
    }
    return result;
}

Matrix matrix_mul(Matrix A, Matrix B)
{
     if (A.data == NULL || B.data == NULL) {
        matrix_error(ERROR, "Одна из матриц пустая");
        return matrix_locate(0, 0);
    }

    if (A.cols != B.rows) {
        matrix_error(ERROR, "Количество столбцов матрицы A отличается от количества строк матрицы B");
        return matrix_locate(0, 0);
    }

    Matrix result = matrix_locate(A.rows, B.cols);

    for (size_t row_A = 0; row_A < A.rows; row_A++) {
        for (size_t col_B = 0; col_B < B.cols; col_B++) {
            double sum = 0;
            for (size_t col_A = 0; col_A < A.cols; col_A++) {
                sum += A.data[row_A * A.cols + col_A] * B.data[col_A * B.cols + col_B];
            }
            result.data[row_A * result.cols + col_B] = sum;
        }
    }
    return result;
}

Matrix matrix_mul_scalar(Matrix matrix, double scalar)
{
    if (matrix.data == NULL) {
        matrix_error(ERROR, "Матрица не должна быть пустой");
        return matrix_locate(0, 0);
    }

    Matrix result = matrix_locate(matrix.rows, matrix.cols);
    
    for (size_t ind = 0; ind < matrix.rows * matrix.cols; ind++) {
        result.data[ind] = matrix.data[ind] * scalar;
    }
    return result;
}

//единичная матрица
Matrix matrix_identity(size_t rows, size_t cols) {
    Matrix matrix = matrix_locate(rows, cols);
    
    if (matrix.data == NULL) {
        matrix_error(ERROR, "Не удалось создать матрицу");
        return matrix_locate(0, 0);
    }

    if(matrix.cols != matrix.rows) {
        matrix_error(ERROR, "Матрица должна быть квадратной");
        return matrix_locate(0, 0);
    }

    for (size_t row = 0; row < matrix.rows; ++row) {
        for (size_t col = 0; col < matrix.cols; ++col) {
            if (row == col) {
                matrix.data[row * matrix.cols + col] = 1.0;
            }
            else {
                matrix.data[row * matrix.cols + col] = 0.0;
            }
        }
    }

    return matrix;
}

//e^A
Matrix matrix_exp(Matrix matrix, const size_t iteration)
{
    if (matrix.rows * matrix.cols == 0) {
        matrix_error(ERROR, "Нулевые размеры матрицы");
        return matrix_locate(0, 0);  
    }
    
    if (matrix.data == NULL) {
        matrix_error(ERROR, "Матрица не должна быть пустой");
        return matrix_locate(0, 0);
    }

    if(matrix.cols != matrix.rows) {
        matrix_error(ERROR, "Матрица должна быть квадратной");
        return matrix_locate(0, 0);
    }

    //заполнение единичной матрицей
    Matrix result = matrix_identity(matrix.rows, matrix.cols);
    Matrix B = matrix_identity(matrix.rows, matrix.cols);

    if (result.data == NULL || B.data == NULL) {
        matrix_error(ERROR, "Не удалось создать матрицу");
        matrix_free(result);
        matrix_free(B);
        return matrix_locate(0, 0);
    }


    for (size_t k = 1; k <= iteration; ++k) {
        struct Matrix B_shadow = B;
            
        B = matrix_mul(B_shadow, matrix);
        matrix_free(B_shadow);
        
        B_shadow = B;
        B = matrix_mul_scalar(B, 1.0 / k);
        matrix_free(B_shadow);

        B_shadow = B;
        result = matrix_add(result, B);
        matrix_free(B_shadow);
    }

    matrix_free(B);

    return result;
}

double matrix_det(Matrix matrix) 
{
    if(matrix.data == NULL) {
        matrix_error(ERROR, "Матрица не должна быть пустой");
        return NAN;
    }

    if(matrix.cols == 0) {
        matrix_error(ERROR, "Количество столбцов матрицы равно нулю");
        return NAN;
    }

    if(matrix.cols != matrix.rows) {
        matrix_error(ERROR, "Матрица должна быть квадратной");
        return NAN;
    }

    if (matrix.cols == 1)
	{
		return matrix.data[0];
	}

	if (matrix.cols == 2)
	{
		double det = (matrix.data[0]) * (matrix.data[3]) - (matrix.data[1]) * (matrix.data[2]);
		return det;
	}

	if (matrix.cols == 3)
	{
		double det = (matrix.data[0]) * (matrix.data[4]) * (matrix.data[8]) + (matrix.data[1]) * (matrix.data[5]) * (matrix.data[6]) + (matrix.data[3]) * (matrix.data[7]) * (matrix.data[2]);
		det -= ((matrix.data[2]) * (matrix.data[4]) * (matrix.data[6]) + (matrix.data[1]) * (matrix.data[3]) * (matrix.data[8]) + (matrix.data[0]) * (matrix.data[5]) * (matrix.data[7]));
		return det;
	}
    return NAN;
}

void matrix_from_array(Matrix* matrix, double* array) 
{
    matrix->data = (double *)malloc(matrix->rows * matrix->cols * sizeof(double));
    if (matrix->data == NULL) {
        matrix_error(ERROR, "Ошибка выделения памяти");
        matrix->cols = 0;
        matrix->rows = 0;
        return;
    }
    
    for (size_t i = 0; i < matrix->rows * matrix->cols; i++) {
        matrix->data[i] = array[i];
    }
}

int main() 
{
    Matrix A = {3, 3, NULL};
    Matrix B = {3, 3, NULL};
    Matrix C = {3, 3, NULL};
    double det_A, det_B;
    double det = NAN;

    matrix_from_array(&A, (double[]){1, 1, 1, 1, 1, 1, 1, 1, 1});
    matrix_from_array(&B, (double[]){5, -2, 1, 0, 2, 3, -1, 4, 7});
    C = matrix_locate(A.rows, A.cols);
    
    det_A = matrix_det(A);
    det_B = matrix_det(B);

    printf("Матрица A:\n");
    matrix_print(A);
    printf("\nМатрица B:\n");
    matrix_print(B);

    printf("\nСложение матриц A и B:\n");
    C = matrix_add(A, B);
    matrix_print(C);

    printf("\nВычитание матриц A и B:\n");
    C = matrix_sub(A, B);
    matrix_print(C);

    printf("\nУмножения матриц A и B:\n");
    C = matrix_mul(A, B);
    matrix_print(C);

    printf("\nУмножения матрицы A на скаляр 3:\n");
    C = matrix_mul_scalar(A, 3);
    matrix_print(C);

    printf("\nЭкспонента матрицы A:\n");
    //C = matrix_identity(3, 3);
    C = matrix_exp(A, 10);
    matrix_print(C);

    printf("\nОпределитель матрицы A: %f\n", det_A);
    printf("Определитель матрицы B: %f\n", det_B);

    // Освобождение памяти
    matrix_free(A);
    matrix_free(B);
    matrix_free(C);

    return 0;
}
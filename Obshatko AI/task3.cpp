#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <locale.h>


class Matrix {
    public:
        size_t cols;
        size_t rows;
        double* data;

        Matrix(const size_t received_cols, const size_t received_rows)
        {
            if(SIZE_MAX / received_cols < received_rows) {
                //matrix_log(ERROR, "Матрица слишком большая");
                cols = 0;
                rows = 0;
                data = NULL;
                return;
            }

            data = new double[received_cols * received_rows];
            if(data != NULL) {
                cols = received_cols;
                rows = received_rows;
            } else {
                //matrix_log(ERROR, "Ошибка выделения памяти");
                cols = 0;
                rows = 0;
                return;
            }
        }


        void fill_random(size_t max_value)
        {
            for(size_t index = 0; index < rows * cols; index++) {
                double random_number = round(((float)rand()/RAND_MAX)*(float)(max_value * 10)) / 10.0;
               data[index] = random_number;
            }
        }


        //TODO перегрузить cout?
        void print()
        {
            for(size_t index_row = 0; index_row < rows; index_row++) {
                for(size_t index_col = 0; index_col < cols; index_col++) {
                    printf("%.2lf  ", data[index_row * cols + index_col]);
                }
                printf("\n");
            }
            printf("\n");
        }


        const Matrix operator+(const Matrix& matrix_to_add) const {
            // проверка равности размерноти матриц
            // TODO вызвать ошибку?
            if(cols != matrix_to_add.cols || rows != matrix_to_add.rows) {
                //matrix_log(ERROR, "Для выполнения операции вычитания матрицы должны иметь одинаковые размерности");
            }

            Matrix result_matrix(cols, rows);

            for(size_t index = 0; index < rows * cols; index++) {
                result_matrix.data[index] = data[index] + matrix_to_add.data[index];
            }
            return result_matrix;
        }
};


int main()
{
    setlocale(LC_CTYPE, "");

    Matrix matrix_A(4, 4);
    Matrix matrix_B(4, 4);

    matrix_A.fill_random(10);
    matrix_B.fill_random(10);

    Matrix matrix_C = matrix_A + matrix_B;

    matrix_A.print();
    matrix_B.print();
    matrix_C.print();

    return 0;
}

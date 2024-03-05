#include <math.h>
#include <iostream>
#include <algorithm>
#include <string>
#include <cstring>
#include "matrix.hpp"

const char* BAD_REQUEST = "bad_request";
const char* OUT_OF_RANGE = "out_of_range";

Matrix::Matrix() : rows{ 0 }, cols{ 0 }, items{ nullptr } {}


Matrix::Matrix(const size_t a, const size_t b)
    : rows{ a }, cols{ b }, items{ nullptr }
{
    // TODO except ���� �� ����
    if (rows == 0 && cols == 0)
        return;

    if (rows == 0 || cols == 0)
        throw std::exception(BAD_REQUEST);

    items = new MatrixItem[rows * cols];
}


Matrix::Matrix(const Matrix& A)
    : rows{ A.rows }, cols{ A.cols }, items{ nullptr }
{
    if (A.items == nullptr) return;

    items = new MatrixItem[rows * cols];

    memcpy(ptr_start(), A.ptr_start(), rows * cols * sizeof(MatrixItem));
}

Matrix::Matrix(Matrix&& A) : rows{ A.rows }, cols{ A.cols }, items{ A.items }
{
    A.set_null();
}


void Matrix::set_null()
{
    items = nullptr;
    rows = 0;
    cols = 0;
}


Matrix& Matrix::operator=(std::initializer_list<MatrixItem> lst)
{
    if (lst.size() != rows * cols)
        throw std::exception(OUT_OF_RANGE);

    std::copy(lst.begin(), lst.end(), ptr_start());

    return *this;
}


Matrix& Matrix::operator=(const Matrix& A)
{
    if (this == &A) return *this;

    if (items == nullptr) {
        items = new MatrixItem[A.rows * A.cols];
        rows = A.rows;
        cols = A.cols;
        memcpy(ptr_start(), A.ptr_start(), rows * cols * sizeof(MatrixItem));
        return *this;
    }

    if (rows * cols == A.cols * A.rows) {
        memcpy(ptr_start(), A.ptr_start(), rows * cols * sizeof(MatrixItem));
        return *this;
    }

    delete[] items;
    items = new MatrixItem[A.rows * A.cols];

    rows = A.rows;
    cols = A.cols;

    memcpy(ptr_start(), A.ptr_start(), rows * cols * sizeof(MatrixItem));

    return *this;
}


Matrix& Matrix::operator=(Matrix&& A)
{
    if (items != nullptr)
        delete[] items;

    rows = A.rows;
    cols = A.cols;
    items = A.items;

    A.set_null();

    return *this;
}


void Matrix::set_all_zero()
{
    memset(items, 0, sizeof(MatrixItem) * cols * rows);
}


void Matrix::set_all_one()
{
    set_all_zero();

    for (size_t idx = 0; idx < cols; idx++)
        items[idx + idx * cols] = 1.0;
}


size_t Matrix::get_rows() const { return rows; }


size_t Matrix::get_cols() const { return cols; }


Matrix& Matrix::operator+=(const Matrix& A)
{
    if ((rows != A.rows) || (cols != A.cols))
        throw std::exception(BAD_REQUEST);

    for (size_t idx = 0; idx < (rows * cols); idx++)
        items[idx] += A.items[idx];

    return *this;
}


Matrix operator+(const Matrix& A, const Matrix& B)
{
    Matrix sum = A;
    sum += B;
    return sum;
}


Matrix operator+(const Matrix& A, Matrix&& B)
{
    Matrix sum = B;
    sum += A;
    return sum;
}


Matrix& Matrix::operator-=(const Matrix& A)
{
    if ((rows != A.rows) || (cols != A.cols))
        throw std::exception(BAD_REQUEST);

    for (size_t idx = 0; idx < (rows * cols); idx++)
        items[idx] -= A.items[idx];

    return *this;
}


Matrix operator-(const Matrix& A, const Matrix& B)
{
    Matrix sub = A;
    sub -= B;
    return sub;
}


Matrix operator-(const Matrix& A, Matrix&& B)
{
    Matrix sub = B;
    sub -= A;
    return sub;
}


Matrix& Matrix::multiply(Matrix& trg, const Matrix& A) const
{
    for (size_t num_row = 0; num_row < rows; num_row++) {
        for (size_t num_col = 0; num_col < A.cols; num_col++) {
            MatrixItem sum = 0;

            for (size_t num_sum = 0; num_sum < A.rows; num_sum++) {
                sum += items[num_row * cols + num_sum] * A.items[num_sum * A.cols + num_col];
            }

            trg.items[num_row * trg.cols + num_col] = sum;
        }
    }

    return trg;
}


Matrix Matrix::operator*(const Matrix& A) const
{
    if (cols != A.rows)
        throw std::exception(BAD_REQUEST);

    Matrix mult(rows, A.cols);
    multiply(mult, A);
    return mult;
}


Matrix& Matrix::operator*=(const Matrix& A)
{
    *this = *this * A;
    return *this;
}


Matrix& Matrix::operator*=(const MatrixItem& factor)
{
    for (size_t idx = 0; idx < (rows * cols); idx++)
        items[idx] *= factor;

    return *this;
}


Matrix Matrix::operator*(const MatrixItem& factor)
{
    Matrix mult = *this;
    mult *= factor;
    return mult;
}


Matrix Matrix::transposed()
{
    Matrix trn(cols, rows);

    for (size_t num_row = 0; num_row < cols; num_row++)
        for (size_t num_col = 0; num_col < rows; num_col++)
            trn.items[num_row * trn.cols + num_col] = items[num_col * cols + num_row];

    return trn;
}


double Matrix::determinant() const
{
    if (cols != rows)
        throw std::exception(BAD_REQUEST);

    if (items == nullptr)
        throw std::exception(BAD_REQUEST);

    Matrix mat = *this;

    double det = 1.0;
    int pivot = 0;
    double factor = 0;

    for (size_t num_col = 0; num_col < mat.cols; num_col++) {
        pivot = num_col;

        for (size_t num_row = num_col + 1; num_row < mat.rows; num_row++) {
            if (fabs(mat.items[num_row * mat.cols + num_col]) > fabs(mat.items[pivot * mat.cols + num_col]))
                pivot = num_row;
        }

        if (pivot != num_col) {
            for (size_t idx = 0; idx < mat.rows; idx++)
                std::swap(mat.items[num_col * mat.cols + idx], mat.items[pivot * mat.cols + idx]);

            det *= -1;
        }

        if (mat.items[num_col * mat.cols + num_col] == 0) {
            det = 0;
            break;
        }

        det *= mat.items[num_col * mat.cols + num_col];

        for (size_t num_row = num_col + 1; num_row < mat.rows; num_row++) {
            factor = mat.items[num_row * mat.cols + num_col] / mat.items[num_col * mat.cols + num_col];

            for (size_t idx = num_col + 1; idx < mat.cols; idx++)
                mat.items[num_row * mat.cols + idx] -= factor * mat.items[num_col * mat.cols + idx];
        }
    }

    return det;
}


Matrix Matrix::exponential(const MatrixItem& accuracy) const
{
    if (cols != rows)
        throw std::exception(BAD_REQUEST);

    if (items == nullptr)
        throw std::exception(BAD_REQUEST);

    Matrix sum(rows, cols);
    Matrix term(rows, cols);
    Matrix temp(rows, cols);

    term.set_all_one();
    sum.set_all_one();

    for (size_t count = 1; count < 200; count++) {
        term *= (1.0 / count);
        multiply(temp, term);
        std::swap(term.items, temp.items);

        sum += term;

        // TODO return old mine func
        if (term.get_maximum() < accuracy)
            return sum;
    }

    return sum;
}


MatrixItem Matrix::get_maximum()
{
    MatrixItem max = 0;
    MatrixItem num = 0;

    for (size_t idx = 0; idx < (rows * cols); idx++) {
        num = std::fabs(items[idx]);
        if (num > max) max = num;
    }

    return max;
}


MatrixItem* Matrix::ptr_start() { return items; }
MatrixItem* Matrix::ptr_end() { return items + rows * cols; }
const MatrixItem* Matrix::ptr_start() const { return items; }
const MatrixItem* Matrix::ptr_end() const { return items + rows * cols; }


void Matrix::set(size_t row, size_t col, MatrixItem value)
{
    if (row >= rows || col >= cols)
        throw OUT_OF_RANGE;

    items[row * cols + col] = value;
}


MatrixItem Matrix::get(size_t row, size_t col) const
{
    if (row >= rows || col >= cols)
        throw OUT_OF_RANGE;

    return items[row * cols + col];
}


bool Matrix::operator==(const Matrix& A) const
{
    if ((cols != A.cols) || (rows != A.rows))
        return false;

    for (size_t idx = 0; idx < (rows * cols); idx++) {
        if ((int)(items[idx] * 10) != (int)(A.items[idx] * 10))
            return false;
    }

    return true;
}


std::ostream& operator<<(std::ostream& os, const Matrix& A)
{
    for (size_t row = 0; row < A.get_rows(); row++) {
        for (size_t col = 0; col < A.get_cols(); col++) {
            os << A.get(row, col) << "\t";
        }
        os << std::endl;
    }

    os << std::endl;

    return os;
}


Matrix::~Matrix()
{
    if (items != nullptr) {
        delete[] items;
    }
}

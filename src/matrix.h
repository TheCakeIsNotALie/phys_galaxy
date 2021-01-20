/*
Author : Yannis Perrin
Date : 20.03.2019
Description : Matrix structure (values are stored in a single array, representing a 2d one)
*/
#include <stdbool.h>

#pragma once

/*
* Initialise a Matrix representing a point 
* Matrix with size x=1, y=3 ((0,0){x}, (0,1){y}, (0,2){used for 3x3 matrix transformation})
* 
* do not forget to free after use
*/
#define INITIALISE_MATRIX_VECTOR2(var, x, y) \
	var = matrix_initializer(1, 3);         \
	*matrix_addressOf(var, 0, 0) = (x);      \
	*matrix_addressOf(var, 0, 1) = (y);      \
	*matrix_addressOf(var, 0, 2) = 1;

//Matrix
typedef struct Matrix_s {
	int size_x;
	int size_y;
	double *values;
} Matrix_t;

/**
 * @brief Initializes a new Matrix_t instance at the specified Matrix_t pointer.
 * @return void
 */
Matrix_t *matrix_initializer(int size_x, int size_y);

/**
 * @brief Get the pointer to one of the values of the Matrix_t.
 * @return double*
 */
double* matrix_addressOf(Matrix_t *matrix, int x, int y);
/**
 * @brief Get the value of one of the values of the Matrix_t.
 * @return double
 */
double matrix_valueOf(Matrix_t *matrix, int x, int y);
/**
 * @brief Verify if two Matrix_t contain equal values.
 * @return bool
 */
bool matrix_equals(Matrix_t *one, Matrix_t *two);

/**
 * @brief Clone a given Matrix_t.
 * @return Matrix_t
 */
Matrix_t *matrix_clone(Matrix_t *matrix);
/**
 * @brief Add two Matrix_t together.
 * @return Matrix_t
 */
Matrix_t *matrix_add(Matrix_t *one, Matrix_t *two);
/**
 * @brief Subtract a Matrix_t with another (one - two).
 * @return Matrix_t
 */
Matrix_t *matrix_sub(Matrix_t *one, Matrix_t *two);


/**
 * @brief Compute the dot product (scalar product) of two one dimensional matrices of the same size.
 * @return double
 */
double matrix1d_dot_product(Matrix_t *one, Matrix_t *two);
/**
 * @brief Compute the dot product (scalar product) of two Matrix_t representing a 2d vector (Matrix_t of size : (1, 3)).
 * @return double
 */
double matrix_vector2_dot_product(Matrix_t *one, Matrix_t *two);
/**
 * @brief Compute the magnitude of a Matrix_t representing a 2d vector (Matrix_t of size : (1, 3)).
 * @return double
 */
double matrix_vector2_magnitude(Matrix_t *matrix);
/**
 * @brief Compute the distance between two Matrix_t representing a 2d vector (Matrix_t of size : (1, 3)).
 * @return double
 */
double matrix_vector2_distance(Matrix_t *one, Matrix_t *two);
/**
 * @brief Multiply a Matrix_t representing a 2d vector (Matrix_t of size : (1, 3)) by a scalar number.
 * @return double
 */
Matrix_t *matrix_vector2_multiply_double(Matrix_t *matrix, double scalar);


/**
 * @brief Compute the vector product of two Matrix_t (has to have the same size).
 * @return Matrix_t
 */
Matrix_t *matrix_multiply(Matrix_t *one, Matrix_t *two);
/**
 * @brief Compute the vector product of two Matrix_t (has to have the same size).
 * @return Matrix_t
 */
Matrix_t *matrix_multiply_double(Matrix_t *matrix, double number);

/**
 * @brief Get the identity matrix of a given size.
 * @return Matrix_t
 */
Matrix_t *matrix_identity(int size);
/**
 * @brief Get a string representation of the given Matrix_t.
 * @return char*
 */
char* matrix_toString(Matrix_t *matrix);

void matrix_destroy(Matrix_t *matrix);
/*
Author : Yannis Perrin
Date : 20.03.2019
Description : Matrix structure and functions (values are stored in a single array, representing a 2d one)
*/
#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"
#include "tests.h"
#include <string.h>
#include <float.h>
#include <math.h>

Matrix_t *matrix_initializer(int size_x, int size_y)
{
	Matrix_t *matrix = (Matrix_t *)malloc(sizeof(Matrix_t));

	matrix->size_x = size_x;
	matrix->size_y = size_y;

	matrix->values = (double *)calloc(size_x * size_y, sizeof(double));

	return matrix;
}

bool matrix_equals(Matrix_t *one, Matrix_t *two)
{
	if (one->size_x == two->size_x && one->size_y == two->size_y)
	{
		for (int y = 0; y < one->size_y; y++)
		{
			for (int x = 0; x < one->size_x; x++)
			{
				//if the difference is less than epsilon (floating point arithmetic is not perfect and isn't comparable so we look if the difference is acceptable)
				//chose to have float precision error since double didn't work too well
				if (fabs(matrix_valueOf(one, x, y) - matrix_valueOf(two, x, y)) > FLT_EPSILON)
				{
					return false;
				}
			}
		}
	}
	else
	{
		return false;
	}
	return true;
}

double *matrix_addressOf(Matrix_t *matrix, int x, int y)
{
	//printf("address of %d, %d\n", x, y);
	return &matrix->values[y * matrix->size_x + x];
}

double matrix_valueOf(Matrix_t *matrix, int x, int y)
{
	//printf("value of %d, %d\n", x, y);
	return matrix->values[y * matrix->size_x + x];
}

Matrix_t *matrix_clone(Matrix_t *matrix)
{
	Matrix_t *newMatrix = matrix_initializer(matrix->size_x, matrix->size_y);

	for (int y = 0; y < matrix->size_y; y++)
	{
		for (int x = 0; x < matrix->size_x; x++)
		{
			*matrix_addressOf(newMatrix, x, y) = matrix_valueOf(matrix, x, y);
		}
	}

	return newMatrix;
}

Matrix_t *matrix_add(Matrix_t *one, Matrix_t *two)
{
	Matrix_t *newMatrix;

	if (one->size_x == two->size_x && one->size_y == two->size_y)
	{
		newMatrix = matrix_initializer(one->size_x, one->size_y);

		for (int y = 0; y < one->size_y; y++)
		{
			for (int x = 0; x < one->size_x; x++)
			{
				*matrix_addressOf(newMatrix, x, y) = matrix_valueOf(one, x, y) + matrix_valueOf(two, x, y);
			}
		}
	}

	return newMatrix;
}

Matrix_t *matrix_sub(Matrix_t *one, Matrix_t *two)
{
	Matrix_t *newMatrix;

	if (one->size_x == two->size_x && one->size_y == two->size_y)
	{
		newMatrix = matrix_initializer(one->size_x, one->size_y);

		for (int y = 0; y < one->size_y; y++)
		{
			for (int x = 0; x < one->size_x; x++)
			{
				*matrix_addressOf(newMatrix, x, y) = matrix_valueOf(one, x, y) - matrix_valueOf(two, x, y);
			}
		}
	}

	return newMatrix;
}

double matrix_vector2_dot_product(Matrix_t *one, Matrix_t *two)
{
	double dotProd = 0;

	//only accept vectors of same length (and remove last y value since it is only used in transform operations)
	if (one->size_x == 1 && one->size_x == two->size_x && one->size_y == two->size_y)
	{
		for (int y = 0; y < one->size_y - 1; y++)
		{
			dotProd += matrix_valueOf(one, 0, y) * matrix_valueOf(two, 0, y);
		}
	}

	return dotProd;
}

double matrix1d_dot_product(Matrix_t *one, Matrix_t *two)
{
	double dotProd = 0;

	//only accept vectors of same length (and remove last y value since it is only used in transform operations)
	if (one->size_x == 1 && one->size_x == two->size_x && one->size_y == two->size_y)
	{
		for (int y = 0; y < one->size_y; y++)
		{
			dotProd += matrix_valueOf(one, 0, y) * matrix_valueOf(two, 0, y);
		}
	}

	return dotProd;
}

Matrix_t *matrix_multiply(Matrix_t *one, Matrix_t *two)
{
	Matrix_t *newMatrix;
	double dotProduct;

	if (one->size_x == two->size_y)
	{
		newMatrix = matrix_initializer(two->size_x, one->size_y);

		for (int y = 0; y < newMatrix->size_y; y++)
		{
			for (int x = 0; x < newMatrix->size_x; x++)
			{
				dotProduct = 0;

				for (int i = 0; i < one->size_x; i++)
				{
					dotProduct += matrix_valueOf(one, i, y) * matrix_valueOf(two, x, i);
				}
				*matrix_addressOf(newMatrix, x, y) = dotProduct;
			}
		}
	}

	return newMatrix;
}

Matrix_t *matrix_multiply_double(Matrix_t *matrix, double number)
{
	Matrix_t *newMatrix = matrix_initializer(matrix->size_x, matrix->size_y);

	for (int y = 0; y < matrix->size_y; y++)
	{
		for (int x = 0; x < matrix->size_x; x++)
		{
			*matrix_addressOf(newMatrix, x, y) = matrix_valueOf(matrix, x, y) * number;
		}
	}

	return newMatrix;
}

double matrix_vector2_magnitude(Matrix_t *matrix)
{
	double result = 0;

	if (matrix->size_x == 1)
	{
		//do not use last value of vector since it is only used for transformations
		for (int i = 0; i < matrix->size_y - 1; i++)
		{
			result += pow(matrix_valueOf(matrix, 0, i), 2);
		}
		result = sqrt(result);
	}
	else
	{
		//Cannot calculate magnitude
		result = -1;
	}

	return result;
}

Matrix_t *matrix_identity(int size)
{
	Matrix_t *newMatrix = matrix_initializer(size, size);

	for (int i = 0; i < size; i++)
	{
		*matrix_addressOf(newMatrix, i, i) = 1;
	}

	return newMatrix;
}

/*
 * Returns a dynamically allocated string that represents the values of a given matrix.
 * Maximum size of the string representation of each number is arbitrarly 7 digits before the comma and 6 digits after it.
 */
char *matrix_toString(Matrix_t *matrix)
{
	//printf("%u, %d, %u, %d\n", matrix, matrix->size_x, &matrix->size_y, matrix->size_y);
	//calculate the max size of the string
	char *strResult = (char *)calloc(matrix->size_y * 3 + matrix->size_x * matrix->size_y * 14, sizeof(char));

	//Create the string to look something like this :
	//|6.000000,7.750000|
	//|1.000000,123.000000|
	for (int y = 0; y < matrix->size_y; y++)
	{
		strcat(strResult, "|\0");
		for (int x = 0; x < matrix->size_x; x++)
		{
			char buffer[32];

			sprintf(buffer, "%f", matrix_valueOf(matrix, x, y));
			strcat(strResult, buffer);
			strcat(strResult, ",\0");
		}
		strResult[strlen(strResult) - 1] = '\0';
		strcat(strResult, "|\n\0");
	}
	return strResult;
}

void matrix_destroy(Matrix_t *matrix)
{
	free(matrix->values);
	free(matrix);
}

//Build test : (mingw32-)gcc -o test.exe matrix.c -DUNIT_TESTS_M
#ifdef UNIT_TESTS_M
/* Start the overall test suite */
START_TESTS()
START_TEST("Initialization")
Matrix_t *newMatrix = matrix_initializer(3, 1);

ASSERT(newMatrix->values[0 * newMatrix->size_x + 0] == 0.0);
ASSERT(newMatrix->values[0 * newMatrix->size_x + 1] == 0.0);
ASSERT(newMatrix->values[0 * newMatrix->size_x + 2] == 0.0);

matrix_destroy(newMatrix);
END_TEST()

START_TEST("Change value by cell address")
Matrix_t *newMatrix = matrix_initializer(2, 2);
*matrix_addressOf(newMatrix, 0, 0) = 2;

ASSERT(newMatrix->values[0 * newMatrix->size_x + 0] == 2);

matrix_destroy(newMatrix);
END_TEST()

START_TEST("Get Value of cell")
Matrix_t *newMatrix = matrix_initializer(2, 2);
*matrix_addressOf(newMatrix, 0, 0) = 2;

ASSERT(matrix_valueOf(newMatrix, 0, 0) == 2);
matrix_destroy(newMatrix);
END_TEST()

START_TEST("Cloning")
Matrix_t *one, *two;

one = matrix_initializer(3, 2);
*matrix_addressOf(one, 1, 1) = 4;

two = matrix_clone(one);

ASSERT(matrix_valueOf(one, 1, 1) == matrix_valueOf(two, 1, 1));
matrix_destroy(one);
matrix_destroy(two);
END_TEST()

START_TEST("Equals")
Matrix_t *one, *two;

one = matrix_initializer(3, 2);
*matrix_addressOf(one, 1, 1) = 4;

two = matrix_clone(one);

ASSERT(matrix_equals(one, two));
matrix_destroy(one);
matrix_destroy(two);
END_TEST()

START_TEST("Addition")
Matrix_t *one, *two, *add;
one = matrix_initializer(2, 2);

*matrix_addressOf(one, 1, 1) = 4;

two = matrix_clone(one);
add = matrix_add(one, two);

ASSERT(matrix_valueOf(add, 1, 1) == 8);
matrix_destroy(one);
matrix_destroy(two);
matrix_destroy(add);
END_TEST()

START_TEST("Multiplication")
Matrix_t *one, *two, *multi;

one = matrix_initializer(2, 2);
*matrix_addressOf(one, 1, 1) = 4;

two = matrix_clone(one);
multi = matrix_multiply(one, two);

ASSERT(matrix_valueOf(multi, 1, 1) == 16);
matrix_destroy(one);
matrix_destroy(two);
matrix_destroy(multi);
END_TEST()

START_TEST("Multiplication 3x2 * 2x3")
Matrix_t *one, *two, *multi, *result;

one = matrix_initializer(3, 2);
*matrix_addressOf(one, 0, 0) = 1;
*matrix_addressOf(one, 2, 0) = 3;
*matrix_addressOf(one, 0, 1) = 4;
*matrix_addressOf(one, 1, 0) = 2;
*matrix_addressOf(one, 1, 1) = 5;
*matrix_addressOf(one, 2, 1) = 6;

two = matrix_initializer(2, 3);
*matrix_addressOf(two, 0, 0) = 7;
*matrix_addressOf(two, 1, 0) = 8;
*matrix_addressOf(two, 0, 1) = 9;
*matrix_addressOf(two, 1, 1) = 10;
*matrix_addressOf(two, 0, 2) = 11;
*matrix_addressOf(two, 1, 2) = 12;

result = matrix_initializer(2, 2);
*matrix_addressOf(result, 0, 0) = 58;
*matrix_addressOf(result, 1, 0) = 64;
*matrix_addressOf(result, 0, 1) = 139;
*matrix_addressOf(result, 1, 1) = 154;

multi = matrix_multiply(one, two);

ASSERT(matrix_equals(multi, result));
matrix_destroy(one);
matrix_destroy(two);
matrix_destroy(multi);
matrix_destroy(result);
END_TEST()

START_TEST("Multiplication 3x2 * 2x3 with doubles")
Matrix_t *one, *two, *multi, *result;

one = matrix_initializer(3, 2);
*matrix_addressOf(one, 0, 0) = 9.4;
*matrix_addressOf(one, 1, 0) = 1.25;
*matrix_addressOf(one, 2, 0) = 6.2;
*matrix_addressOf(one, 0, 1) = 12;
*matrix_addressOf(one, 1, 1) = 1.54;
*matrix_addressOf(one, 2, 1) = 2.76;

two = matrix_initializer(2, 3);
*matrix_addressOf(two, 0, 0) = 1;
*matrix_addressOf(two, 1, 0) = 8.88;
*matrix_addressOf(two, 0, 1) = 7.75;
*matrix_addressOf(two, 1, 1) = 2.3;
*matrix_addressOf(two, 0, 2) = 1.11;
*matrix_addressOf(two, 1, 2) = 6.7;

result = matrix_initializer(2, 2);
*matrix_addressOf(result, 0, 0) = 25.9695;
*matrix_addressOf(result, 1, 0) = 127.887;
*matrix_addressOf(result, 0, 1) = 26.9986;

*matrix_addressOf(result, 1, 1) = 128.594;

multi = matrix_multiply(one, two);

ASSERT(matrix_equals(multi, result));
matrix_destroy(one);
matrix_destroy(two);
matrix_destroy(multi);
matrix_destroy(result);
END_TEST()

START_TEST("ToString")
Matrix_t *one = matrix_initializer(2, 2);
*matrix_addressOf(one, 0, 0) = 1;
*matrix_addressOf(one, 1, 0) = 2;
*matrix_addressOf(one, 0, 1) = 3;
*matrix_addressOf(one, 1, 1) = 4;

char *toString = matrix_toString(one);
ASSERT(strcmp(toString, "|1.000000,2.000000|\n|3.000000,4.000000|\n") == 0);
free(toString);
matrix_destroy(one);
END_TEST()
/* End the overall test suite */
END_TESTS()
#endif
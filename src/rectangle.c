/*
 * Author : Yannis Perrin
 * Date : 16.09.2020
 * Description : Rectangle structure and functions (uses matrices to store informations)
                 UR = Upper Right, UL = Upper Left, LR = Lower Right, LL = Lower Left
*/
#include <stdlib.h>
#include <math.h>
#include "matrix.h"
#include "rectangle.h"
#include "tests.h"

/**
 * @brief Initializes a rectangle
 */
Rectangle_t *rect_initializer(Matrix_t *ul, Matrix_t *size)
{
	Rectangle_t *rect = (Rectangle_t*)malloc(sizeof(Rectangle_t));

	rect->ul = matrix_clone(ul);
	
	//create ur
	rect->ur = matrix_clone(ul);
	*matrix_addressOf(rect->ur, 0, 0) += matrix_valueOf(size, 0, 0);

	//create lr
	rect->lr = matrix_clone(ul);
	*matrix_addressOf(rect->lr, 0, 0) += matrix_valueOf(size, 0, 0);
	*matrix_addressOf(rect->lr, 0, 1) -= matrix_valueOf(size, 0, 1);

	//create ll
	rect->ll = matrix_clone(ul);
	*matrix_addressOf(rect->ll, 0, 1) -= matrix_valueOf(size, 0, 1);

	//calculate x, y, width and height
	// rect.x = matrix_valueOf(&rect.ul, 0, 0);
	// rect.y = matrix_valueOf(&rect.ul, 0, 1);

	// rect.width = fabs(matrix_valueOf(&rect.ul, 0, 0) - matrix_valueOf(&rect.lr, 0, 0));
	// rect.height = fabs(matrix_valueOf(&rect.ul, 0, 1) - matrix_valueOf(&rect.lr, 0, 1));

	return rect;
}

Rectangle_t *rect_initializer_primitive(double x, double y, double width, double height)
{
	Rectangle_t *rect = (Rectangle_t*)malloc(sizeof(Rectangle_t));

	// rect.x = x;
	// rect.y = y;

	// rect.width = width;
	// rect.height = height;

	//create ul
	rect->ul = matrix_initializer(1, 3);
	*matrix_addressOf(rect->ul, 0, 0) = x;
	*matrix_addressOf(rect->ul, 0, 1) = y;

	//create ur
	rect->ur = matrix_initializer(1, 3);
	*matrix_addressOf(rect->ur, 0, 0) = x + width;
	*matrix_addressOf(rect->ur, 0, 1) = y;

	//create lr
	rect->lr = matrix_initializer(1, 3);
	*matrix_addressOf(rect->lr, 0, 0) = x + width;
	*matrix_addressOf(rect->lr, 0, 1) = y - height;

	//create ll
	rect->ll = matrix_initializer(1, 3);
	*matrix_addressOf(rect->ll, 0, 0) = x;
	*matrix_addressOf(rect->ll, 0, 1) = y - height;

	return rect;
}

void rect_transform(Rectangle_t *rect, Matrix_t *t)
{
	Matrix_t *tmp;

	//modify corners and free older values
	tmp = rect->ul;
	rect->ul = matrix_multiply(t, rect->ul);
	matrix_destroy(tmp);

	tmp = rect->ur;
	rect->ur = matrix_multiply(t, rect->ur);
	matrix_destroy(tmp);

	tmp = rect->lr;
	rect->lr = matrix_multiply(t, rect->lr);
	matrix_destroy(tmp);

	tmp = rect->ll;
	rect->ll = matrix_multiply(t, rect->ll);
	matrix_destroy(tmp);

	//recalculate x, y, width and height
	// newRect.x = matrix_valueOf(&rect->ul, 0, 0);
	// newRect.y = matrix_valueOf(&rect->ul, 0, 1);

	// newRect.width = fabs(matrix_valueOf(&rect->ul, 0, 0) - matrix_valueOf(&rect->lr, 0, 0));
	// newRect.height = fabs(matrix_valueOf(&rect->ul, 0, 1) - matrix_valueOf(&rect->lr, 0, 1));
}

bool rect_axis_projection_overlap(Matrix_t *axis, Rectangle_t *a, Rectangle_t *b)
{
	Matrix_t *aulP, *aurP, *alrP, *allP, *bulP, *burP, *blrP, *bllP;
	double tmp[4];
	double maxA, minA, maxB, minB;

	double den = pow(matrix_valueOf(axis, 0, 0), 2) + pow(matrix_valueOf(axis, 0, 1), 2);
	double delta = (matrix_valueOf(a->ul, 0, 0) * matrix_valueOf(axis, 0, 0) + matrix_valueOf(a->ul, 0, 1) * matrix_valueOf(axis, 0, 1)) / den;
	aulP = matrix_multiply_double(axis, delta);

	delta = (matrix_valueOf(a->ur, 0, 0) * matrix_valueOf(axis, 0, 0) + matrix_valueOf(a->ur, 0, 1) * matrix_valueOf(axis, 0, 1)) / den;
	aurP = matrix_multiply_double(axis, delta);

	delta = (matrix_valueOf(a->lr, 0, 0) * matrix_valueOf(axis, 0, 0) + matrix_valueOf(a->lr, 0, 1) * matrix_valueOf(axis, 0, 1)) / den;
	alrP = matrix_multiply_double(axis, delta);

	delta = (matrix_valueOf(a->ll, 0, 0) * matrix_valueOf(axis, 0, 0) + matrix_valueOf(a->ll, 0, 1) * matrix_valueOf(axis, 0, 1)) / den;
	allP = matrix_multiply_double(axis, delta);

	tmp[0] = matrix_vector2_dot_product(aulP, axis);
	tmp[1] = matrix_vector2_dot_product(aurP, axis);
	tmp[2] = matrix_vector2_dot_product(alrP, axis);
	tmp[3] = matrix_vector2_dot_product(allP, axis);
	//init min and max
	maxA = tmp[0];
	minA = tmp[0];

	//go through tmp array to find the min and max of them
	for (int i = 1; i < 4; i++)
	{
		if (tmp[i] > maxA)
		{
			maxA = tmp[i];
		}
		else if (tmp[i] < minA)
		{
			minA = tmp[i];
		}
	}

	delta = (matrix_valueOf(b->ul, 0, 0) * matrix_valueOf(axis, 0, 0) + matrix_valueOf(b->ul, 0, 1) * matrix_valueOf(axis, 0, 1)) / den;
	bulP = matrix_multiply_double(axis, delta);

	delta = (matrix_valueOf(b->ur, 0, 0) * matrix_valueOf(axis, 0, 0) + matrix_valueOf(b->ur, 0, 1) * matrix_valueOf(axis, 0, 1)) / den;
	burP = matrix_multiply_double(axis, delta);

	delta = (matrix_valueOf(b->lr, 0, 0) * matrix_valueOf(axis, 0, 0) + matrix_valueOf(b->lr, 0, 1) * matrix_valueOf(axis, 0, 1)) / den;
	blrP = matrix_multiply_double(axis, delta);

	delta = (matrix_valueOf(b->ll, 0, 0) * matrix_valueOf(axis, 0, 0) + matrix_valueOf(b->ll, 0, 1) * matrix_valueOf(axis, 0, 1)) / den;
	bllP = matrix_multiply_double(axis, delta);

	tmp[0] = matrix_vector2_dot_product(bulP, axis);
	tmp[1] = matrix_vector2_dot_product(burP, axis);
	tmp[2] = matrix_vector2_dot_product(blrP, axis);
	tmp[3] = matrix_vector2_dot_product(bllP, axis);

	//init min and max
	maxB = tmp[0];
	minB = tmp[0];

	//go through tmp array to find the min and max of them
	for (int i = 1; i < 4; i++)
	{
		if (tmp[i] > maxB)
		{
			maxB = tmp[i];
		}
		else if (tmp[i] < minB)
		{
			minB = tmp[i];
		}
	}

	matrix_destroy(aulP);
	matrix_destroy(aurP);
	matrix_destroy(alrP);
	matrix_destroy(allP);

	matrix_destroy(bulP);
	matrix_destroy(burP);
	matrix_destroy(blrP);
	matrix_destroy(bllP);

	//printf("B : %f < %f   |   A: %f < %f\n", minB,maxB, minA,maxA);
	//return if the projection overlaps
	return !(minB <= maxA && maxB >= minA);
}

bool rect_intersect(Rectangle_t *a, Rectangle_t *b)
{
	Matrix_t *a1, *a2, *a3, *a4;

	//check if the first axis projection overlaps
	a1 = matrix_sub(a->ur, a->ul);
	if (rect_axis_projection_overlap(a1, a, b))
	{
		matrix_destroy(a1);
		return false;
	}
	matrix_destroy(a1);

	//check if the 2nd axis projection overlaps
	a2 = matrix_sub(a->ur, a->lr);
	if (rect_axis_projection_overlap(a2, a, b))
	{
		matrix_destroy(a2);
		return false;
	}
	matrix_destroy(a2);

	//check if the 3rd axis projection overlaps
	a3 = matrix_sub(b->ur, b->ul);
	if (rect_axis_projection_overlap(a3, a, b))
	{
		matrix_destroy(a3);
		return false;
	}
	matrix_destroy(a3);

	//check if the 4th axis projection overlaps
	a4 = matrix_sub(b->ur, b->lr);
	if (rect_axis_projection_overlap(a4, a, b))
	{
		matrix_destroy(a4);
		return false;
	}
	matrix_destroy(a4);

	return true;
}

void rect_printf(Rectangle_t *rect){
	char *tmpStr;
	tmpStr = matrix_toString(rect->ul);
	printf("ul:\n");
	printf(tmpStr);
	free(tmpStr);

	tmpStr = matrix_toString(rect->ur);
	printf("ur:\n");
	printf(tmpStr);
	free(tmpStr);

	tmpStr = matrix_toString(rect->lr);
	printf("lr:\n");
	printf(tmpStr);
	free(tmpStr);

	tmpStr = matrix_toString(rect->ll);
	printf("ll:\n");
	printf(tmpStr);
	free(tmpStr);

	// printf("\nPrimitives:\n");
	// printf("x:%lf, y:%lf\nw:%lf, h:%lf\n", rect->x, rect->y, rect->width, rect->height);
}

void rect_destroy(Rectangle_t *rect)
{
	matrix_destroy(rect->ul);
	matrix_destroy(rect->ur);
	matrix_destroy(rect->lr);
	matrix_destroy(rect->ll);
}


//Build test : (mingw32-)gcc -o test.exe rectangle.c matrix.c -DUNIT_TESTS_R
#ifdef UNIT_TESTS_R
/* Start the overall test suite */
START_TESTS()

START_TEST("Initialization (UL + Size)")

Rectangle_t *newRectangle;
Matrix_t *ul, *size; //(0,0) (1,5)
Matrix_t *eul, *eur, *elr, *ell;

//expected values for corners
eul = matrix_initializer(1, 3); //expect (0,0)
eur = matrix_initializer(1, 3); //expect (1,0)
elr = matrix_initializer(1, 3); //expect (1,-5)
ell = matrix_initializer(1, 3); //expect (0,-5)

*matrix_addressOf(eur, 0, 0) = 1;

*matrix_addressOf(elr, 0, 0) = 1;
*matrix_addressOf(elr, 0, 1) = -5;

*matrix_addressOf(ell, 0, 1) = -5;

//initialise the rectangle
ul = matrix_initializer(1, 3);
size = matrix_initializer(1, 3);

*matrix_addressOf(size, 0, 0) = 1;
*matrix_addressOf(size, 0, 1) = 5;

newRectangle = rect_initializer(ul, size);

ASSERT(matrix_equals(newRectangle->ul, eul));
ASSERT(matrix_equals(newRectangle->ur, eur));
ASSERT(matrix_equals(newRectangle->lr, elr));
ASSERT(matrix_equals(newRectangle->ll, ell));

//free everything
rect_destroy(newRectangle);

matrix_destroy(eul);
matrix_destroy(eur);
matrix_destroy(elr);
matrix_destroy(ell);

matrix_destroy(ul);
matrix_destroy(size);

END_TEST()
START_TEST("Initialization (X Y WIDTH HEIGHT)")

Rectangle_t *newRectangle;
Matrix_t *eul, *eur, *elr, *ell;

//expected values for corners
eul = matrix_initializer(1, 3); //expect (0,0)
eur = matrix_initializer(1, 3); //expect (1,0)
elr = matrix_initializer(1, 3); //expect (1,-5)
ell = matrix_initializer(1, 3); //expect (0,-5)

*matrix_addressOf(eur, 0, 0) = 1;

*matrix_addressOf(elr, 0, 0) = 1;
*matrix_addressOf(elr, 0, 1) = -5;

*matrix_addressOf(ell, 0, 1) = -5;

//initialise the rectangle
newRectangle = rect_initializer_primitive(0, 0, 1, 5);

ASSERT(matrix_equals(newRectangle->ul, eul));
ASSERT(matrix_equals(newRectangle->ur, eur));
ASSERT(matrix_equals(newRectangle->lr, elr));
ASSERT(matrix_equals(newRectangle->ll, ell));

//free everything
rect_destroy(newRectangle);

matrix_destroy(eul);
matrix_destroy(eur);
matrix_destroy(elr);
matrix_destroy(ell);

END_TEST()
/* End the overall test suite */
END_TESTS()
#endif
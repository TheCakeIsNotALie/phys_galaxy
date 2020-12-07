/*
Author : Yannis Perrin
Date : 16.09.2020
Description : Rectangle structure and functions (uses matrices to store informations)
*/
#include <stdbool.h>
#include "matrix.h"

#pragma once

//Rectangle 
typedef struct Rectangle_s {
	Matrix_t *ul;
    Matrix_t *ur;
    Matrix_t *lr;
    Matrix_t *ll;
    // double x;
    // double y;
    // double width;
    // double height;
} Rectangle_t;

/**
 * @brief Initializes a new Rectangle_t at the given pointer from a 2d vector Matrix_t(1, 3) representing the upper left corner,
 * and another 2d vector Matrix_t(1, 3) representing the size.
 * @return void
 */
Rectangle_t *rect_initializer(Matrix_t *ul, Matrix_t *size);
/**
 * @brief Initializes a new Rectangle_t at the given pointer from the primitive data (x, y, width, height).
 * @return void
 */
Rectangle_t *rect_initializer_primitive(double x, double y, double width, double height);

/**
 * @brief Transforms a rectangle data with a (2D) transformation matrix.
 * @return Rectangle_t The transformed rectangle
 */
void rect_transform(Rectangle_t *rect, Matrix_t *t);
/**
 * @brief Check if two rectangles projections on an axis overlap.
 * Used resources from https://www.gamedev.net/articles/programming/general-and-gameplay-programming/2d-rotated-rectangle-collision-r2604/ to develop function.
 * @return True or false
 */
bool rect_axis_projection_overlap(Matrix_t *axis, Rectangle_t *a, Rectangle_t *b);
/**
 * @brief Check if two rotated 2d rectangles intersect
 * Used resources from https://www.gamedev.net/articles/programming/general-and-gameplay-programming/2d-rotated-rectangle-collision-r2604/ to develop function.
 * @return True or false
 */
bool rect_intersect(Rectangle_t *a, Rectangle_t *b);

/**
 * @brief Free the different matrices used in the Rectangle_t.
 * @return void
 */
void rect_destroy(Rectangle_t *rect);

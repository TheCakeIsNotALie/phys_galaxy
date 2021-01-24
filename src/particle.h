/*
Author : Yannis Perrin
Date : 30.11.2020
Description : Particle structure
*/

#include "matrix.h"
#include <math.h>
#define G (6.67430 * pow(10, -11))

#pragma once

typedef struct Particle_s
{
    Matrix_t *lastPos;
    Matrix_t *pos;
    double mass;
} Particle_t;

Particle_t *particle_initializer(Matrix_t *lastPos, Matrix_t *pos, double mass);

void particle_updatePosition(Particle_t *particle, Matrix_t *position);
void particle_changeMass(Particle_t *particle, double massChange);
void particle_print(Particle_t *particle);
void particle_destroy(Particle_t *particle);

/**
 * @brief Get the gravitational force acting on the first particle from the second particle.
 * @return Matrix_t* the gravitational force as a 2d vector.
 */
Matrix_t *gravitational_force(Particle_t *first, Particle_t *second);
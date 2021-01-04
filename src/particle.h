/*
Author : Yannis Perrin
Date : 30.11.2020
Description : Particle structure
*/
#include "matrix.h"

#pragma once

typedef struct Particle_s
{
    Matrix_t *pos;
    Matrix_t *speed;
    double mass;
} Particle_t;

Particle_t *particle_initializer(Matrix_t *pos, Matrix_t *speed, double mass);

void particle_addSpeed(Particle_t *particle, Matrix_t *speed);
void particle_addPosition(Particle_t *particle, Matrix_t *position);
void particle_addMass(Particle_t *particle, double mass);
void particle_destroy(Particle_t *particle);
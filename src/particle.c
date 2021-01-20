#include "matrix.h"
#include "particle.h"
#include <stdlib.h>
#include "tests.h"

Particle_t *particle_initializer(Matrix_t *pos, Matrix_t *speed, double mass)
{
    Particle_t *newParticle = (Particle_t *)malloc(sizeof(Particle_t));

    newParticle->pos = matrix_clone(pos);
    newParticle->speed = matrix_clone(speed);
    newParticle->mass = mass;

    return newParticle;
}

void particle_addSpeed(Particle_t *particle, Matrix_t *speed)
{
    Matrix_t *old = particle->speed;

    particle->speed = matrix_add(particle->speed, speed);
    
    matrix_destroy(old);
}

void particle_addPosition(Particle_t *particle, Matrix_t *position)
{
    Matrix_t *old = particle->pos;

    particle->pos = matrix_add(particle->pos, position);

    matrix_destroy(old);
}

void particle_addMass(Particle_t *particle, double mass)
{
    particle->mass += mass;
}

void particle_print(Particle_t *particle)
{
    char *tmp = matrix_toString(particle->pos);
	printf("Position : \n");
    printf(tmp);
    free(tmp);
    tmp = matrix_toString(particle->speed);
    printf("Speed : \n");
    printf(tmp);
    free(tmp);
    printf("Mass : %lf\n", particle->mass);
}

void particle_destroy(Particle_t *particle)
{
    matrix_destroy(particle->pos);
    matrix_destroy(particle->speed);
}

//Build test : (mingw32-)gcc -o test.exe matrix.c -DUNIT_TESTS_M
#ifdef UNIT_TESTS_P
/* Start the overall test suite */
START_TESTS()
START_TEST("Initialization")
Particle_t *g_particles = (Particle_t *)calloc(100, sizeof(Particle_t));
Matrix_t *pos, *speed;
INITIALISE_MATRIX_VECTOR2(pos, 10, 10)
INITIALISE_MATRIX_VECTOR2(speed, 3, 3)
Particle_t *newParticle = particle_initializer(pos, speed, 10);

char *tmpStr = "";

for (size_t i = 0; i < 100; i++)
{
    Matrix_t *tmpPos, *tmpSpeed;
    INITIALISE_MATRIX_VECTOR2(tmpPos, rand() % 50, rand() % 50)
    INITIALISE_MATRIX_VECTOR2(tmpSpeed, 0, 0)
    g_particles[i] = *particle_initializer(tmpPos, tmpSpeed, (rand() % 10) + 1);
}
for (size_t i = 0; i < 100; i++)
{
    tmpStr = matrix_toString(g_particles[i].pos);
    printf("%s", tmpStr);
    free(tmpStr);

    tmpStr = matrix_toString(g_particles[i].speed);
    printf("%s", tmpStr);
    printf("%f\n", g_particles[i].mass);
    free(tmpStr);
}

ASSERT(true);

particle_destroy(newParticle);
matrix_destroy(pos);
matrix_destroy(speed);
END_TEST()
/* End the overall test suite */
END_TESTS()
#endif
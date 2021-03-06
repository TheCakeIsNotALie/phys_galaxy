#include "matrix.h"
#include "particle.h"
#include <stdlib.h>
#include "tests.h"

Particle_t *particle_initializer(Matrix_t *lastPos, Matrix_t *pos, double mass)
{
    Particle_t *newParticle = (Particle_t *)malloc(sizeof(Particle_t));

    newParticle->pos = matrix_clone(pos);
    newParticle->lastPos = matrix_clone(lastPos);
    newParticle->mass = mass;

    return newParticle;
}

void particle_updatePosition(Particle_t *particle, Matrix_t *position)
{
    Matrix_t *old = particle->lastPos;

    particle->lastPos = particle->pos;
    particle->pos = matrix_clone(position);

    matrix_destroy(old);
}

void particle_changeMass(Particle_t *particle, double massChange)
{
    particle->mass += massChange;
}

void particle_print(Particle_t *particle)
{
    char *tmp = matrix_toString(particle->lastPos);
	printf("Last position : \n");
    printf(tmp);
    free(tmp);
    tmp = matrix_toString(particle->pos);
    printf("Position : \n");
    printf(tmp);
    free(tmp);
    printf("Mass : %lf\n", particle->mass);
}

void particle_destroy(Particle_t *particle)
{
    matrix_destroy(particle->lastPos);
    matrix_destroy(particle->pos);
}

Matrix_t *gravitational_force(Particle_t *first, Particle_t *second)
{
	Matrix_t *diff, *diffMult;
	double mult;

	//calculations
	diff = matrix_sub(second->pos, first->pos);
	mult = G * ((first->mass * second->mass) / pow(matrix_vector2_magnitude(diff), 3));
	diffMult = matrix_vector2_multiply_double(diff, mult);

	//cleanup
	matrix_destroy(diff);

	return diffMult;
}

//Build test : (mingw32-)gcc -o test.exe matrix.c -DUNIT_TESTS_P
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
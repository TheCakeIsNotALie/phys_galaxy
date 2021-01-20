#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "matrix.h"
#include "rectangle.h"
#include "particle.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <time.h>

#define E_PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062

//debug var
char *tmpStrDebug;
#define DEBUG_PRINT_MATRIX(var)         \
	tmpStrDebug = matrix_toString(var); \
	printf(tmpStrDebug);                \
	free(tmpStrDebug);

#define NB_PARTICLES 250

#define INITIAL_WINDOW_HEIGHT 720
#define INITIAL_WINDOW_WIDTH 1280
#define SCALE 1
#define MIN_BOUND_X (-(INITIAL_WINDOW_WIDTH * SCALE / 2))  //left most value possible for x
#define MAX_BOUND_X (INITIAL_WINDOW_WIDTH * SCALE / 2)	   //right most value possible for x
#define MIN_BOUND_Y (-(INITIAL_WINDOW_HEIGHT * SCALE / 2)) //bottom most value possible for y
#define MAX_BOUND_Y (INITIAL_WINDOW_HEIGHT * SCALE / 2)	   //top most value possible for y
#define MIN_MASS (1 * SCALE)
#define MAX_MASS (10 * SCALE)
#define TIME_DIALATION (25 * SCALE)
#define G (6.67430 * pow(10, -11))

Uint64 NOW = 0;
Uint64 LAST = 0;
double deltaTime = 0;

Matrix_t *g_origin;
Particle_t *g_particles;
Particle_t *g_black_hole;
int g_window_width = INITIAL_WINDOW_WIDTH, g_window_height = INITIAL_WINDOW_HEIGHT;
bool g_press_right, g_press_left, g_press_control;

int fill_circle(SDL_Renderer *renderer, int x, int y, int radius)
{
	int origin_x = (int)matrix_valueOf(g_origin, 0, 0);
	int origin_y = (int)matrix_valueOf(g_origin, 0, 1);
	int offsetx, offsety, d;
	int status;

	offsetx = 0;
	offsety = radius;
	d = radius - 1;
	status = 0;

	while (offsety >= offsetx)
	{

		status += SDL_RenderDrawLine(renderer, origin_x + x - offsety, origin_y - y + offsetx, origin_x + x + offsety, origin_y - y + offsetx);
		status += SDL_RenderDrawLine(renderer, origin_x + x - offsetx, origin_y - y + offsety, origin_x + x + offsetx, origin_y - y + offsety);
		status += SDL_RenderDrawLine(renderer, origin_x + x - offsetx, origin_y - y - offsety, origin_x + x + offsetx, origin_y - y - offsety);
		status += SDL_RenderDrawLine(renderer, origin_x + x - offsety, origin_y - y - offsetx, origin_x + x + offsety, origin_y - y - offsetx);
		if (status < 0)
		{
			status = -1;
			break;
		}

		if (d >= 2 * offsetx)
		{
			d -= 2 * offsetx + 1;
			offsetx += 1;
		}
		else if (d < 2 * (radius - offsety))
		{
			d += 2 * offsety - 1;
			offsety -= 1;
		}
		else
		{
			d += 2 * (offsety - offsetx - 1);
			offsety -= 1;
			offsetx += 1;
		}
	}

	return status;
}

/**
 * @brief Get the gravitational force acting on the first particle from the second particle.
 * @return Matrix_t* the gravitational force as a 2d vector.
 */
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

/**
 * @brief Updates the physics values of every particles currently in the simulation.
 */
void PhysicsUpdate()
{
	//for every particle
	for (size_t i = 0; i < NB_PARTICLES; i++)
	{
		//declare temporary variables
		Matrix_t *tmpTotalForce, *old;
		INITIALISE_MATRIX_VECTOR2(tmpTotalForce, 0, 0)
		Matrix_t *tmpForce;

		//calculate the gravity forces with every other particle
		for (size_t j = 0; j < NB_PARTICLES; j++)
		{
			if (i != j)
			{
				tmpForce = gravitational_force(&g_particles[i], &g_particles[j]);

				old = tmpTotalForce;
				tmpTotalForce = matrix_add(tmpTotalForce, tmpForce);

				//cleanup
				matrix_destroy(old);
				matrix_destroy(tmpForce);
			}
		}
		//calculate the gravity force with the black hole
		tmpForce = gravitational_force(&g_particles[i], g_black_hole);

		old = tmpTotalForce;
		tmpTotalForce = matrix_add(tmpTotalForce, tmpForce); // kg * m/s^2

		//translate force to acceleration then to speed
		Matrix_t *acceleration = matrix_vector2_multiply_double(tmpTotalForce, 1 / g_particles[i].mass); // m/s^2
		Matrix_t *speed = matrix_vector2_multiply_double(acceleration, deltaTime * TIME_DIALATION);		 // m/s

		//update speed
		particle_addSpeed(&g_particles[i], speed);

		//cleanup
		matrix_destroy(speed);
		matrix_destroy(acceleration);
		matrix_destroy(tmpForce);
		matrix_destroy(old);
		matrix_destroy(tmpTotalForce);
	}

	//change the position of every particle
	for (size_t i = 0; i < NB_PARTICLES; i++)
	{
		Matrix_t *deltaMove = matrix_multiply_double(g_particles[i].speed, deltaTime * TIME_DIALATION); // m
		particle_addPosition(&g_particles[i], deltaMove);

		//cleanup
		matrix_destroy(deltaMove);
	}
}

/**
 * @brief Render all of the simulation.
 */
void Render(SDL_Renderer *renderer)
{
	Matrix_t *size;
	INITIALISE_MATRIX_VECTOR2(size, 5, 5)

	//draw every particles
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	for (size_t i = 0; i < NB_PARTICLES; i++)
	{
		fill_circle(renderer, matrix_valueOf(g_particles[i].pos, 0, 0) / SCALE, matrix_valueOf(g_particles[i].pos, 0, 1) / SCALE, g_particles[i].mass / SCALE);
		//SDL_RenderDrawPoint(renderer, matrix_valueOf(g_origin, 0, 0) + matrix_valueOf(g_particles[i].pos, 0, 0) / SCALE, matrix_valueOf(g_origin, 0, 1) +  matrix_valueOf(g_particles[i].pos, 0, 1) / SCALE);
	}

	//draw the black hole
	SDL_SetRenderDrawColor(renderer, 100, 100, 100, 128);
	fill_circle(renderer, matrix_valueOf(g_black_hole->pos, 0, 0), matrix_valueOf(g_black_hole->pos, 0, 1), 5);
}

int main(int argc, char *argv[])
{
	// ----- SDL INITIALIZATION ------
	int runSDL = 1;
	SDL_Event event;
	char fpsBuffer[50];

	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		printf("SDL_Init Error: %s\n", SDL_GetError());
		return 1;
	}

	if (TTF_Init() != 0)
	{
		printf("TTF_Init Error: %s\n", TTF_GetError());
		SDL_Quit();
		return 1;
	}

	SDL_Window *win = SDL_CreateWindow("Particles Simulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, g_window_width, g_window_height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (win == NULL)
	{
		printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
		TTF_Quit();
		SDL_Quit();
		return 1;
	}

	SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren == NULL)
	{
		printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
		SDL_DestroyWindow(win);
		TTF_Quit();
		SDL_Quit();
		return 1;
	}
	//init text variables
	TTF_Font *arial = TTF_OpenFont("./resources/fonts/arial.ttf", 9); //this opens a font style and sets a size

	if (arial == NULL)
	{
		fprintf(stderr, "error: font not found\n");
		SDL_DestroyRenderer(ren);
		SDL_DestroyWindow(win);
		TTF_Quit();
		SDL_Quit();
		return 1;
	}
	// ----- END SDL INITIALIZATION -----

	SDL_Color white = {255, 255, 255, 255}; //color in rgba format

	SDL_Rect fps_rect = {.x = 0, .y = 0, .w = 30, .h = 50};

	//init origin
	g_origin = matrix_initializer(1, 2);
	*matrix_addressOf(g_origin, 0, 0) = (double)g_window_width / 2.0;
	*matrix_addressOf(g_origin, 0, 1) = (double)g_window_height / 2.0;

	//set random seed
	srand(time(NULL));

	//init black hole
	Matrix_t *zero;
	INITIALISE_MATRIX_VECTOR2(zero, 0, 0)
	g_black_hole = particle_initializer(zero, zero, pow(10, 11));

	//init particles
	g_particles = (Particle_t *)calloc(NB_PARTICLES, sizeof(Particle_t));
	for (size_t i = 0; i < NB_PARTICLES; i++)
	{
		Matrix_t *tmpPos, *tmpSpeed;

		INITIALISE_MATRIX_VECTOR2(tmpPos, rand() % (MAX_BOUND_X - MIN_BOUND_X) + MIN_BOUND_X, rand() % (MAX_BOUND_Y - MIN_BOUND_Y) + MIN_BOUND_Y)

		//initialise particle with no speed
		g_particles[i] = *particle_initializer(tmpPos, zero, rand() % (MAX_MASS - MIN_MASS) + MIN_MASS);

		//compute the orbital velocity and angle needed for a circular orbit
		Matrix_t *force = gravitational_force(&g_particles[i], g_black_hole);
		Matrix_t *acceleration = matrix_vector2_multiply_double(force, 1 / g_particles[i].mass);

		double orbitalVelocity = sqrt(matrix_vector2_magnitude(acceleration) * matrix_vector2_distance(tmpPos, g_black_hole->pos));
		double angle = atan2(matrix_valueOf(tmpPos, 0, 1), matrix_valueOf(tmpPos, 0, 0)) + E_PI / 2;

		INITIALISE_MATRIX_VECTOR2(tmpSpeed, orbitalVelocity * cos(angle), orbitalVelocity * sin(angle))

		particle_addSpeed(&g_particles[i], tmpSpeed);

		matrix_destroy(force);
		matrix_destroy(acceleration);
		matrix_destroy(tmpPos);
		matrix_destroy(tmpSpeed);
	}

	printf("Start main SDL loop\n");
	NOW = SDL_GetPerformanceCounter();
	while (runSDL)
	{
		LAST = NOW;
		NOW = SDL_GetPerformanceCounter();
		deltaTime = (double)((NOW - LAST) / (double)SDL_GetPerformanceFrequency());

		//Events handling
		while (SDL_PollEvent(&event) > 0)
		{
			switch (event.type)
			{
			case SDL_QUIT:
				runSDL = 0;
				break;
			case SDL_WINDOWEVENT:
				switch (event.window.event)
				{
				case SDL_WINDOWEVENT_RESIZED:
					g_window_width = event.window.data1;
					g_window_height = event.window.data2;

					*matrix_addressOf(g_origin, 0, 0) = (double)g_window_width / 2.0;
					*matrix_addressOf(g_origin, 0, 1) = (double)g_window_height / 2.0;

					SDL_SetWindowSize(win, g_window_width, g_window_height);
					break;
				default:
					printf("Window event not processed\n");
					break;
				}
				break;
			default:
				//printf("Event not processed\n");
				break;
			}
		}

		// Set render color to black
		SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);

		// Clear the entire screen to our selected color.
		SDL_RenderClear(ren);

		//FPS counter
		snprintf(fpsBuffer, 50, "%.0f", (1.0 / deltaTime));
		SDL_Surface *fpsMessage = TTF_RenderText_Solid(arial, fpsBuffer, white);

		SDL_Texture *fpsTexture = SDL_CreateTextureFromSurface(ren, fpsMessage);
		SDL_FreeSurface(fpsMessage);

		SDL_RenderCopy(ren, fpsTexture, NULL, &fps_rect);

		//Physics and render
		PhysicsUpdate();
		Render(ren);

		SDL_RenderPresent(ren);
		SDL_DestroyTexture(fpsTexture);
	}

	// SDL Cleanup
	TTF_CloseFont(arial);
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	TTF_Quit();
	SDL_Quit();

	// app variables cleanup
	matrix_destroy(g_origin);
	matrix_destroy(zero);
	particle_destroy(g_black_hole);
	for (size_t i = 0; i < NB_PARTICLES; i++)
	{
		particle_destroy(&g_particles[i]);
	}

	return 0;
}
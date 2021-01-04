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

#define MAX_THREADS 4
#define NB_PARTICLES 20

#define MIN_BOUND_X -400
#define MAX_BOUND_X 400
#define MIN_BOUND_Y -400
#define MAX_BOUND_Y 400
#define MAX_MASS 5
#define MAX_SPEED 10
#define TIME_DIALATION 0.001
#define G 6.67

Uint64 NOW = 0;
Uint64 LAST = 0;
double deltaTime = 0;

Matrix_t *g_origin;
Particle_t *g_particles;
Particle_t *g_black_hole;
int g_window_width = 640, g_window_height = 480;
bool g_press_right, g_press_left, g_press_control;

/**
 * @brief Draw a Rectangle with it's four points
 * @return Void
 */
void draw_rectangle(SDL_Renderer *renderer, Rectangle_t *rect)
{
	// Render rect
	int origin_x = (int)matrix_valueOf(g_origin, 0, 0);
	int origin_y = (int)matrix_valueOf(g_origin, 0, 1);

	SDL_RenderDrawLine(renderer, origin_x + (int)matrix_valueOf(rect->ul, 0, 0), origin_y - (int)matrix_valueOf(rect->ul, 0, 1), origin_x + (int)matrix_valueOf(rect->ur, 0, 0), origin_y - (int)matrix_valueOf(rect->ur, 0, 1));
	SDL_RenderDrawLine(renderer, origin_x + (int)matrix_valueOf(rect->ur, 0, 0), origin_y - (int)matrix_valueOf(rect->ur, 0, 1), origin_x + (int)matrix_valueOf(rect->lr, 0, 0), origin_y - (int)matrix_valueOf(rect->lr, 0, 1));
	SDL_RenderDrawLine(renderer, origin_x + (int)matrix_valueOf(rect->lr, 0, 0), origin_y - (int)matrix_valueOf(rect->lr, 0, 1), origin_x + (int)matrix_valueOf(rect->ll, 0, 0), origin_y - (int)matrix_valueOf(rect->ll, 0, 1));
	SDL_RenderDrawLine(renderer, origin_x + (int)matrix_valueOf(rect->ll, 0, 0), origin_y - (int)matrix_valueOf(rect->ll, 0, 1), origin_x + (int)matrix_valueOf(rect->ul, 0, 0), origin_y - (int)matrix_valueOf(rect->ul, 0, 1));
}

/**
 * @brief Draw a circle with it's bounding square
 * Expecting a rectangle with no transformation as transformations will not be applied to the circle drawing
 * @return Void
 */
void draw_circle(SDL_Renderer *renderer, Rectangle_t *rect)
{
	int origin_x = (int)matrix_valueOf(g_origin, 0, 0);
	int origin_y = (int)matrix_valueOf(g_origin, 0, 1);

	Matrix_t *size;

	size = matrix_sub(rect->ul, rect->lr);

	*matrix_addressOf(size, 0, 0) = fabs(matrix_valueOf(size, 0, 0));
	*matrix_addressOf(size, 0, 1) = fabs(matrix_valueOf(size, 0, 1));

	int centerX = (int)(matrix_valueOf(rect->ul, 0, 0) + matrix_valueOf(size, 0, 0) / 2.0);
	int centerY = (int)(matrix_valueOf(rect->ul, 0, 1) - matrix_valueOf(size, 0, 1) / 2.0);
	int diameter = matrix_valueOf(size, 0, 0);
	int x = ((int)(diameter / 2.0) - 1);
	int y = 0;
	int tx = 1;
	int ty = 1;
	int error = (tx - diameter);

	while (x >= y)
	{
		//  Each of the following renders an octant of the circle
		SDL_RenderDrawPoint(renderer, origin_x + centerX + x, origin_y - centerY - y);
		SDL_RenderDrawPoint(renderer, origin_x + centerX + x, origin_y - centerY + y);
		SDL_RenderDrawPoint(renderer, origin_x + centerX - x, origin_y - centerY - y);
		SDL_RenderDrawPoint(renderer, origin_x + centerX - x, origin_y - centerY + y);
		SDL_RenderDrawPoint(renderer, origin_x + centerX + y, origin_y - centerY - x);
		SDL_RenderDrawPoint(renderer, origin_x + centerX + y, origin_y - centerY + x);
		SDL_RenderDrawPoint(renderer, origin_x + centerX - y, origin_y - centerY - x);
		SDL_RenderDrawPoint(renderer, origin_x + centerX - y, origin_y - centerY + x);

		if (error <= 0)
		{
			++y;
			error += ty;
			ty += 2;
		}

		if (error > 0)
		{
			--x;
			tx += 2;
			error += (tx - diameter);
		}
	}

	matrix_destroy(size);
}

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

		status += SDL_RenderDrawLine(renderer, origin_x + x - offsety, origin_y + y + offsetx, origin_x + x + offsety, origin_y + y + offsetx);
		status += SDL_RenderDrawLine(renderer, origin_x + x - offsetx, origin_y + y + offsety, origin_x + x + offsetx, origin_y + y + offsety);
		status += SDL_RenderDrawLine(renderer, origin_x + x - offsetx, origin_y + y - offsety, origin_x + x + offsetx, origin_y + y - offsety);
		status += SDL_RenderDrawLine(renderer, origin_x + x - offsety, origin_y + y - offsetx, origin_x + x + offsety, origin_y + y - offsetx);
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

void PhysicsUpdate()
{
	//for every particle
	for (size_t i = 0; i < NB_PARTICLES; i++)
	{
		//declare temporary variables
		Matrix_t *newSpeed, *tmp;
		INITIALISE_MATRIX_VECTOR2(tmp, 0, 0)
		Matrix_t *diff, *diffMult;
		double mult;

		//calculate the gravity forces with every other particle
		for (size_t j = 0; j < NB_PARTICLES; j++)
		{
			if (i != j)
			{
				diff = matrix_sub(g_particles[j].pos, g_particles[i].pos);
				mult = G * ((g_particles[i].mass * g_particles[j].mass) / pow(matrix_vector2_magnitude(diff), 3));
				diffMult = matrix_multiply_double(diff, mult);
				tmp = matrix_add(tmp, diff);

				matrix_destroy(diff);
				matrix_destroy(diffMult);
			}
		}
		//calculate the gravity force with the black hole
		diff = matrix_sub(g_black_hole->pos, g_particles[i].pos);
		mult = G * ((g_particles[i].mass * g_black_hole->mass) / pow(matrix_vector2_magnitude(diff), 3));
		diffMult = matrix_multiply_double(diff, mult);
		tmp = matrix_add(tmp, diff);

		//fix the changes to the deltatime
		newSpeed = matrix_multiply_double(tmp, deltaTime * TIME_DIALATION);
		particle_addSpeed(&g_particles[i], newSpeed);

		matrix_destroy(newSpeed);
		matrix_destroy(tmp);
	}

	//change the position of every particle
	for (size_t i = 0; i < NB_PARTICLES; i++)
	{
		particle_addPosition(&g_particles[i], g_particles[i].speed);
	}
}

void Render(SDL_Renderer *renderer)
{
	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
	Matrix_t *size;
	INITIALISE_MATRIX_VECTOR2(size, 5, 5)
	//Rectangle_t *rect;

	for (size_t i = 0; i < NB_PARTICLES; i++)
	{
		// rect = rect_initializer(g_particles[i].pos, size);
		// draw_circle(renderer, rect);
		fill_circle(renderer, matrix_valueOf(g_particles[i].pos, 0, 0), matrix_valueOf(g_particles[i].pos, 0, 1), g_particles[i].mass);
		//rect_destroy(rect);
	}

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 128);
	fill_circle(renderer, matrix_valueOf(g_black_hole->pos, 0, 0), matrix_valueOf(g_black_hole->pos, 0, 1), 10);
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

	// //set anti alias (NOT WORKING)
	// SDL_GLContext context;
	// SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	// SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
	// SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	// SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	// glEnable(GL_MULTISAMPLE);

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

	SDL_Rect fps_rect;
	fps_rect.x = 0;
	fps_rect.y = 0;
	fps_rect.w = 30; //width
	fps_rect.h = 50; //height

	//init origin
	g_origin = matrix_initializer(1, 2);
	*matrix_addressOf(g_origin, 0, 0) = (double)g_window_width / 2.0;
	*matrix_addressOf(g_origin, 0, 1) = (double)g_window_height / 2.0;

	//set random seed
	srand(time(NULL));

	//init black hole
	Matrix_t *zero;
	INITIALISE_MATRIX_VECTOR2(zero, 0, 0)
	g_black_hole = particle_initializer(zero, zero, 100000);

	//init particles
	g_particles = (Particle_t *)calloc(NB_PARTICLES, sizeof(Particle_t));
	for (size_t i = 0; i < NB_PARTICLES; i++)
	{
		Matrix_t *tmpPos, *tmpSpeed;
		INITIALISE_MATRIX_VECTOR2(tmpPos, rand() % (MAX_BOUND_X - MIN_BOUND_X) + MIN_BOUND_X, rand() % (MAX_BOUND_Y - MIN_BOUND_Y) + MIN_BOUND_Y)
		INITIALISE_MATRIX_VECTOR2(tmpSpeed, (rand() % MAX_SPEED) - MAX_SPEED / 2, (rand() % MAX_SPEED) - MAX_SPEED / 2)
		g_particles[i] = *particle_initializer(tmpPos, tmpSpeed, (rand() % MAX_MASS) + 1);
	}

	//start main SDL loop
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
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case SDLK_a:
					g_press_left = true;
					break;
				case SDLK_d:
					g_press_right = true;
					break;
				case SDLK_LSHIFT:
					g_press_control = true;
					break;
				default:
					printf("Key down not processed\n");
					break;
				}
				break;
			case SDL_KEYUP:
				switch (event.key.keysym.sym)
				{
				case SDLK_a:
					g_press_left = false;
					break;
				case SDLK_d:
					g_press_right = false;
					break;
				case SDLK_LSHIFT:
					g_press_control = false;
					break;
				default:
					printf("Key up not processed\n");
					break;
				}
				break;
			default:
				printf("Event not processed\n");
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

		PhysicsUpdate();
		Render(ren);

		// Render the rect to the screen
		SDL_RenderPresent(ren);

		SDL_DestroyTexture(fpsTexture);
		//SDL_Delay(500);
	}

	// SDL Cleanup
	TTF_CloseFont(arial);
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	TTF_Quit();
	SDL_Quit();

	// app variables cleanup
	matrix_destroy(g_origin);
	particle_destroy(g_black_hole);
	for (size_t i = 0; i < NB_PARTICLES; i++)
	{
		particle_destroy(&g_particles[i]);
	}

	return 0;
}
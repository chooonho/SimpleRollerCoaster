/*******************************************************************************
*	Author				: OOI CHOON HO
*	Date				: 05/12/2016
*******************************************************************************/

#pragma once

#ifndef _WEATHER_CONTROL_H
#define _WEATHER_CONTROL_H

#define MAX_PARTICLES 1000

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <GL/glut.h>
#include <GL/gl.h>

struct Particle {
	// Life
	bool alive;	// particle status
	float life;	// particle lifespan
	float fade; // decay rate
	// Color
	float red;
	float green;
	float blue;
	// Position/direction
	float xpos;
	float ypos;
	float zpos;
	// Velocity/Direction, only goes down in y dir
	float velocity;
	// Gravity
	float gravity;
};

// Static veriables to keep the values alive until the application is terminated
static float slowDown = 0.5;
static float currentVelocity = 0.0;
static float zoom = -40.0;
static float hailSize = 0.1;
static float r = 0.0;
static float g = 1.0;
static float b = 0.0;
static float groundPoints[201][201][3];
static float groundColors[201][201][4];
static float accum = -10.0;
static Particle particles[MAX_PARTICLES];

// Functions
// Initialize the weather (ground vertices, colors, particles)
void initWeather();
// Initialize or reset particles
void initParticles(int i);
// Renders hail
void drawHail();

#endif
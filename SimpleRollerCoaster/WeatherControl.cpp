/*******************************************************************************
*	Author				: OOI CHOON HO
*	Date				: 05/12/2016
*******************************************************************************/

#include "WeatherControl.h"

// Initialize or reset particles
void initParticles(int i) {
	particles[i].alive = true;
	particles[i].life = 5.0;
	particles[i].fade = float(rand() % 100) / 1000.0f + 0.003f;

	particles[i].xpos = (float)(rand() % 201) - 100;
	particles[i].ypos = 50.0;
	particles[i].zpos = (float)(rand() % 201) - 100;

	particles[i].red = 0.5;
	particles[i].green = 0.5;
	particles[i].blue = 1.0;

	particles[i].velocity = currentVelocity;
	particles[i].gravity = -0.8;
}

// Initialize the weather (ground vertices, colors, particles)
void initWeather()
{
	for (int z = 0; z < 201; z++) {
		for (int x = 0; x < 201; x++) {
			groundPoints[x][z][0] = x - 100.0;
			groundPoints[x][z][1] = accum;
			groundPoints[x][z][2] = z - 100.0;

			groundColors[z][x][0] = r;
			groundColors[z][x][1] = g;
			groundColors[z][x][2] = b;
			groundColors[z][x][3] = 0.0; // acummulation factor
		}
	}

	// Initialize particles
	for (int particleCounter = 0; particleCounter < MAX_PARTICLES; particleCounter++) {
		initParticles(particleCounter);
	}
}

// Renders hail
void drawHail() {
	float x, y, z;

	glEnable(GL_COLOR_MATERIAL);
	for (int particleCounter = 0; particleCounter < MAX_PARTICLES; particleCounter = particleCounter + 2) {
		if (particles[particleCounter].alive == true) {
			x = particles[particleCounter].xpos;
			y = particles[particleCounter].ypos;
			z = particles[particleCounter].zpos + zoom;

			// Draw particles
			// Vertex sequence as follow:
			// - Lower left
			// - Upper left
			// - Upper right
			// - Lower left
			glColor3f(0.8, 0.8, 0.9);
			glBegin(GL_QUADS);

			// Front
			glVertex3f(x - hailSize, y - hailSize, z + hailSize);
			glVertex3f(x - hailSize, y + hailSize, z + hailSize);
			glVertex3f(x + hailSize, y + hailSize, z + hailSize);
			glVertex3f(x + hailSize, y - hailSize, z + hailSize);

			//Left
			glVertex3f(x - hailSize, y - hailSize, z + hailSize);
			glVertex3f(x - hailSize, y - hailSize, z - hailSize);
			glVertex3f(x - hailSize, y + hailSize, z - hailSize);
			glVertex3f(x - hailSize, y + hailSize, z + hailSize);

			// Back
			glVertex3f(x - hailSize, y - hailSize, z - hailSize);
			glVertex3f(x - hailSize, y + hailSize, z - hailSize);
			glVertex3f(x + hailSize, y + hailSize, z - hailSize);
			glVertex3f(x + hailSize, y - hailSize, z - hailSize);

			//Right
			glVertex3f(x + hailSize, y + hailSize, z + hailSize);
			glVertex3f(x + hailSize, y + hailSize, z - hailSize);
			glVertex3f(x + hailSize, y - hailSize, z - hailSize);
			glVertex3f(x + hailSize, y - hailSize, z + hailSize);

			//Top 
			glVertex3f(x - hailSize, y + hailSize, z + hailSize);
			glVertex3f(x - hailSize, y + hailSize, z - hailSize);
			glVertex3f(x + hailSize, y + hailSize, z - hailSize);
			glVertex3f(x + hailSize, y + hailSize, z + hailSize);

			//Bottom 
			glVertex3f(x - hailSize, y - hailSize, z + hailSize);
			glVertex3f(x - hailSize, y - hailSize, z - hailSize);
			glVertex3f(x + hailSize, y - hailSize, z - hailSize);
			glVertex3f(x + hailSize, y - hailSize, z + hailSize);
			glEnd();

			// Update values
			// Move
			if (particles[particleCounter].ypos <= -10) {
				particles[particleCounter].velocity = particles[particleCounter].velocity * -1.0;
			}
			particles[particleCounter].ypos += particles[particleCounter].velocity / (slowDown * 1000); // * 1000
			particles[particleCounter].velocity += particles[particleCounter].gravity;

			// Decay
			particles[particleCounter].life -= particles[particleCounter].fade;

			// Revive 
			if (particles[particleCounter].life < 0.0) {
				initParticles(particleCounter);
			}
		}
	}

	glEnable(GL_COLOR_MATERIAL);
}
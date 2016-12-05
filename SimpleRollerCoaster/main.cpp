/*******************************************************************************
*	Name				: OOI CHOON HO
*	Student ID			: 4805604
*	Coursework Title	: CSCI336 Assignment 3
*	Objective			: 3D Roller Coaster Simulator
*******************************************************************************/

#pragma comment (linker,"/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include "glext.h"
#include "Environment.h";
#include "Spline.h";
#include "WeatherControl.h"

// Global variables
GLfloat pointSource[] = { 15.0f, 20.0f, -5.0f, 1.0f };
GLfloat ambient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lModelAmbient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
GLfloat fogColor[] = { 0.5f, 0.5f, 0.5f, 1.0f };

Environment* environment = NULL;
Spline* spline = NULL;
int font = (int)GLUT_BITMAP_8_BY_13;
int segmentIndex = 0;
int cPointIndex = 0;
int counter = 0;
int sTime = 0;
int eTime = 0;
float speed = 1.0f;
float fps = 0.0f;
float avgFps = 0.0f;
long elapsedSec = 0;
bool onScreenHelpOn = true;
bool enableAntiAlias = false;
bool enableFog = false;

// Function to measure frame rate
void measureFps(int n)
{
	sTime = glutGet(GLUT_ELAPSED_TIME);

	if (sTime - eTime > 1000) {
		fps = counter * 1000.0 / (sTime - eTime);
		eTime = sTime;
		counter = 0;
		elapsedSec++;
		avgFps = (avgFps * (elapsedSec - 1) + fps) / elapsedSec;
	}

	glutTimerFunc(25, measureFps, 0);
}

// Renders a string onto the screen
void renderBitmapCharacter(int x, int y, void *font, char *string)
{
	char* characterPtr;

	glMatrixMode(GL_PROJECTION);
	glEnable(GL_COLOR_MATERIAL);
	glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0.0, 960.0, 0.0, 540.0);
		glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();
			glColor3f(1.0f, 0.0f, 0.0f);
			glRasterPos2d(x, y);
			for (characterPtr = string; *characterPtr != '\0'; characterPtr++) {
				glutBitmapCharacter(font, *characterPtr);
			}
			glColor3f(1.0f, 1.0f, 1.0f);
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glDisable(GL_COLOR_MATERIAL);
}

// Displays on screen help to assist users
void printOnScreenHelp()
{
	// Lights has to be disabled before rendering the texts to prevent it
	// from blending the color of the texts
	// Lights shall be re-enabled after the texts are rendered
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);

	renderBitmapCharacter(10, 520, (void *)font, "On-Screen Help");
	renderBitmapCharacter(10, 510, (void *)font, "------------------------------");
	renderBitmapCharacter(10, 500, (void *)font, "H - Toggle on-screen help on/off");
	if (onScreenHelpOn)
	{
		char buffer1[10] = "0.0";
		char buffer2[10] = "0.0";

		renderBitmapCharacter(10, 480, (void *)font, "Scene Control");
		renderBitmapCharacter(10, 470, (void *)font, "------------------------------");
		renderBitmapCharacter(10, 460, (void *)font, "1             - Sky Texture");
		renderBitmapCharacter(10, 445, (void *)font, "2             - Forest Texture");
		renderBitmapCharacter(10, 430, (void *)font, "3             - Night Texture");
		renderBitmapCharacter(10, 415, (void *)font, "4             - Space Texture");
		renderBitmapCharacter(10, 400, (void *)font, "t / T         - Texture on/off");
		renderBitmapCharacter(10, 385, (void *)font, "a / A         - Anti-aliasing on/off");
		renderBitmapCharacter(10, 370, (void *)font, "f / F         - Fog on/off");
		
		renderBitmapCharacter(10, 350, (void *)font, "Mode ON / OFF");
		renderBitmapCharacter(10, 340, (void *)font, "------------------------------");
		if (enableAntiAlias)
		{
			renderBitmapCharacter(10, 330, (void *)font, "Anti-alias    - ON");
		}
		else
		{
			renderBitmapCharacter(10, 330, (void *)font, "Anti-alias    - OFF");
		}

		if (enableFog)
		{
			renderBitmapCharacter(10, 315, (void *)font, "Fog           - ON");
		}
		else
		{
			renderBitmapCharacter(10, 315, (void *)font, "Fog           - OFF");
		}

		if (environment->getEnableTexture())
		{
			renderBitmapCharacter(10, 300, (void *)font, "Texture       - ON");
		}
		else
		{
			renderBitmapCharacter(10, 300, (void *)font, "Texture       - OFF");
		}

		sprintf(buffer1, "%f", fps);
		sprintf(buffer2, "%f", avgFps);
		renderBitmapCharacter(10, 280, (void *)font, "FPS");
		renderBitmapCharacter(10, 270, (void *)font, "------------------------------");
		renderBitmapCharacter(10, 260, (void *)font, "FPS           - ");
		renderBitmapCharacter(120, 260, (void *)font, buffer1);
		renderBitmapCharacter(10, 245, (void *)font, "Average FPS   - ");
		renderBitmapCharacter(120, 245, (void *)font, buffer2);

		renderBitmapCharacter(10, 30, (void *)font, "Extras");
		renderBitmapCharacter(10, 20, (void *)font, "------------------------------");
		renderBitmapCharacter(10, 10, (void *)font, "ESC            -  Quit");
	}

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}

// Decide the position of the camera, the direction it should face and look at
void NTBLookAt(Point3D cPoint, Point3D tangent, Point3D normal, Point3D biNormal)
{
	// Calculates the eye position, center and up
	Point3D eye = sumVectors(cPoint, multiplyByScalar(0.2, normal));
	eye = sumVectors(eye, multiplyByScalar(0.5 / 2, biNormal));
	Point3D center = sumVectors(eye, tangent);
	Point3D up = normal;

	// Increase the speed if the camera is facing downwards
	if (center.y < eye.y)
	{
		if (speed < 100)
		{
			speed += 0.25f;
		}
	}
	else
	{
		if (speed > 1)
		{
			speed -= 0.25f;
		}
	}

	gluLookAt(eye.x, eye.y, eye.z, center.x, center.y, center.z, up.x, up.y, up.z);
}

// Display callback function
void display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Enable / disable anti-aliasing
	if (enableAntiAlias)
	{
		glEnable(GL_MULTISAMPLE_ARB);
		glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);
	}
	else
	{
		glDisable(GL_MULTISAMPLE_ARB);
	}

	// Moves the roller coaster based on the control points
	if (cPointIndex > (spline->getMinorCPointCount(segmentIndex) - 1))
	{
		if (segmentIndex > (spline->getSegmentCount() - 1))
		{
			segmentIndex = 0;
			speed = 1.0f;
		}
		else
		{
			segmentIndex++;
		}

		cPointIndex = 0;
	}

	if (segmentIndex > (spline->getSegmentCount() - 1))
	{
		segmentIndex = 0;
		cPointIndex = 0;
		speed = 1.0f;
	}

	NTBLookAt(spline->getMinorCPoint(segmentIndex, cPointIndex), spline->getMinorCPointTangent(segmentIndex, cPointIndex),
				spline->getMinorCPointNormal(segmentIndex, cPointIndex), spline->getMinorCPointBiNormal(segmentIndex, cPointIndex));
	cPointIndex += (int)(1.0f * speed);
	
	// Draws the scene
	environment->draw();
	spline->draw();
	drawHail();

	counter++;
	printOnScreenHelp();

	glutSwapBuffers();
}

// Dispose unused pointers and release its memory
void dispose()
{
	delete environment;
	environment = NULL;

	delete spline;
	spline = NULL;
}

// Normal key callback function
void processNormalKey(unsigned char key, int x, int y)
{
	/*
	*	Instructions are as below:
	*	1	- Change texture to cloudy
	*	2	- Change texture to forest
	*	3	- Change texture to night
	*	4	- Change texture to space
	*	t/T - Enable / disable texture
	*	a/A - Enable / disable anti-aliasing
	*	f/F - Enable / disable fog
	*	h/H - Enable / disable on screen help
	*/

	switch (key)
	{
		case '1':
			environment->changeTexture(CLOUDY);
			environment->setTiledFloor(false);
			break;
		case '2':
			environment->changeTexture(FOREST);
			environment->setTiledFloor(true);
			break;
		case '3':
			environment->changeTexture(NIGHT);
			environment->setTiledFloor(true);
			break;
		case '4':
			environment->changeTexture(SPACE);
			environment->setTiledFloor(true);
			break;
		case 't': case 'T':
			environment->setEnableTexture(!environment->getEnableTexture());
			spline->setEnableTexture(!spline->getEnableTexture());
			break;
		case 'a': case 'A':
			enableAntiAlias = !enableAntiAlias;
			avgFps = 0;
			elapsedSec = 0;
			break;
		case 'f': case 'F':
			enableFog = !enableFog;
			if (enableFog)
			{
				glEnable(GL_FOG);
				glFogfv(GL_FOG_COLOR, fogColor);
				glFogf(GL_FOG_START, 5);
				glFogf(GL_FOG_END, 13);

				glFogf(GL_FOG_MODE, GL_EXP);
				glFogf(GL_FOG_DENSITY, 0.15);
			}
			else
			{
				glDisable(GL_FOG);
			}
			break;
		case 'h': case 'H':
			onScreenHelpOn = !onScreenHelpOn;
			break;
		case 27:
			dispose();
			exit(0);
			break;
	}
}

void init()
{
	glClearColor(0.0, 0.0, 0.0, 1.0);	// set background color 
	glColor3f(0.8, 0.8, 0.8);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70.0f, 1.33, 0.01, 1000);

	glutFullScreen();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, pointSource);	// set light 0 location
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lModelAmbient);	// enable global ambient light
	glEnable(GL_COLOR_MATERIAL);
	glShadeModel(GL_SMOOTH);

	environment = new Environment();
	spline = new Spline();

	initWeather();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(1280, 720);
	glutInitWindowPosition(50, 50);
	glutCreateWindow("CSCI336 Assignment 3 - Roller Coaster");
	init();
	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutKeyboardFunc(processNormalKey);
	glutTimerFunc(25, measureFps, 0);
	glutMainLoop();

	dispose();

	return 0;
}
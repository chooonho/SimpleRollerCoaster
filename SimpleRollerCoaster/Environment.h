/*******************************************************************************
*	Author				: OOI CHOON HO
*	Date				: 05/12/2016
*******************************************************************************/

#pragma once

#ifndef _ENVIRONMENT_H_
#define _ENVIRONMENT_H_

#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <iostream>

// Enumeration for textures
enum TextureCode
{
	CLOUDY,
	FOREST,
	NIGHT,
	SPACE
};

// Enumeration for sky texture orientation (textures wrapping front-left-rear-right-top)
enum SkyTextureOrientation
{
	TOP,
	FRONT,
	REAR,
	LEFT,
	RIGHT
};

class Environment
{
	private:
		// Attributes
		GLuint mFloorTexture;
		GLuint* mSkyTextures;
		bool mTiledFloor;
		bool mEnableTexture;
		TextureCode mSelectedTexture;
		// Draw the sky (includes front-left-rear-right-top)
		void drawSky();
		// Draws the floor
		void drawGround();
		// Loads texture from file
		void loadTexture();
	public:
		// Constructor / Destructor
		Environment();
		~Environment();
		// Setter and getter
		void setTiledFloor(bool tiledFloor);
		void setEnableTexture(bool applyTexture);
		bool getEnableTexture();
		// Draws the environment
		void draw();
		// Change the texture based on the texture code
		void changeTexture(TextureCode textureCode);
};

static const char* PATH_NAME_FLOOR_TEXTURE[] = { "Textures/cloudyFloor.bmp", "Textures/forestFloor.bmp", "Textures/nightFloor.bmp", "Textures/spaceFloor.bmp" };
static const char* PATH_NAME_CLOUDY_TEXTURE[] = { "Textures/cloudyTop.bmp", "Textures/cloudyFront.bmp", "Textures/cloudyRear.bmp", "Textures/cloudyLeft.bmp", "Textures/cloudyRight.bmp" };
static const char* PATH_NAME_FOREST_TEXTURE[] = { "Textures/forestTop.bmp", "Textures/forest.bmp", "Textures/forest.bmp", "Textures/forest.bmp", "Textures/forest.bmp" };
static const char* PATH_NAME_NIGHT_TEXTURE[] = { "Textures/nightTop.bmp", "Textures/nightFront.bmp", "Textures/nightRear.bmp", "Textures/nightLeft.bmp", "Textures/nightRight.bmp" };
static const char* PATH_NAME_SPACE_TEXTURE[] = { "Textures/spaceTop.bmp", "Textures/spaceFront.bmp", "Textures/spaceRear.bmp", "Textures/spaceLeft.bmp", "Textures/spaceRight.bmp" };

#endif // _ENVIRONTMENT_H_
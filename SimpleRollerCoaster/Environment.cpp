#include "Environment.h"
#include "TextureHandler.h"

// Constructor / Destructor
Environment::Environment()
{
	mFloorTexture = NULL;
	mSkyTextures = new GLuint[5];
	mSelectedTexture = CLOUDY;
	mTiledFloor = false;
	mEnableTexture = true;

	loadTexture();
}

Environment::~Environment()
{
	delete[] mSkyTextures;
}

// Draw floor
void Environment::drawGround()
{
	if (mFloorTexture == NULL)
	{
		return;
	}

	// Check if it requires tiles or not before drawing
	float maxTextureCoord = 1.0f;
	if (mTiledFloor)
	{
		maxTextureCoord = 100.0f;
	}

	if (mEnableTexture)
	{
		glBindTexture(GL_TEXTURE_2D, mFloorTexture);
	}
	
	glBegin(GL_QUADS);
		glNormal3f(0.0f, 1.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-100.0f, 0.0f, 100.0f);
		glTexCoord2f(maxTextureCoord, 0.0f); glVertex3f(100.0f, 0.0f, 100.0f);
		glTexCoord2f(maxTextureCoord, maxTextureCoord); glVertex3f(100.0f, 0.0f, -100.0f);
		glTexCoord2f(0.0f, maxTextureCoord); glVertex3f(-100.0f, 0.0f, -100.0f);
	glEnd();

	if (mEnableTexture)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

// Draw the sky (includes front-left-rear-right-top)
void Environment::drawSky()
{
	if (mSkyTextures == NULL)
	{
		return;
	}

	// Check if the texture mapping is allowed before mapping texture
	// Top
	if (mEnableTexture)
	{
		glBindTexture(GL_TEXTURE_2D, mSkyTextures[TOP]);
	}
	glBegin(GL_QUADS);
		glNormal3f(0.0f, -1.0f, 0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-100.0f, 96.0f, 100.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(100.0f, 96.0f, 100.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(100.0f, 96.0f, -100.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-100.0f, 96.0f, -100.0f);
	glEnd();

	// Front
	if (mEnableTexture)
	{
		glBindTexture(GL_TEXTURE_2D, mSkyTextures[FRONT]);
	}
	glBegin(GL_QUADS);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(100.0f, 98.0f, -100.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-100.0f, 98.0f, -100.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-100.0f, 0.0f, -100.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(100.0f, 0.0f, -100.0f);
	glEnd();

	// Rear
	if (mEnableTexture)
	{
		glBindTexture(GL_TEXTURE_2D, mSkyTextures[REAR]);
	}
	glBegin(GL_QUADS);
		glNormal3f(0.0f, 0.0f, -1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(100.0f, 98.0f, 100.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-100.0f, 98.0f, 100.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-100.0f, 0.0f, 100.0f);
		glTexCoord2f(1.0f, 0.0f);  glVertex3f(100.0f, 0.0f, 100.0f);
	glEnd();

	// Left
	if (mEnableTexture)
	{
		glBindTexture(GL_TEXTURE_2D, mSkyTextures[LEFT]);
	}
	glBegin(GL_QUADS);
		glNormal3f(1.0f, 0.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-100.0f, 0.0f, 100.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-100.0f, 0.0f, -100.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-100.0f, 98.0f, -100.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-100.0f, 98.0f, 100.0f);
	glEnd();

	 // Right
	if (mEnableTexture)
	{
		glBindTexture(GL_TEXTURE_2D, mSkyTextures[RIGHT]);
	}
	glBegin(GL_QUADS);
		glNormal3f(-1.0f, 0.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(100.0f, 0.0f, 100.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(100.0f, 0.0f, -100.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(100.0f, 98.0f, -100.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(100.0f, 98.0f, 100.0f);
	glEnd();

	if (mEnableTexture)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

// Load texture from file
void Environment::loadTexture()
{
	// Based on the selected texture
	// Calls function to load bmp file to texture the sky and floor
	for (int i = 0; i < 5; i++)
	{
		switch (mSelectedTexture)
		{
			case CLOUDY:
				mSkyTextures[i] = loadBMP(PATH_NAME_CLOUDY_TEXTURE[i]);
				break;
			case FOREST:
				mSkyTextures[i] = loadBMP(PATH_NAME_FOREST_TEXTURE[i]);
				break;
			case NIGHT:
				mSkyTextures[i] = loadBMP(PATH_NAME_NIGHT_TEXTURE[i]);
				break;
			case SPACE:
				mSkyTextures[i] = loadBMP(PATH_NAME_SPACE_TEXTURE[i]);
				break;
		}
	}

	mFloorTexture = loadBMP(PATH_NAME_FLOOR_TEXTURE[mSelectedTexture]);
}

// Public functions
// Setter and getter
void Environment::setTiledFloor(bool tiledFloor)
{
	mTiledFloor = tiledFloor;
}


void Environment::setEnableTexture(bool applyTexture)
{
	mEnableTexture = applyTexture;
}

bool Environment::getEnableTexture()
{
	return mEnableTexture;
}

// Function that draws the environment
void Environment::draw()
{
	// Check if texture is allowed before enabling it
	if (mEnableTexture)
	{
		glEnable(GL_TEXTURE_2D);
	}
	else
	{
		glEnable(GL_COLOR_MATERIAL);
		glColor3f(0.5, 0.5, 0.5);
	}

	drawGround();
	drawSky();

	if (mEnableTexture)
	{
		glDisable(GL_TEXTURE_2D);
	}
	else
	{
		glDisable(GL_COLOR_MATERIAL);
	}
}

// Change the texture based on texture code specified by user
void Environment::changeTexture(TextureCode textureCode)
{
	// Update the selected texture
	// Loads the new texture from file
	mSelectedTexture = textureCode;

	loadTexture();
}
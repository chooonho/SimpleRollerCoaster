/*******************************************************************************
*	Author				: OOI CHOON HO
*	Date				: 05/12/2016
*******************************************************************************/

#include "TextureHandler.h"

// Loads bmp file as texture
GLuint loadBMP(const char* path)
{
	// Data read from the header of the BMP file
	// Each BMP file begins by a 54-bytes header
	// Position in the file where the actual data begins
	// imageSize = width * height * 3
	// Actual RGB data
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int width, height;
	unsigned int imageSize;
	unsigned char * data;

	// Open the file
	// If not 54 bytes read : problem
	FILE * file = fopen(path, "rb");

	if (!file)
	{
		exit(0);
	}

	if (fread(header, 1, 54, file) != 54)
	{
		exit(1);
	}

	if (header[0] != 'B' || header[1] != 'M')
	{
		exit(2);
	}

	// Read ints from the byte array
	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);

	// Some BMP files are misformatted, guess missing information
	if (imageSize == 0)
	{
		imageSize = width*height * 3; // 3 : one byte for each Red, Green and Blue component
	}

	if (dataPos == 0)
	{
		dataPos = 54; // The BMP header is done that way
	}

	// Create a buffer
	data = new unsigned char[imageSize];
	// Read the actual data from the file into the buffer
	fread(data, 1, imageSize, file);
	//Everything is in memory now, the file can be closed
	fclose(file);

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	return textureID;
}
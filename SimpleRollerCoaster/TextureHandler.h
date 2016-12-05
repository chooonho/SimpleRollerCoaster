/*******************************************************************************
*	Author				: OOI CHOON HO
*	Date				: 05/12/2016
*******************************************************************************/

#pragma once

#ifndef _TEXTURE_HANDLER_H
#define _TEXTURE_HANDLER_H

#define _CRT_SECURE_NO_DEPRECATE

#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <stdio.h>
#include <fstream>

// Loads bmp file as texture
GLuint loadBMP(const char* path);

#endif
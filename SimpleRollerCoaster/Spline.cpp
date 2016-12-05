#include "Spline.h"
#include "TextureHandler.h"
#include <iostream>

// Constructor / Destructor
Spline::Spline() : UDIVISOR(600) // 90 | 250 | 300 | 1000 - numbers other than this will cause exception (cause not known yet)
{
	mSplineSegments = NULL;
	mMajorCPoints = NULL;
	mSegmentCount = 0;
	mEnableTexture = true;

	loadTexture();
	loadMajorCPoints(FILE_NAME_SPLINE_CPOINTS);
	generateSplineSegments();
}

Spline::~Spline()
{
	delete[] mSplineSegments;
	mSplineSegments = NULL;

	delete[] mMajorCPoints;
	mMajorCPoints = NULL;

	mSegmentCount = 0;
}

// Load Texture
void Spline::loadTexture()
{
	mRailTexture = loadBMP(PATH_NAME_RAIL_TEXTURE);
}

// Load major control points from file
void Spline::loadMajorCPoints(const char* fileName)
{
	std::ifstream infile;
	infile.open(fileName, std::ios::in);

	if (infile.fail())
	{
		std::cerr << "Unable to load file\n";
		exit(1);
	}

	int pointCount;
	infile >> pointCount;
	mSegmentCount = pointCount - 3;

	const Point3D MIN_POINT = { -99, 1, -99 };
	const Point3D MAX_POINT = { 99, 99, 99 };
	mMajorCPoints = new Point3D[pointCount];
	mSplineSegments = new SplineSegment[mSegmentCount];

	for (int i = 0; i < pointCount; i++)
	{
		infile >> mMajorCPoints[i].x >> mMajorCPoints[i].y >> mMajorCPoints[i].z;
	}

	infile.close();
}

// Calculate minor control points (points in a spline segment)
// Reference: slide 26 of http://run.usc.edu/cs420-s14/lec08-splines/08-splines.pdf
Point3D Spline::calcMinorCPoint(Point3D cPoints[4], float u)
{
	Point3D newPoints[4];
	Point3D minorPoint;

	for (int i = 0; i < 4; i++) {
		newPoints[i] = { 0,0,0 };

		for (int j = 0; j < 4; j++) {
			newPoints[i].x = newPoints[i].x + (BASIS[i][j] * cPoints[j].x);
			newPoints[i].y = newPoints[i].y + (BASIS[i][j] * cPoints[j].y);
			newPoints[i].z = newPoints[i].z + (BASIS[i][j] * cPoints[j].z);
		}
	}

	minorPoint.x = (u * u * u * newPoints[0].x) + (u * u * newPoints[1].x) + (u * newPoints[2].x) + newPoints[3].x;
	minorPoint.y = (u * u * u * newPoints[0].y) + (u * u * newPoints[1].y) + (u * newPoints[2].y) + newPoints[3].y;
	minorPoint.z = (u * u * u * newPoints[0].z) + (u * u * newPoints[1].z) + (u * newPoints[2].z) + newPoints[3].z;

	return minorPoint;
}

// Calculate minor control point tangent
// Reference: http://run.usc.edu/cs480-s11/assignments/assign2/assign2.html
Point3D Spline::calcMinorCPointTangent(Point3D cPoints[4], float u)
{
	Point3D newPoints[4];
	Point3D minorPointTangent;

	for (int i = 0; i < 4; i++) {
		newPoints[i] = { 0,0,0 };

		for (int j = 0; j < 4; j++) {
			newPoints[i].x = newPoints[i].x + (BASIS[i][j] * cPoints[j].x);
			newPoints[i].y = newPoints[i].y + (BASIS[i][j] * cPoints[j].y);
			newPoints[i].z = newPoints[i].z + (BASIS[i][j] * cPoints[j].z);
		}
	}

	minorPointTangent.x = ((3 * u * u) * newPoints[0].x) + ((2 * u) * newPoints[1].x) + newPoints[2].x;
	minorPointTangent.y = ((3 * u * u) * newPoints[0].y) + ((2 * u) * newPoints[1].y) + newPoints[2].y;
	minorPointTangent.z = ((3 * u * u) * newPoints[0].z) + ((2 * u) * newPoints[1].z) + newPoints[2].z;

	return minorPointTangent;
}

// Generate spline segments
void Spline::generateSplineSegments()
{
	if (mMajorCPoints == NULL || mSplineSegments == NULL)
	{
		return;
	}

	for (int i = 0; i < mSegmentCount; i++)
	{
		mSplineSegments[i].minorCPointCount = UDIVISOR;
		mSplineSegments[i].minorCPoints = new Point3D[UDIVISOR];
		mSplineSegments[i].minorCPointsTangents = new Point3D[UDIVISOR];
		mSplineSegments[i].minorCPointsNormals = new Point3D[UDIVISOR];
		mSplineSegments[i].minorCPointsBiNormals = new Point3D[UDIVISOR];

		int minorIndex = 0;

		for (double u = 0.0f; u < 1.0f; u += (1.0f / UDIVISOR))
		{
			Point3D cPoints[4] = { mMajorCPoints[i], mMajorCPoints[i + 1], mMajorCPoints[i + 2], mMajorCPoints[i + 3] };
			Point3D minorCPoint = calcMinorCPoint(cPoints, u);
			Point3D minorCPointTangent = calcMinorCPointTangent(cPoints, u);
			mSplineSegments[i].minorCPointsTangents[minorIndex] = minorCPointTangent;
			minorCPointTangent = normalize(minorCPointTangent);

			Point3D minorCPointNormal;
			Point3D minorCPointBiNormal;
			if (i == 0 && minorIndex == 0)
			{
				Point3D v0 = { 0.0, 0.0, 1.0 };

				minorCPointNormal = normalize(crossProduct(minorCPointTangent, v0));
				minorCPointBiNormal = normalize(crossProduct(minorCPointTangent, minorCPointNormal));
			}
			else
			{
				if (minorIndex == 0)
				{
					minorCPointNormal = normalize(crossProduct(mSplineSegments[i - 1].minorCPointsBiNormals[UDIVISOR - 1], minorCPointTangent));
				}
				else
				{
					minorCPointNormal = normalize(crossProduct(mSplineSegments[i].minorCPointsBiNormals[minorIndex - 1], minorCPointTangent));
				}

				minorCPointBiNormal = normalize(crossProduct(minorCPointTangent, minorCPointNormal));
			}

			mSplineSegments[i].minorCPoints[minorIndex] = minorCPoint;
			mSplineSegments[i].minorCPointsNormals[minorIndex] = minorCPointNormal;
			mSplineSegments[i].minorCPointsBiNormals[minorIndex] = minorCPointBiNormal;

			minorIndex++;
		}
	}
}

// Draw rail
void Spline::drawRail(Point3D vertices[8])
{
	//Top
	Point3D normal = normalize(crossProduct(subtractVectors(vertices[5], vertices[6]), subtractVectors(vertices[2], vertices[6])));
	normal = { -normal.x, -normal.y, -normal.z };
	glBegin(GL_QUADS);
		glNormal3f(normal.x, normal.y, normal.z);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(vertices[1].x, vertices[1].y, vertices[1].z);
		glNormal3f(normal.x, normal.y, normal.z);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(vertices[2].x, vertices[2].y, vertices[2].z);
		glNormal3f(normal.x, normal.y, normal.z);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(vertices[6].x, vertices[6].y, vertices[6].z);
		glNormal3f(normal.x, normal.y, normal.z);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(vertices[5].x, vertices[5].y, vertices[5].z);
	glEnd();

	//Bottom
	normal = normalize(crossProduct(subtractVectors(vertices[4], vertices[7]), subtractVectors(vertices[3], vertices[7])));
	normal = { -normal.x, -normal.y, -normal.z };
	glBegin(GL_QUADS);
		glNormal3f(normal.x, normal.y, normal.z);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(vertices[0].x, vertices[0].y, vertices[0].z);
		glNormal3f(normal.x, normal.y, normal.z);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(vertices[3].x, vertices[3].y, vertices[3].z);
		glNormal3f(normal.x, normal.y, normal.z);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(vertices[7].x, vertices[7].y, vertices[7].z);
		glNormal3f(normal.x, normal.y, normal.z);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(vertices[4].x, vertices[4].y, vertices[4].z);
	glEnd();

	//Left
	normal = normalize(crossProduct(subtractVectors(vertices[3], vertices[7]), subtractVectors(vertices[6], vertices[7])));
	normal = { -normal.x, -normal.y, -normal.z };
	glBegin(GL_QUADS);
		glNormal3f(normal.x, normal.y, normal.z);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(vertices[2].x, vertices[2].y, vertices[2].z);
		glNormal3f(normal.x, normal.y, normal.z);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(vertices[6].x, vertices[6].y, vertices[6].z);
		glNormal3f(normal.x, normal.y, normal.z);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(vertices[7].x, vertices[7].y, vertices[7].z);
		glNormal3f(normal.x, normal.y, normal.z);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(vertices[3].x, vertices[3].y, vertices[3].z);
	glEnd();

	//Right
	normal = normalize(crossProduct(subtractVectors(vertices[0], vertices[4]), subtractVectors(vertices[5], vertices[4])));
	normal = { -normal.x, -normal.y, -normal.z };
	glBegin(GL_QUADS);
		glNormal3f(normal.x, normal.y, normal.z);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(vertices[1].x, vertices[1].y, vertices[1].z);
		glNormal3f(normal.x, normal.y, normal.z);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(vertices[5].x, vertices[5].y, vertices[5].z);
		glNormal3f(normal.x, normal.y, normal.z);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(vertices[4].x, vertices[4].y, vertices[4].z);
		glNormal3f(normal.x, normal.y, normal.z);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(vertices[0].x, vertices[0].y, vertices[0].z);
	glEnd();
}

// Draw rail holder
void Spline::drawRailHolder(Point3D vertices[8])
{
	// Front
	Point3D normal = normalize(crossProduct(subtractVectors(Point3D {vertices[0].x, 0.0f, vertices[0].z}, Point3D {vertices[4].x, 0.0f, vertices[4].z}),
									subtractVectors(vertices[4], Point3D {vertices[4].x, 0.0f, vertices[4].z})));
	normal = { -normal.x, -normal.y, -normal.z };
	glBegin(GL_QUADS);
		glNormal3f(normal.x, normal.y, normal.z);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(vertices[0].x, vertices[0].y, vertices[1].z);
		glNormal3f(normal.x, normal.y, normal.z);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(vertices[0].x, 0.0f, vertices[0].z);
		glNormal3f(normal.x, normal.y, normal.z);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(vertices[4].x, 0.0f, vertices[4].z);
		glNormal3f(normal.x, normal.y, normal.z);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(vertices[4].x, vertices[4].y, vertices[4].z);
	glEnd();

	// Rear
	normal = normalize(crossProduct(subtractVectors(Point3D {vertices[3].x, 0.0f, vertices[3].z}, Point3D {vertices[7].x, 0.0f, vertices[7].z}),
									subtractVectors(vertices[7], Point3D {vertices[7].x, 0.0f, vertices[7].z})));
	normal = { -normal.x, -normal.y, -normal.z };
	glBegin(GL_QUADS);
		glNormal3f(normal.x, normal.y, normal.z);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(vertices[3].x, vertices[3].y, vertices[3].z);
		glNormal3f(normal.x, normal.y, normal.z);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(vertices[3].x, 0.0f, vertices[3].z);
		glNormal3f(normal.x, normal.y, normal.z);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(vertices[7].x, 0.0f, vertices[7].z);
		glNormal3f(normal.x, normal.y, normal.z);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(vertices[7].x, vertices[7].y, vertices[7].z);
	glEnd();

	// Left
	normal = normalize(crossProduct(subtractVectors(Point3D {vertices[0].x, 0.0f, vertices[0].z}, Point3D {vertices[3].x, 0.0f, vertices[3].z}),
									subtractVectors(vertices[3], Point3D {vertices[3].x, 0.0f, vertices[3].z})));
	normal = { -normal.x, -normal.y, -normal.z };
	glBegin(GL_QUADS);
		glNormal3f(normal.x, normal.y, normal.z);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(vertices[0].x, vertices[0].y, vertices[0].z);
		glNormal3f(normal.x, normal.y, normal.z);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(vertices[0].x, 0.0f, vertices[0].z);
		glNormal3f(normal.x, normal.y, normal.z);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(vertices[3].x, 0.0f, vertices[3].z);
		glNormal3f(normal.x, normal.y, normal.z);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(vertices[3].x, vertices[3].y, vertices[3].z);
	glEnd();

	// Right
	normal = normalize(crossProduct(subtractVectors(Point3D {vertices[7].x, 0.0f, vertices[7].z}, Point3D {vertices[4].x, 0.0f, vertices[4].z}),
									subtractVectors(vertices[4], Point3D {vertices[4].x, 0.0f, vertices[4].z})));
	normal = { -normal.x, -normal.y, -normal.z };
	glBegin(GL_QUADS);
		glNormal3f(normal.x, normal.y, normal.z);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(vertices[7].x, vertices[7].y, vertices[7].z);
		glNormal3f(normal.x, normal.y, normal.z);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(vertices[7].x, 0.0f, vertices[7].z);
		glNormal3f(normal.x, normal.y, normal.z);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(vertices[4].x, 0.0f, vertices[4].z);
		glNormal3f(normal.x, normal.y, normal.z);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(vertices[4].x, vertices[4].y, vertices[4].z);
	glEnd();
}

// Draw rail sleeper
void Spline::drawSleeper(Point3D v0, Point3D tangent, Point3D normal, Point3D biNormal, double gauge, double size = 0.03)
{
	// Only the front part of the sleeper is drawn (improve performance)
	// Calculate the 4 vertices required to draw a quad
	Point3D vertices[4];
	vertices[0] = v0;
	vertices[3] = sumVectors(vertices[0], multiplyByScalar(gauge * 0.9, biNormal));
	vertices[1] = sumVectors(vertices[0], multiplyByScalar(size, normal));
	vertices[2] = sumVectors(vertices[3], multiplyByScalar(size, normal));

	Point3D vertexNormal = normalize(crossProduct(subtractVectors(vertices[0], vertices[3]), subtractVectors(vertices[2], vertices[3])));
	glBegin(GL_QUADS);
		glNormal3f(vertexNormal.x, vertexNormal.y, vertexNormal.z);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(vertices[1].x, vertices[1].y, vertices[1].z);
		glNormal3f(vertexNormal.x, vertexNormal.y, vertexNormal.z);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(vertices[2].x, vertices[2].y, vertices[2].z);
		glNormal3f(vertexNormal.x, vertexNormal.y, vertexNormal.z);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(vertices[3].x, vertices[3].y, vertices[3].z);
		glNormal3f(vertexNormal.x, vertexNormal.y, vertexNormal.z);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(vertices[0].x, vertices[0].y, vertices[0].z);
	glEnd();
}

// Draw rail(left and right + sleeper)
void Spline::drawBiRail(int segmentIndex, int cPointIndex, double scale, double gauge = 0.5, int uInterval = 1, int sleeperInterval = 100)
{
	// Calculates the tangent
	// Calculates 8 points (left 4 - right 4) for drawing part of the rail
	Point3D tangent = normalize(mSplineSegments[segmentIndex].minorCPoints[cPointIndex]);
	Point3D vertices[8];
	vertices[2] = mSplineSegments[segmentIndex].minorCPoints[cPointIndex];
	vertices[1] = sumVectors(mSplineSegments[segmentIndex].minorCPoints[cPointIndex], multiplyByScalar(scale, mSplineSegments[segmentIndex].minorCPointsBiNormals[cPointIndex]));
	vertices[0] = sumVectors(vertices[1], multiplyByScalar(-1 * scale, mSplineSegments[segmentIndex].minorCPointsNormals[cPointIndex]));
	vertices[3] = sumVectors(mSplineSegments[segmentIndex].minorCPoints[cPointIndex], multiplyByScalar(-scale, mSplineSegments[segmentIndex].minorCPointsNormals[cPointIndex]));

	int newSegmentIndex = segmentIndex;
	int newCPointIndex = cPointIndex + uInterval;
	if (newCPointIndex > (mSplineSegments[newSegmentIndex].minorCPointCount - 1))
	{
		newSegmentIndex += 1;
		newCPointIndex = (newCPointIndex + uInterval) % mSplineSegments[newSegmentIndex].minorCPointCount;

		if (newSegmentIndex > (mSegmentCount - 1))
		{
			return;
		}
	}

	tangent = normalize(mSplineSegments[newSegmentIndex].minorCPointsTangents[newCPointIndex]);
	vertices[6] = mSplineSegments[newSegmentIndex].minorCPoints[newCPointIndex];
	vertices[5] = sumVectors(mSplineSegments[newSegmentIndex].minorCPoints[newCPointIndex], multiplyByScalar(scale, mSplineSegments[newSegmentIndex].minorCPointsBiNormals[newCPointIndex]));
	vertices[4] = sumVectors(vertices[5], multiplyByScalar(-1 * scale, mSplineSegments[newSegmentIndex].minorCPointsNormals[newCPointIndex]));
	vertices[7] = sumVectors(mSplineSegments[newSegmentIndex].minorCPoints[newCPointIndex], multiplyByScalar(-scale, mSplineSegments[newSegmentIndex].minorCPointsNormals[newCPointIndex]));

	drawRail(vertices);
	if (cPointIndex == 0)
	{
		drawRailHolder(vertices);
	}

	if (cPointIndex % sleeperInterval == 0)
	{
		drawSleeper(vertices[0], tangent, mSplineSegments[segmentIndex].minorCPointsNormals[cPointIndex], mSplineSegments[segmentIndex].minorCPointsBiNormals[cPointIndex], gauge);
	}

	for (int i = 0; i < 8; i++)
	{
		if (i < 4)
		{
			vertices[i] = sumVectors(vertices[i], multiplyByScalar(gauge, mSplineSegments[segmentIndex].minorCPointsBiNormals[cPointIndex]));
		}
		else
		{
			vertices[i] = sumVectors(vertices[i], multiplyByScalar(gauge, mSplineSegments[newSegmentIndex].minorCPointsBiNormals[newCPointIndex]));
		}
	}

	drawRail(vertices);
	if (cPointIndex == 0)
	{
		drawRailHolder(vertices);
	}
}

// Draw the spline - rail
void Spline::draw()
{
	double scale = 0.07;
	double gauge = 0.5;
	double interval = 10;
	double sleeperInterval = 15; // Use 4 for 250

	if (mEnableTexture)
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, mRailTexture);
	}
	else
	{
		glEnable(GL_COLOR_MATERIAL);
		glColor3f(0.2, 0.2, 0.2);
	}

	for (int i = 0; i < mSegmentCount; i++)
	{
		for (int j = 0; j < mSplineSegments[i].minorCPointCount; j++)
		{
			drawBiRail(i, j, scale, gauge, interval, sleeperInterval);
		}
	}

	if (mEnableTexture)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);
	}
	else
	{
		glDisable(GL_COLOR_MATERIAL);
	}
}

// Getter and setter
void Spline::setEnableTexture(bool applyTexture)
{
	mEnableTexture = applyTexture;
}

int Spline::getSegmentCount()
{
	return mSegmentCount;
}

int Spline::getMinorCPointCount(int segmentIndex)
{
	return mSplineSegments[segmentIndex].minorCPointCount;
}

SplineSegment Spline::getSplineSegment(int segmentIndex)
{
	return mSplineSegments[segmentIndex];
}

Point3D Spline::getMajorCPoint(int majorCPointIndex)
{
	return mMajorCPoints[majorCPointIndex];
}

Point3D Spline::getMinorCPoint(int segmentIndex, int cPointIndex)
{
	return mSplineSegments[segmentIndex].minorCPoints[cPointIndex];
}

Point3D Spline::getMinorCPointTangent(int segmentIndex, int cPointIndex)
{
	return mSplineSegments[segmentIndex].minorCPointsTangents[cPointIndex];
}

Point3D Spline::getMinorCPointNormal(int segmentIndex, int cPointIndex)
{
	return mSplineSegments[segmentIndex].minorCPointsNormals[cPointIndex];
}

Point3D Spline::getMinorCPointBiNormal(int segmentIndex, int cPointIndex)
{
	return mSplineSegments[segmentIndex].minorCPointsBiNormals[cPointIndex];
}

bool Spline::getEnableTexture()
{
	return mEnableTexture;
}

// Vector math functions
Point3D normalize(Point3D point)
{
	float length = sqrt((point.x * point.x) + (point.y * point.y) + (point.z * point.z));

	Point3D normalizedPoint;
	normalizedPoint.x = point.x / length;
	normalizedPoint.y = point.y / length;
	normalizedPoint.z = point.z / length;

	return normalizedPoint;
}

Point3D crossProduct(Point3D point1, Point3D point2)
{
	Point3D crossedPoint;
	crossedPoint.x = (point1.y * point2.z) - (point2.y * point1.z);
	crossedPoint.y = (point1.z * point2.x) - (point2.z * point1.x);
	crossedPoint.z = (point1.x * point2.y) - (point2.x * point1.y);

	return crossedPoint;
}

Point3D sumVectors(Point3D vector1, Point3D vector2) {
	Point3D summedVector;

	summedVector.x = vector1.x + vector2.x;
	summedVector.y = vector1.y + vector2.y;
	summedVector.z = vector1.z + vector2.z;

	return summedVector;
}

Point3D subtractVectors(Point3D vector1, Point3D vector2) {
	Point3D subtractedVector;

	subtractedVector.x = vector1.x - vector2.x;
	subtractedVector.y = vector1.y - vector2.y;
	subtractedVector.z = vector1.z - vector2.z;

	return subtractedVector;
}

Point3D multiplyByScalar(double multiplier, Point3D vector) {
	Point3D multipliedVector;

	multipliedVector.x = vector.x * multiplier;
	multipliedVector.y = vector.y * multiplier;
	multipliedVector.z = vector.z * multiplier;

	return multipliedVector;
}
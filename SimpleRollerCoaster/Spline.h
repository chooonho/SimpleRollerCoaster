#pragma once

#ifndef _SPLINE_H_
#define _SPLINE_H_

#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>

// Structure that defines a 3D point
struct Point3D
{
	float x;
	float y;
	float z;
};

// Structure that defines a spline segment (segment is in between two major control points)
struct SplineSegment
{
	// Minor control points are the points interpolated between two major control points
	int minorCPointCount;
	Point3D* minorCPoints;
	Point3D* minorCPointsTangents;
	Point3D* minorCPointsNormals;
	Point3D* minorCPointsBiNormals;

	~SplineSegment()
	{
		delete[] minorCPoints;
		minorCPoints = NULL;

		delete[] minorCPointsTangents;
		minorCPointsTangents = NULL;

		delete[] minorCPointsNormals;
		minorCPointsNormals = NULL;

		delete[] minorCPointsBiNormals;
		minorCPointsBiNormals = NULL;
	}
};

class Spline
{
	private:
		// Attributes
		const int UDIVISOR;
		int mSegmentCount;
		SplineSegment* mSplineSegments;
		Point3D* mMajorCPoints;
		GLuint mRailTexture;
		bool mEnableTexture;
		// Loads major control points from file
		void loadMajorCPoints(const char* fileName);
		// Generate the spline segements - interpolates major control points
		void generateSplineSegments();
		// Calculate minor control points
		Point3D calcMinorCPoint(Point3D cPoints[4], float u);
		// Calculate minor control point tangent
		Point3D calcMinorCPointTangent(Point3D cPoints[4], float u);
		// Load texture
		void loadTexture();
		// Draw the rail
		void drawRail(Point3D vertices[8]);
		// Draw the rail holder
		void drawRailHolder(Point3D vertices[8]);
		// Draw the rail sleeper (cross section)
		void drawSleeper(Point3D point, Point3D tangent, Point3D normal, Point3D biNormal, double gauge, double size);
		// Draw the rail (left and right + sleeper)
		void drawBiRail(int segmentIndex, int cPointIndex, double scale, double gauge, int uInterval, int sleeperInterval);
	public:
		// Constructor and destructor
		Spline();
		~Spline();

		// Getter and setter
		int getSegmentCount();
		int getMinorCPointCount(int segmentIndex);
		Point3D getMinorCPoint(int segmentIndex, int cPointIndex);
		Point3D getMinorCPointTangent(int segmentIndex, int cPointIndex);
		Point3D getMinorCPointNormal(int segmentIndex, int cPointIndex);
		Point3D getMinorCPointBiNormal(int segmentIndex, int cPointIndex);
		SplineSegment getSplineSegment(int segmentIndex);
		Point3D getMajorCPoint(int majorCPointIndex);
		void setEnableTexture(bool applyTexture);
		bool getEnableTexture();

		// Draw the spline - the rail
		void draw();
};

// Static variables
static const char* PATH_NAME_RAIL_TEXTURE = { "Textures/rail.bmp" };
static const char* FILE_NAME_SPLINE_CPOINTS = { "Splines/RollerCoaster.crs" };
static const float BASIS[4][4] = {
	{ -0.5	, 1.5	, -1.5	, 0.5 },
	{ 1		, -2.5	, 2		, -0.5 },
	{ -0.5	, 0		, 0.5	, 0 },
	{ 0		, 1		, 0		, 0 }
};

// Vector math functions
Point3D normalize(Point3D point);
Point3D crossProduct(Point3D point1, Point3D point2);
Point3D sumVectors(Point3D vector1, Point3D vector2);
Point3D subtractVectors(Point3D vector1, Point3D vector2);
Point3D multiplyByScalar(double multiplier, Point3D vector);

#endif // _SPLINE_H_

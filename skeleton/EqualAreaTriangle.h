#pragma once

#include <vector>
#include <iostream>

#include "Geometry.h"
#include "Renderable.h"
#include "double128/double128.h"
#include "d128Vec3.h"

class EqualAreaTriangle {

public:
	EqualAreaTriangle(Renderable & r, cgiDouble128NS::double128 numFaces);
	~EqualAreaTriangle();

	void print(cgiDouble128NS::double128 p);

	void outputCSV();

	cgiDouble128NS::double128 sphericalTriangleArea(d128Vec3 p1, d128Vec3 p2, d128Vec3 p3);

	d128Vec3 findSlerpVector(cgiDouble128NS::double128 longitude);

	//Pentagon
	cgiDouble128NS::double128 slerpBinarySearch(d128Vec3 source, d128Vec3 dest, d128Vec3 dest2);
	//Single Unknown Slerp
	cgiDouble128NS::double128 slerpBinarySearch(d128Vec3 source, d128Vec3 dest, d128Vec3 known, d128Vec3 known2, cgiDouble128NS::double128 lower, cgiDouble128NS::double128 upper);
	//Double Unknown Longitude
	cgiDouble128NS::double128 longitudeBinarySearch(cgiDouble128NS::double128 minLon, cgiDouble128NS::double128 maxLon, d128Vec3 source, d128Vec3 known, d128Vec3 known2, cgiDouble128NS::double128* outLon, cgiDouble128NS::double128 l, cgiDouble128NS::double128 u);
	//Single Unknown Longitude
	cgiDouble128NS::double128 longitudeBinarySearch(cgiDouble128NS::double128 minLon, cgiDouble128NS::double128 maxLon, d128Vec3 source, d128Vec3 known, d128Vec3 known2, d128Vec3 known3, cgiDouble128NS::double128* outLon);
	//Single Unknown Longitude Near Equator
	cgiDouble128NS::double128 longitudeBinarySearch2(cgiDouble128NS::double128 minLon, cgiDouble128NS::double128 maxLon, d128Vec3 source, d128Vec3 known, d128Vec3 known2, d128Vec3 known3, cgiDouble128NS::double128* outLon);
	//North South Orientation
	cgiDouble128NS::double128 angleExhaustiveSearch(cgiDouble128NS::double128 minAngle, cgiDouble128NS::double128 maxAngle, d128Vec3 known, d128Vec3 known2, d128Vec3 known3);

	void createSouth();
	void fillRenderable(Renderable& r);

	//First 2 are poles, next 10 are first ring, next 20 are second ring, etc.
	std::vector<d128Vec3> verts;

	cgiDouble128NS::double128 pi;

	//sin and cos of pi/2, used for calculating seeds for slerp binary search
	cgiDouble128NS::double128 sinLatitude;
	cgiDouble128NS::double128 cosLatitude;

	//Area of each face in the mesh ((4*pi)/numFaces)
	cgiDouble128NS::double128 targetArea;
	cgiDouble128NS::double128 numFaces;

	//Number of binary search iterations (corresponds to number of mantissa digits in double128)
	unsigned int numIterations;
};


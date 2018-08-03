#pragma once

#include <vector>
#include <iostream>

#include "Geometry.h"
#include "Renderable.h"
#include "double128/double128.h"
#include "d128Vec3.h"

class EqualAreaOctahedron {
public:
	EqualAreaOctahedron(Renderable & r, cgiDouble128NS::double128 numFaces);
	~EqualAreaOctahedron();

	void print(cgiDouble128NS::double128 p);

	void outputCSV();

	void outputOBJ();

	void fillRenderable(Renderable& r);

	cgiDouble128NS::double128 sphericalTriangleArea(d128Vec3 p1, d128Vec3 p2, d128Vec3 p3);

	d128Vec3 findSlerpVector(cgiDouble128NS::double128 longitude);

	//Single Unknown Slerp
	cgiDouble128NS::double128 slerpBinarySearch(d128Vec3 source, d128Vec3 dest, d128Vec3 known, d128Vec3 known2, cgiDouble128NS::double128 lower, cgiDouble128NS::double128 upper);
	//Trisect slerp
	cgiDouble128NS::double128 slerpBinarySearch(d128Vec3 source, d128Vec3 dest, d128Vec3 known, d128Vec3 known2, cgiDouble128NS::double128 lower, cgiDouble128NS::double128 upper, cgiDouble128NS::double128 targetArea);

	void createSouth();

	std::vector<d128Vec3> verts;

	std::vector<unsigned int> faces;

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


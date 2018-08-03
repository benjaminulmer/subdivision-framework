#pragma once

#include <vector>
#include <iostream>

#include "Geometry.h"
#include "Renderable.h"
#include "double128/double128.h"
#include "d128Vec3.h"
#include "d128Vec2.h"

class EqualAreaTetrahedron {
public:
	EqualAreaTetrahedron(Renderable & r, unsigned int subdivisionLevel);
	~EqualAreaTetrahedron();

	void print(cgiDouble128NS::double128 p);

	void outputArea();

	void outputAreaDifference();

	void outputCompactness();

	void outputCSV();

	void outputOBJ(bool mode);

	cgiDouble128NS::double128 planarTriangleArea(d128Vec3 p1, d128Vec3 p2, d128Vec3 p3);

	cgiDouble128NS::double128 sphericalTriangleArea(d128Vec3 p1, d128Vec3 p2, d128Vec3 p3);

	cgiDouble128NS::double128 slerpBinarySearch(d128Vec3 source, d128Vec3 dest, d128Vec3 known, d128Vec3 known2, cgiDouble128NS::double128 lower, cgiDouble128NS::double128 upper, cgiDouble128NS::double128 targetArea);

	void fillRenderable(Renderable& r, bool mode);

	bool children(unsigned int cell, unsigned int* c1, unsigned int* c2, unsigned int* c3, unsigned int* c4, unsigned int resolution);

	void findNeighbor(unsigned int cell, int * n1, int * n2, int * n3);

	unsigned int pointToCell(d128Vec2 latlon);

	std::vector<d128Vec3> verts;
	std::vector<glm::u32vec3> faces;

	std::vector<d128Vec3> flatVerts;
	std::vector<glm::u32vec3> flatFaces;

	std::vector<d128Vec3> vertsRhombicTriacontahedron;

	cgiDouble128NS::double128 pi;
	cgiDouble128NS::double128 targetArea;

	unsigned int numTries;
	unsigned int maxNumTries;
 
	unsigned int numIterations;
	unsigned int subdivisionLevel;
	unsigned int planarSubdivisionLevel;
};


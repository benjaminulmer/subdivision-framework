#pragma once

#include <array>
#include <vector>
#include <map>

#include <glm/glm.hpp>
#include "double128/double128.h"
#include "d128Vec3.h"
#include "d128Vec2.h"

#include "Cell.h"

class DGGS {

public:

	DGGS();
	DGGS(std::vector<d128Vec3> vertsRhombicTriacontahedron, std::vector<d128Vec3> vertsSubdivided, std::vector<glm::u32vec3> facesSubdivided);
	~DGGS();

	void print(cgiDouble128NS::double128 p);

	bool cellToVerts(unsigned int cell, unsigned int resolution, d128Vec3* v1, d128Vec3* v2, d128Vec3* v3);

	//Convert XYZ to cell index at current resolution
	unsigned int pointToCell(d128Vec3 point);
	//Convert XYZ to cell index at specified resolution
	unsigned int pointToCell(d128Vec3 point, unsigned int resolution);
	//Convert lat lon to cell index at current resolution
	unsigned int pointToCell(d128Vec2 latlon);
	//Convert lat lon to cell index at specified resolution
	unsigned int pointToCell(d128Vec2 latlon, unsigned int resolution);

	//Find the neighbors of a given cell by index
	bool neighbors(unsigned int cell, unsigned int* n1, unsigned int * n2, unsigned int* n3);
	bool neighbors(unsigned int cell, unsigned int* n1, unsigned int * n2, unsigned int* n3, unsigned int resolution);

	//Find the index of the given cell's parent, return which child we are
	unsigned int parent(unsigned int cell, unsigned int resolution, unsigned int* parent);

	//Find the indices of a given cell's children, return false if the cell does not have children
	bool children(unsigned int cell, unsigned int resolution, unsigned int* c1, unsigned int* c2, unsigned int* c3, unsigned int* c4);

	//Select cell at given XYZ at current resolution
	bool select(d128Vec3 point);
	//Select cell at given XYZ at specified resolution
	bool select(d128Vec3 point, unsigned int resolution);
	//Select cell at given lat lon at current resolution
	bool select(d128Vec2 point);
	//Select cell at given lat lon at specified resolution
	bool select(d128Vec2 point, unsigned int resolution);

	//Selects all cells in a given spherical polygon specified by a vecor of XYZ points at current resolution. The last point is inside the polygon.
	bool selectArea(std::vector<d128Vec3> points, d128Vec3 inside);
	//Selects all cells in a given spherical polygon specified by a vecor of XYZ points at given resolution. The last point is inside the polygon.
	bool selectArea(std::vector<d128Vec3> points, d128Vec3 inside, unsigned int resolution);
	//Selects all cells in a given spherical polygon specified by a vecor of lat lon points at current resolution. The last point is inside the polygon.
	bool selectArea(std::vector<d128Vec2> area, d128Vec2 point);
	//Selects all cells in a given spherical polygon specified by a vecor of lat lon points at given resolution. The last point is inside the polygon.
	bool selectArea(std::vector<d128Vec2> area, d128Vec2 point, unsigned int resolution);

	bool selectRegion(std::string regionName);

	//Deselect cell at given XYZ at current resolution
	bool deSelect(d128Vec3 point);
	//Deselect cell at given XYZ at specified resolution
	bool deSelect(d128Vec3 point, unsigned int resolution);
	//Deselect cell at given lat lon at current resolution
	bool deSelect(d128Vec2 point);
	//Deselect cell at given lat lon at specified resolution
	bool deSelect(d128Vec2 point, unsigned int resolution);

	//Clear selected buffer
	bool deSelectAll();

	void viewUpdate(glm::vec3 camZ, double zoom);

	//Return the population density of the selected area(cached)
	cgiDouble128NS::double128 populationDensityQuery();

	//Consider carefully whether these should be map or unordered map
	std::map<unsigned int, Cell> visible;
	std::map<unsigned int, Cell> visibleBoundary;
	std::map<unsigned int, Cell> selected;
	std::map<unsigned int, Cell> selectedBoundary;

	std::vector<std::array<unsigned int, 4>> baseNeighbors;
	std::vector<std::array<unsigned int, 4>> transforms;

	//30 * 4 verts
	std::vector<d128Vec3> vertsRhombicTriacontahedron;

	//Subdivision lv 4: 15,362 verts
	std::vector<d128Vec3> vertsSubdivided;
	//Subdivision lv 4: 30,720 faces
	std::vector<glm::u32vec3> facesSubdivided;

	unsigned int resolution;
	unsigned int switchResolution;

	cgiDouble128NS::double128 selectedPopulationDensity;

	//Renderable r;
};


#include "Sdog.h"

// Creats an SdogGrid with given bounds in each (spherical) direction
SdogGrid::SdogGrid(GridType type, double maxRadius, double minRadius,
	               double maxLat, double minLat, double maxLong, double minLong) :
	type(type),
	maxRadius(maxRadius), minRadius(minRadius),
	maxLat(maxLat), minLat(minLat),
	maxLong(maxLong), minLong(minLong) {

	// Each grid type has a different number of children
	if (type == GridType::NG) {
		numChildren = 8;
	}
	else if (type == GridType::LG) {
		numChildren = 6;
	}
	else { // (type == GridType::SG)
		numChildren = 4;
	}
	leaf = true;
}

// Recursive function for subdiving the tree to given level
void SdogGrid::subdivideTo(int level) {

	// Only subdivide if a leaf (children already exist otherwise)
	if (leaf) {
		subdivide();
		leaf = false;
	}
	// If more leveles need to be created recursively subdivide children
	if (level > 0) {
		for (int i = 0; i < numChildren; i++) {
			children[i]->subdivideTo(level - 1);
		}
	}
}

// Subdivides grid into children grids
void SdogGrid::subdivide() {

	// Midpoints
	double midRadius = (maxRadius + minRadius) / 2;
	double midLat = (maxLat + minLat) / 2;
	double midLong = (maxLong + minLong) / 2;

	// Subdivision is different for each grid type
	if (type == GridType::NG) {
		children[0] = new SdogGrid(GridType::NG, maxRadius, midRadius, midLat, minLat, midLong, minLong);
		children[1] = new SdogGrid(GridType::NG, maxRadius, midRadius, maxLat, midLat, midLong, minLong);
		children[2] = new SdogGrid(GridType::NG, maxRadius, midRadius, maxLat, midLat, maxLong, midLong);
		children[3] = new SdogGrid(GridType::NG, maxRadius, midRadius, midLat, minLat, maxLong, midLong);

		children[4] = new SdogGrid(GridType::NG, midRadius, minRadius, midLat, minLat, midLong, minLong);
		children[5] = new SdogGrid(GridType::NG, midRadius, minRadius, maxLat, midLat, midLong, minLong);
		children[6] = new SdogGrid(GridType::NG, midRadius, minRadius, maxLat, midLat, maxLong, midLong);
		children[7] = new SdogGrid(GridType::NG, midRadius, minRadius, midLat, minLat, maxLong, midLong);
	}
	else if (type == GridType::LG) {
		children[0] = new SdogGrid(GridType::NG, maxRadius, midRadius, midLat, minLat, midLong, minLong);
		children[1] = new SdogGrid(GridType::NG, maxRadius, midRadius, midLat, minLat, maxLong, midLong);
		children[2] = new SdogGrid(GridType::NG, midRadius, minRadius, midLat, minLat, midLong, minLong);
		children[3] = new SdogGrid(GridType::NG, midRadius, minRadius, midLat, minLat, maxLong, midLong);

		children[4] = new SdogGrid(GridType::LG, maxRadius, midRadius, maxLat, midLat, maxLong, minLong);
		children[5] = new SdogGrid(GridType::LG, midRadius, minRadius, maxLat, midLat, maxLong, minLong);
	}
	else { // (type == GridType::SG)
		children[0] = new SdogGrid(GridType::LG, maxRadius, midRadius, maxLat, midLat, maxLong, minLong);
		children[1] = new SdogGrid(GridType::NG, maxRadius, midRadius, midLat, minLat, midLong, minLong);
		children[2] = new SdogGrid(GridType::NG, maxRadius, midRadius, midLat, minLat, maxLong, midLong);
		children[3] = new SdogGrid(GridType::SG, midRadius, 0.0, maxLat, minLat, maxLong, minLong);
	}
}

// Recursive function for creating renderable at desired level
void SdogGrid::createRenderable(Renderable & r, int level) {
	
	// If not at desired level recursively create renderables for children
	if (level != 0) {
		for (int i = 0; i < numChildren; i++) {
			children[i]->createRenderable(r, level - 1);
		}
	}
	else {
		fillRenderable(r);
	}
}

// Fills a renderable with geometry for the grid
void SdogGrid::fillRenderable(Renderable& r) {
	glm::vec3 origin(0.f, 0.f, 0.f);

	// Find key points, same for all types of grids
	// Outer points
	glm::vec3 o1 = glm::vec3(sin(minLong)*cos(minLat), sin(minLat), cos(minLong)*cos(minLat)) * (float)maxRadius;
	glm::vec3 o2 = glm::vec3(sin(maxLong)*cos(minLat), sin(minLat), cos(maxLong)*cos(minLat)) * (float)maxRadius;
	glm::vec3 o3 = glm::vec3(sin(minLong)*cos(maxLat), sin(maxLat), cos(minLong)*cos(maxLat)) * (float)maxRadius;
	glm::vec3 o4 = glm::vec3(sin(maxLong)*cos(maxLat), sin(maxLat), cos(maxLong)*cos(maxLat)) * (float)maxRadius;

	// Inner points
	glm::vec3 i1 = glm::vec3(sin(minLong)*cos(minLat), sin(minLat), cos(minLong)*cos(minLat)) * (float)minRadius;
	glm::vec3 i2 = glm::vec3(sin(maxLong)*cos(minLat), sin(minLat), cos(maxLong)*cos(minLat)) * (float)minRadius;
	glm::vec3 i3 = glm::vec3(sin(minLong)*cos(maxLat), sin(maxLat), cos(minLong)*cos(maxLat)) * (float)minRadius;
	glm::vec3 i4 = glm::vec3(sin(maxLong)*cos(maxLat), sin(maxLat), cos(maxLong)*cos(maxLat)) * (float)minRadius;

	// Straight lines connect each inner point to coresponding outer point
	Geometry::createLine(i1, o1, r);
	Geometry::createLine(i2, o2, r);
	Geometry::createLine(i3, o3, r);
	Geometry::createLine(i4, o4, r);

	// Great circle arcs connect points on same longtitude line
	Geometry::createArc(o1, o3, origin, r);
	Geometry::createArc(o2, o4, origin, r);
	Geometry::createArc(i1, i3, origin, r);
	Geometry::createArc(i2, i4, origin, r);

	// Small circle arcs connect points on same latitude line
	Geometry::createArc(o1, o2, glm::vec3(0.f, sin(minLat), 0.f) * (float)maxRadius, r);
	Geometry::createArc(o3, o4, glm::vec3(0.f, sin(maxLat), 0.f) * (float)maxRadius, r);
	Geometry::createArc(i1, i2, glm::vec3(0.f, sin(minLat), 0.f) * (float)minRadius, r);
	Geometry::createArc(i3, i4, glm::vec3(0.f, sin(maxLat), 0.f) * (float)minRadius, r);
}

// Creates an SDOG with the given radius
// Does not automatically subdivide
Sdog::Sdog(double radius) : 
	radius(radius), numLevels(0) {
	
	// TODO Uses standard Z curve to determine order of quadrants?
	// Create starting octants of SDOG
	octants[0] = new SdogGrid(GridType::SG, radius, 0.0, -M_PI / 2, 0.0, -M_PI / 2, 0.0);
	octants[1] = new SdogGrid(GridType::SG, radius, 0.0, -M_PI / 2, 0.0,  M_PI / 2, 0.0);
	octants[2] = new SdogGrid(GridType::SG, radius, 0.0,  M_PI / 2, 0.0, -M_PI / 2, 0.0);
	octants[3] = new SdogGrid(GridType::SG, radius, 0.0,  M_PI / 2, 0.0,  M_PI / 2, 0.0);

	octants[4] = new SdogGrid(GridType::SG, radius, 0.0, -M_PI / 2, 0.0, -M_PI, -M_PI / 2);
	octants[5] = new SdogGrid(GridType::SG, radius, 0.0, -M_PI / 2, 0.0,  M_PI,  M_PI / 2);
	octants[6] = new SdogGrid(GridType::SG, radius, 0.0,  M_PI / 2, 0.0, -M_PI, -M_PI / 2);
	octants[7] = new SdogGrid(GridType::SG, radius, 0.0,  M_PI / 2, 0.0,  M_PI,  M_PI / 2);
}

// Subdivides SDOG to the desired level (if not already that deep)
void Sdog::subdivideTo(int level) {

	// Check if enough levels are already created
	if (level <= numLevels || level <= 0) {
		return;
	}

	// Start recursive call down tree
	for (SdogGrid* g : octants) {
		g->subdivideTo(level - 1);
	}
	numLevels = level;
}

// Creates a renderable for the SDOG at the given level
void Sdog::createRenderable(Renderable& r, int level) {

	// Create more levels if needed
	if (level > numLevels) {
		subdivideTo(level);
	}

	// Start recursive call down tree
	for (int i = 0; i < 8; i ++) {
		//octants[i]->createRenderable(r, level);
	}
	octants[2]->createRenderable(r, level);
}


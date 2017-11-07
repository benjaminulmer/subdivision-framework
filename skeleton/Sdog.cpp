#include "Sdog.h"

Scheme Sdog::scheme;
bool Sdog::cull;
double Sdog::maxRadius, Sdog::minRadius, Sdog::maxLat, Sdog::minLat, Sdog::maxLong, Sdog::minLong;

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

SdogGrid::~SdogGrid() {
	if (!leaf) {
		for (int i = 0; i < numChildren; i++) {
			delete children[i];
		}
	}
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
	double midLong = (maxLong + minLong) / 2;
	double midRadius;
	double midLat;

	if (Sdog::scheme == Scheme::NAIVE || Sdog::scheme == Scheme::SDOG) {
		midRadius = (maxRadius + minRadius) / 2;
		midLat = (maxLat + minLat) / 2;
	}
	else if (Sdog::scheme == Scheme::SDOG_OPT) {
		midRadius = 0.51 * maxRadius + 0.49 * minRadius;
		midLat = 0.49 * maxLat + 0.51 * minLat;
	}
	else { // Sdog::scheme == Scheme::VOLUME || Sdog::scheme == Scheme::VOLUME_SDOG
		if (type == GridType::SG) {
			midRadius = (0.629960524 * maxRadius) + ((1.0 - 0.629960524) * minRadius);
			midLat = (0.464559054 * maxLat) + ((1.0 - 0.464559054) * minLat);
		}
		else if (type == GridType::NG) {
			double num = cbrt(-(pow(maxRadius, 3) + pow(minRadius, 3)) * (sin(maxLat) - sin(minLat)));
			double denom = cbrt(2.0) * cbrt(sin(minLat) - sin(maxLat));

			midRadius = num / denom;
			midLat = asin((sin(maxLat) + sin(minLat)) / 2);
		}
		else { // (type == GridType::LG)
			double num = cbrt(-(pow(maxRadius, 3) + pow(minRadius, 3)) * (sin(maxLat) - sin(minLat)));
			double denom = cbrt(2.0) * cbrt(sin(minLat) - sin(maxLat));

			midRadius = num / denom;
			midLat = asin((2 * sin(maxLat) + sin(minLat)) / 3);
		}
	}

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
		children[3] = new SdogGrid(GridType::SG, midRadius, minRadius, maxLat, minLat, maxLong, minLong);
	}
}

// Recursive function for creating renderable at desired level
void SdogGrid::createRenderable(Renderable & r, int level, float max, float min, float avg, bool lines) {
	
	// If not at desired level recursively create renderables for children
	if (level != 0) {
		for (int i = 0; i < numChildren; i++) {
			children[i]->createRenderable(r, level - 1, max, min, avg, lines);
		}
	}
	else if (inRange()){
		fillRenderable(r, max, min, avg, lines);
	}
}

// Return true if grid is in bounds and culling is on
bool SdogGrid::inRange() {

	// Special cases for negative numbers
	double rMinLat, rMaxLat, rMinLong, rMaxLong;
	if (maxLat < 0.0) {
		rMinLat = maxLat;
		rMaxLat = minLat;
	}
	else {
		rMinLat = minLat;
		rMaxLat = maxLat;
	}
	if (maxLong < 0.0) {
		rMinLong = maxLong;
		rMaxLong = minLong;
	}
	else {
		rMinLong = minLong;
		rMaxLong = maxLong;
	}

	return ((maxRadius <= Sdog::maxRadius && minRadius >= Sdog::minRadius &&
	        rMaxLat <= Sdog::maxLat && rMinLat >= Sdog::minLat &&
	        rMaxLong <= Sdog::maxLong && rMinLong >= Sdog::minLong)) || !Sdog::cull;
}

// Recursive function for getting volumes at desired level
void SdogGrid::getVolumes(std::vector<float>& volumes, int level) {

	// If not at desired level recursively get volumes for children
	if (level != 0) {
		for (int i = 0; i < numChildren; i++) {
			children[i]->getVolumes(volumes, level - 1);
		}
	}
	else {
		float volume = (maxLong - minLong) * (pow(maxRadius, 3) - powf(minRadius, 3)) * (sin(maxLat) - sin(minLat)) / 3.0;
		volumes.push_back(abs(volume));
	}
}

// Fills a renderable with geometry for the grid
void SdogGrid::fillRenderable(Renderable& r, float max, float min, float avg, bool lines) {
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

	if (!lines) {


		// Outside and inside
		r.verts.push_back(o1); r.verts.push_back(o2); r.verts.push_back(o4);
		r.verts.push_back(o1); r.verts.push_back(o3); r.verts.push_back(o4);

		r.verts.push_back(i1); r.verts.push_back(i2); r.verts.push_back(i4);
		r.verts.push_back(i1); r.verts.push_back(i3); r.verts.push_back(i4);

		// Sides
		r.verts.push_back(o1); r.verts.push_back(i1); r.verts.push_back(i3);
		r.verts.push_back(o1); r.verts.push_back(o3); r.verts.push_back(i3);

		r.verts.push_back(o2); r.verts.push_back(i2); r.verts.push_back(i4);
		r.verts.push_back(o2); r.verts.push_back(o4); r.verts.push_back(i4);

		// Top and bottom
		r.verts.push_back(o3); r.verts.push_back(i3); r.verts.push_back(i4);
		r.verts.push_back(o3); r.verts.push_back(o4); r.verts.push_back(i4);

		r.verts.push_back(o1); r.verts.push_back(i1); r.verts.push_back(i2);
		r.verts.push_back(o1); r.verts.push_back(o2); r.verts.push_back(i2);


		r.drawMode = GL_TRIANGLES;

		float volume = abs((maxLong - minLong) * (pow(maxRadius, 3) - powf(minRadius, 3)) * (sin(maxLat) - sin(minLat)) / 3.0);
		float norm;

		glm::vec3 colour;
		if (volume / avg > 1.25) {

			norm = (volume - avg) / (max - avg);
			colour = glm::vec3(norm, 0.f, 0.f);
		}
		else if (volume / avg < 0.8) {

			norm = (volume - min) / (avg - min);
			colour = glm::vec3(0.f, 0.f, 1.f - norm);
		}
		else {

			if (abs(max - min) < 0.00001) {
				norm = 0.5f;
			}
			else {
				norm = (volume - min) / (max - min);
			}
			colour = glm::vec3(norm, norm, norm);
		}

		for (int i = 0; i < 36; i++) {
			r.colours.push_back(colour);
		}
	}
	else {
		r.drawMode = GL_TRIANGLES;

		// Straight lines connect each inner point to coresponding outer point
		Geometry::createLineR(i1, o1, r);
		Geometry::createLineR(i2, o2, r);
		Geometry::createLineR(i3, o3, r);
		Geometry::createLineR(i4, o4, r);

		// Great circle arcs connect points on same longtitude line
		Geometry::createArcR(o1, o3, origin, r);
		Geometry::createArcR(o2, o4, origin, r);
		Geometry::createArcR(i1, i3, origin, r);
		Geometry::createArcR(i2, i4, origin, r);

		// Small circle arcs connect points on same latitude line
		Geometry::createArcR(o1, o2, glm::vec3(0.f, sin(minLat), 0.f) * (float)maxRadius, r);
		Geometry::createArcR(o3, o4, glm::vec3(0.f, sin(maxLat), 0.f) * (float)maxRadius, r);
		Geometry::createArcR(i1, i2, glm::vec3(0.f, sin(minLat), 0.f) * (float)minRadius, r);
		Geometry::createArcR(i3, i4, glm::vec3(0.f, sin(maxLat), 0.f) * (float)minRadius, r);
	}

}

// Creates an SDOG with the given radius
// Does not automatically subdivide
Sdog::Sdog(Scheme scheme, double radius) :
	radius(radius), numLevels(0) {
	
	Sdog::scheme = scheme;

	// TODO Uses standard Z curve to determine order of quadrants?
	// Create starting octants of SDOG

	if (scheme == Scheme::SDOG || scheme == Scheme::VOLUME_SDOG || scheme == Scheme::SDOG_OPT) {
		octants[0] = new SdogGrid(GridType::SG, radius, 0.0, -M_PI / 2, 0.0, -M_PI / 2, 0.0);
		octants[1] = new SdogGrid(GridType::SG, radius, 0.0, -M_PI / 2, 0.0, M_PI / 2, 0.0);
		octants[2] = new SdogGrid(GridType::SG, radius, 0.0, M_PI / 2, 0.0, -M_PI / 2, 0.0);
		octants[3] = new SdogGrid(GridType::SG, radius, 0.0, M_PI / 2, 0.0, M_PI / 2, 0.0);

		octants[4] = new SdogGrid(GridType::SG, radius, 0.0, -M_PI / 2, 0.0, -M_PI, -M_PI / 2);
		octants[5] = new SdogGrid(GridType::SG, radius, 0.0, -M_PI / 2, 0.0, M_PI, M_PI / 2);
		octants[6] = new SdogGrid(GridType::SG, radius, 0.0, M_PI / 2, 0.0, -M_PI, -M_PI / 2);
		octants[7] = new SdogGrid(GridType::SG, radius, 0.0, M_PI / 2, 0.0, M_PI, M_PI / 2);
	}
	else { // scheme == Scheme::NAIVE || scheme == Scheme::VOLUME
		octants[0] = new SdogGrid(GridType::NG, radius, 0.0, -M_PI / 2, 0.0, -M_PI / 2, 0.0);
		octants[1] = new SdogGrid(GridType::NG, radius, 0.0, -M_PI / 2, 0.0, M_PI / 2, 0.0);
		octants[2] = new SdogGrid(GridType::NG, radius, 0.0, M_PI / 2, 0.0, -M_PI / 2, 0.0);
		octants[3] = new SdogGrid(GridType::NG, radius, 0.0, M_PI / 2, 0.0, M_PI / 2, 0.0);

		octants[4] = new SdogGrid(GridType::NG, radius, 0.0, -M_PI / 2, 0.0, -M_PI, -M_PI / 2);
		octants[5] = new SdogGrid(GridType::NG, radius, 0.0, -M_PI / 2, 0.0, M_PI, M_PI / 2);
		octants[6] = new SdogGrid(GridType::NG, radius, 0.0, M_PI / 2, 0.0, -M_PI, -M_PI / 2);
		octants[7] = new SdogGrid(GridType::NG, radius, 0.0, M_PI / 2, 0.0, M_PI, M_PI / 2);
	}
}

Sdog::Sdog(const Sdog & other) {
	for (int i = 0; i < 8; i++) {

	}
}

Sdog& Sdog::operator= (const Sdog& other) {
	if (this == &other) {
		return *this;
	}

	for (int i = 0; i < 8; i++) {
		delete octants[i];
		//octants[i] = other.octants[i];
	}
	radius = other.radius;
	numLevels = other.numLevels;
	return *this;
}

Sdog::~Sdog() {
	for (int i = 0; i < 8; i++) {
		delete octants[i];
	}
}

// Subdivides SDOG to the desired level (if not already that deep)
void Sdog::subdivideTo(int level, bool wholeSphere) {

	// Check if enough levels are already created
	if (level <= numLevels || level <= 0) {
		return;
	}

	if (wholeSphere) {
		for (int i = 0; i < 8; i++) {
			octants[i]->subdivideTo(level);
		}
	}
	// Only for one octant
	else {
		octants[2]->subdivideTo(level);
	}
	numLevels = level;
}

// Creates a renderable for the SDOG at the given level
void Sdog::createRenderable(Renderable& r, int level, float max, float min, float avg, bool lines, bool wholeSphere) {

	// Create more levels if needed
	if (level > numLevels) {
		subdivideTo(level, wholeSphere);
	}

	if (wholeSphere) {
		for (int i = 0; i < 8; i++) {
			octants[i]->createRenderable(r, level, max, min, avg, lines);
		}
	}
	// Only for one octant
	else {
		octants[2]->createRenderable(r, level, max, min, avg, lines);
	}
}

// Gets the volume of all cell for the SDOG at the given level
void Sdog::getVolumes(std::vector<float>& volumes, int level, bool wholeSphere) {
	
	// Create more levels if needed
	if (level > numLevels) {
		subdivideTo(level);
	}

	if (wholeSphere) {
		for (int i = 0; i < 8; i++) {
			octants[i]->getVolumes(volumes, level);
		}
	}
	// Only for one octant
	else {
		octants[2]->getVolumes(volumes, level);
	}
}

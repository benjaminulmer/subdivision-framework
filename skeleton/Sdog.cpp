#include "Sdog.h"

Scheme Sdog::scheme;
bool Sdog::cull;
double Sdog::maxRadius, Sdog::minRadius, Sdog::maxLat, Sdog::minLat, Sdog::maxLong, Sdog::minLong;

// Creats an SdogGrid with given bounds in each (spherical) direction
SdogGrid::SdogGrid(GridType type, int depth, double maxRadius, double minRadius,
	               double maxLat, double minLat, double maxLong, double minLong) :
	type(type), depth(depth),
	maxRadius(maxRadius), minRadius(minRadius),
	maxLat(maxLat), minLat(minLat),
	maxLong(maxLong), minLong(minLong) {

	// Each grid type has a different number of children
	if (type == GridType::NG) {
		numChildren = 2;
	}
	else if (type == GridType::LG) {
		numChildren = 2;
	}
	else { // (type == GridType::SG)
		numChildren = 2;
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

	if (Sdog::scheme == Scheme::SDOG_OPT && depth <= 4) {
		midRadius = 0.51 * maxRadius + 0.49 * minRadius;
		midLat = 0.49 * maxLat + 0.51 * minLat;
	}
	else {
		midRadius = (maxRadius + minRadius) / 2;
		midLat = (maxLat + minLat) / 2;
	}

	float bigRadius = 0.0, smallRadius = 0.0;
	float bigLat = 0.0, smallLat = 0.0;

	if (bsRad == BigSmall::BIG) {
		bigRadius = maxRadius;
		smallRadius = midRadius;
	}
	else if (bsRad == BigSmall::SMALL) {
		bigRadius = midRadius;
		smallRadius = minRadius;
	}

	if (bsLat == BigSmall::BIG) {
		bigLat = maxLat;
		smallLat = midLat;
	}
	else if (bsLat == BigSmall::SMALL) {
		bigLat = midLat;
		smallLat = minLat;
	}

	// Subdivision is different for each grid type
	if (type == GridType::NG) {

		if (bsRad == BigSmall::BOTH && bsLat == BigSmall::BOTH) {
			children[0] = new SdogGrid(GridType::NG, depth + 1, maxRadius, midRadius, midLat, minLat, midLong, minLong);
			children[0]->setBigSmall(BigSmall::BIG, BigSmall::SMALL);

			children[1] = new SdogGrid(GridType::NG, depth + 1, maxRadius, midRadius, maxLat, midLat, midLong, minLong);
			children[1]->setBigSmall(BigSmall::BIG, BigSmall::BIG);

			children[2] = new SdogGrid(GridType::NG, depth + 1, midRadius, minRadius, midLat, minLat, midLong, minLong);
			children[2]->setBigSmall(BigSmall::SMALL, BigSmall::SMALL);

			children[3] = new SdogGrid(GridType::NG, depth + 1, midRadius, minRadius, maxLat, midLat, midLong, minLong);
			children[3]->setBigSmall(BigSmall::SMALL, BigSmall::BIG);

			numChildren = 4;
		}
		else if (bsLat == BigSmall::BOTH) {
			children[0] = new SdogGrid(GridType::NG, depth + 1, bigRadius, smallRadius, maxLat, midLat, midLong, minLong);
			children[0]->setBigSmall(bsRad, BigSmall::BIG);

			children[1] = new SdogGrid(GridType::NG, depth + 1, bigRadius, smallRadius, midLat, minLat, midLong, minLong);
			children[1]->setBigSmall(bsRad, BigSmall::SMALL);

			numChildren = 2;
		}
		else if (bsRad == BigSmall::BOTH) {
			children[0] = new SdogGrid(GridType::NG, depth + 1, maxRadius, midRadius, bigLat, smallLat, midLong, minLong);
			children[0]->setBigSmall(BigSmall::BIG, bsLat);

			children[1] = new SdogGrid(GridType::NG, depth + 1, midRadius, minRadius, bigLat, smallLat, midLong, minLong);
			children[1]->setBigSmall(BigSmall::SMALL, bsLat);

			numChildren = 2;
		}
		else {
			children[0] = new SdogGrid(GridType::NG, depth + 1, bigRadius, smallRadius, bigLat, smallLat, midLong, minLong);
			children[0]->setBigSmall(bsRad, bsLat);

			numChildren = 1;
		}
	}
	else if (type == GridType::LG) {

		if (bsRad == BigSmall::BOTH) {
			children[0] = new SdogGrid(GridType::NG, depth + 1, maxRadius, midRadius, midLat, minLat, midLong, minLong);
			children[0]->setBigSmall(BigSmall::BIG, BigSmall::BOTH);

			children[1] = new SdogGrid(GridType::NG, depth + 1, midRadius, minRadius, midLat, minLat, midLong, minLong);
			children[1]->setBigSmall(BigSmall::SMALL, BigSmall::BOTH);

			children[2] = new SdogGrid(GridType::LG, depth + 1, maxRadius, midRadius, maxLat, midLat, maxLong, minLong);
			children[2]->setBigSmall(BigSmall::BIG, BigSmall::BOTH);

			children[3] = new SdogGrid(GridType::LG, depth + 1, midRadius, minRadius, maxLat, midLat, maxLong, minLong);
			children[3]->setBigSmall(BigSmall::SMALL, BigSmall::BOTH);

			numChildren = 4;
		}
		else if (bsRad == BigSmall::BIG) {
			children[0] = new SdogGrid(GridType::NG, depth + 1, maxRadius, midRadius, midLat, minLat, midLong, minLong);
			children[0]->setBigSmall(BigSmall::BIG, BigSmall::BOTH);

			children[1] = new SdogGrid(GridType::LG, depth + 1, maxRadius, midRadius, maxLat, midLat, maxLong, minLong);
			children[1]->setBigSmall(BigSmall::BIG, BigSmall::BOTH);

			numChildren = 2;
		}
		else {
			children[0] = new SdogGrid(GridType::NG, depth + 1, midRadius, minRadius, midLat, minLat, midLong, minLong);
			children[0]->setBigSmall(BigSmall::SMALL, BigSmall::BOTH);

			children[1] = new SdogGrid(GridType::LG, depth + 1, midRadius, minRadius, maxLat, midLat, maxLong, minLong);
			children[1]->setBigSmall(BigSmall::SMALL, BigSmall::BOTH);

			numChildren = 2;
		}
	}
	else if (type == GridType::SG) {
		children[0] = new SdogGrid(GridType::LG, depth + 1, maxRadius, midRadius, maxLat, midLat, maxLong, minLong);
		children[0]->setBigSmall(BigSmall::BOTH, BigSmall::BOTH);

		children[1] = new SdogGrid(GridType::NG, depth + 1, maxRadius, midRadius, midLat, minLat, midLong, minLong);
		children[1]->setBigSmall(BigSmall::BOTH, BigSmall::BOTH);

		children[2] = new SdogGrid(GridType::SG, depth + 1, midRadius, minRadius, maxLat, minLat, maxLong, minLong);
		children[2]->setBigSmall(BigSmall::BOTH, BigSmall::BOTH);

		numChildren = 3;
	}
	if (Sdog::scheme == Scheme::SDOG_OPT && depth <= 4) {
		for (int i = 0; i < numChildren; i++) {
			children[i]->setBigSmall(BigSmall::BOTH, BigSmall::BOTH);
		}
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
	else if (inRange()){
		fillRenderable(r);
	}
}

// Return true if grid is in bounds and culling is on
bool SdogGrid::inRange() {
	return ((maxRadius <= Sdog::maxRadius && minRadius >= Sdog::minRadius &&
	        maxLat <= Sdog::maxLat && minLat >= Sdog::minLat &&
	        maxLong <= Sdog::maxLong && minLong >= Sdog::minLong)) || !Sdog::cull;
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
Sdog::Sdog(Scheme scheme, double radius) :
	radius(radius), numLevels(0) {
	
	Sdog::scheme = scheme;

	// TODO Uses standard Z curve to determine order of quadrants?
	// Create starting octants of SDOG

	if (scheme == Scheme::SDOG || scheme == Scheme::VOLUME_SDOG || scheme == Scheme::SDOG_OPT) {
		octants[0] = new SdogGrid(GridType::SG, 1, radius, 0.0, -M_PI / 2, 0.0, -M_PI / 2, 0.0);
		octants[1] = new SdogGrid(GridType::SG, 1, radius, 0.0, -M_PI / 2, 0.0, M_PI / 2, 0.0);
		octants[2] = new SdogGrid(GridType::SG, 1, radius, 0.0, M_PI / 2, 0.0, -M_PI / 2, 0.0);
		octants[3] = new SdogGrid(GridType::SG, 1, radius, 0.0, M_PI / 2, 0.0, M_PI / 2, 0.0);

		octants[4] = new SdogGrid(GridType::SG, 1, radius, 0.0, -M_PI / 2, 0.0, -M_PI, -M_PI / 2);
		octants[5] = new SdogGrid(GridType::SG, 1, radius, 0.0, -M_PI / 2, 0.0, M_PI, M_PI / 2);
		octants[6] = new SdogGrid(GridType::SG, 1, radius, 0.0, M_PI / 2, 0.0, -M_PI, -M_PI / 2);
		octants[7] = new SdogGrid(GridType::SG, 1, radius, 0.0, M_PI / 2, 0.0, M_PI, M_PI / 2);
	}
	else { // scheme == Scheme::NAIVE || scheme == Scheme::VOLUME
		octants[0] = new SdogGrid(GridType::NG, 1, radius, 0.0, -M_PI / 2, 0.0, -M_PI / 2, 0.0);
		octants[1] = new SdogGrid(GridType::NG, 1, radius, 0.0, -M_PI / 2, 0.0, M_PI / 2, 0.0);
		octants[2] = new SdogGrid(GridType::NG, 1, radius, 0.0, M_PI / 2, 0.0, -M_PI / 2, 0.0);
		octants[3] = new SdogGrid(GridType::NG, 1, radius, 0.0, M_PI / 2, 0.0, M_PI / 2, 0.0);

		octants[4] = new SdogGrid(GridType::NG, 1, radius, 0.0, -M_PI / 2, 0.0, -M_PI, -M_PI / 2);
		octants[5] = new SdogGrid(GridType::NG, 1, radius, 0.0, -M_PI / 2, 0.0, M_PI, M_PI / 2);
		octants[6] = new SdogGrid(GridType::NG, 1, radius, 0.0, M_PI / 2, 0.0, -M_PI, -M_PI / 2);
		octants[7] = new SdogGrid(GridType::NG, 1, radius, 0.0, M_PI / 2, 0.0, M_PI, M_PI / 2);
	}

	for (int i = 0; i < 8; i++) {
		octants[i]->setBigSmall(BigSmall::BOTH, BigSmall::BOTH);
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
			octants[i]->subdivideTo(level - 1);
		}
	}
	// Only for one octant
	else {
		octants[2]->subdivideTo(level - 1);
	}
	numLevels = level;
}

// Creates a renderable for the SDOG at the given level
void Sdog::createRenderable(Renderable& r, int level, bool wholeSphere) {

	// Create more levels if needed
	if (level > numLevels) {
		subdivideTo(level);
	}

	if (wholeSphere) {
		for (int i = 0; i < 8; i++) {
			octants[i]->createRenderable(r, level);
		}
	}
	// Only for one octant
	else {
		octants[2]->createRenderable(r, level);
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

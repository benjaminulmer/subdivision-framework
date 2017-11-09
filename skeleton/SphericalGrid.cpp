#include "SphericalGrid.h"

// Creats an SdogGrid with given bounds in each (spherical) direction
SphericalGrid::SphericalGrid(GridType type, const GridInfo& info, double maxRadius, double minRadius,
                             double maxLat, double minLat, double maxLong, double minLong) :
	type(type),
	maxRadius(maxRadius), minRadius(minRadius),
	maxLat(maxLat), minLat(minLat),
	maxLong(maxLong), minLong(minLong), info(info) {

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

// Deletes all children recursively (if has any)
SphericalGrid::~SphericalGrid() {
	if (!leaf) {
		for (int i = 0; i < numChildren; i++) {
			delete children[i];
		}
	}
}

// Recursive function for subdiving the tree to given level
void SphericalGrid::subdivideTo(int level) {

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
void SphericalGrid::subdivide() {

	// Midpoints
	double midLong = (maxLong + minLong) / 2;
	double midRadius;
	double midLat;

	// Set splitting parameters from subdivision scheme
	if (info.scheme == Scheme::NAIVE || info.scheme == Scheme::SDOG) {
		midRadius = (maxRadius + minRadius) / 2;
		midLat = (maxLat + minLat) / 2;
	}
	else if (info.scheme == Scheme::SDOG_OPT) {
		midRadius = 0.51 * maxRadius + 0.49 * minRadius;
		midLat = 0.49 * maxLat + 0.51 * minLat;
	}
	else { // info.scheme == Scheme::VOLUME || info.scheme == Scheme::VOLUME_SDOG
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
		children[0] = new SphericalGrid(GridType::NG, info, maxRadius, midRadius, midLat, minLat, midLong, minLong);
		children[1] = new SphericalGrid(GridType::NG, info, maxRadius, midRadius, maxLat, midLat, midLong, minLong);
		children[2] = new SphericalGrid(GridType::NG, info, maxRadius, midRadius, maxLat, midLat, maxLong, midLong);
		children[3] = new SphericalGrid(GridType::NG, info, maxRadius, midRadius, midLat, minLat, maxLong, midLong);

		children[4] = new SphericalGrid(GridType::NG, info, midRadius, minRadius, midLat, minLat, midLong, minLong);
		children[5] = new SphericalGrid(GridType::NG, info, midRadius, minRadius, maxLat, midLat, midLong, minLong);
		children[6] = new SphericalGrid(GridType::NG, info, midRadius, minRadius, maxLat, midLat, maxLong, midLong);
		children[7] = new SphericalGrid(GridType::NG, info, midRadius, minRadius, midLat, minLat, maxLong, midLong);
	}
	else if (type == GridType::LG) {
		children[0] = new SphericalGrid(GridType::NG, info, maxRadius, midRadius, midLat, minLat, midLong, minLong);
		children[1] = new SphericalGrid(GridType::NG, info, maxRadius, midRadius, midLat, minLat, maxLong, midLong);
		children[2] = new SphericalGrid(GridType::NG, info, midRadius, minRadius, midLat, minLat, midLong, minLong);
		children[3] = new SphericalGrid(GridType::NG, info, midRadius, minRadius, midLat, minLat, maxLong, midLong);

		children[4] = new SphericalGrid(GridType::LG, info, maxRadius, midRadius, maxLat, midLat, maxLong, minLong);
		children[5] = new SphericalGrid(GridType::LG, info, midRadius, minRadius, maxLat, midLat, maxLong, minLong);
	}
	else { // (type == GridType::SG)
		children[0] = new SphericalGrid(GridType::LG, info, maxRadius, midRadius, maxLat, midLat, maxLong, minLong);
		children[1] = new SphericalGrid(GridType::NG, info, maxRadius, midRadius, midLat, minLat, midLong, minLong);
		children[2] = new SphericalGrid(GridType::NG, info, maxRadius, midRadius, midLat, minLat, maxLong, midLong);
		children[3] = new SphericalGrid(GridType::SG, info, midRadius, minRadius, maxLat, minLat, maxLong, minLong);
	}
}

// Recursive function for creating renderable at desired level
void SphericalGrid::createRenderable(Renderable & r, int level, bool lines) {
	
	// If not at desired level recursively create renderables for children
	if (level != 0) {
		for (int i = 0; i < numChildren; i++) {
			children[i]->createRenderable(r, level - 1, lines);
		}
	}
	else if (inRange()) {
		fillRenderable(r, lines);
	}
}

// Return true if grid is in bounds and culling is on
bool SphericalGrid::inRange() {

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

	return (maxRadius <= info.cullMaxRadius && minRadius >= info.cullMinRadius &&
	        rMaxLat <= info.cullMaxLat && rMinLat >= info.cullMinLat &&
	        rMaxLong <= info.cullMaxLong && rMinLong >= info.cullMinLong) || !info.cull;
}

// Recursive function for getting volumes at desired level
void SphericalGrid::getVolumes(std::vector<float>& volumes, int level) {

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
void SphericalGrid::fillRenderable(Renderable& r, bool lines) {
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
		r.drawMode = GL_TRIANGLES;

		const std::vector<SphericalDatum>& data = info.data.getData();

		float sum = 0.f;
		int count = 0;

		// Find and sum data contained in grid
		for (SphericalDatum d : data) {

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

			if (d.latitude <= rMaxLat && d.latitude > rMinLat && d.longitude <= rMaxLong && d.longitude > rMinLong &&
			                                                     d.radius <= maxRadius && d.radius > minRadius) {
				sum += d.datum;
				count++;
			}
		}
		// If no data in grid do not draw
		if (count == 0) {
			return;
		}

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

		float selfValue = sum / count;
		float norm;

		float min = info.data.getMin();
		float max = info.data.getMax();
		float avg = info.data.getAvg();

		glm::vec3 colour;

		// Set colour of cell - temporary - will probal
		if (selfValue / avg > 1.25) {

			norm = (selfValue - avg) / (max - avg);
			colour = glm::vec3(norm, 0.f, 0.f);
		}
		else if (selfValue / avg < 0.8) {

			norm = (selfValue - min) / (avg - min);
			colour = glm::vec3(0.f, 0.f, 1.f - norm);
		}
		else {

			if (abs(max - min) < 0.00001) {
				norm = 0.5f;
			}
			else {
				norm = (selfValue - min) / (max - min);
			}
			colour = glm::vec3(norm, norm, norm);
		}

		for (int i = 0; i < 36; i++) {
			r.colours.push_back(colour);
		}
	}
	else {
		r.drawMode = GL_LINES;

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
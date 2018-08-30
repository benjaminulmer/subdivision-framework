#include "SphericalGrid.h"

// Creats an SdogGrid with given bounds in each (spherical) direction
SphericalGrid::SphericalGrid(GridType type, const GridInfo& info, double maxRadius, double minRadius,
                             double maxLat, double minLat, double maxLong, double minLong) :
	type(type),
	maxRadius(maxRadius), minRadius(minRadius),
	maxLat(maxLat), minLat(minLat),
	maxLong(maxLong), minLong(minLong),
	children(0), info(info) {}

// Deletes all children recursively (if has any)
SphericalGrid::~SphericalGrid() {
	for (SphericalGrid* g : children) {
		delete g;
	}
}

// Returns if given data point resides in grid
bool SphericalGrid::contains(const SphericalDatum& d) {

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
	return (d.latitude <= rMaxLat && d.latitude > rMinLat && d.longitude <= rMaxLong && d.longitude > rMinLong &&
			d.radius <= maxRadius && d.radius > minRadius);
}

// Recursive function for agregating data located inside grid
void SphericalGrid::fillData(const SphericalDatum& d, int level) {

	// If not at desired level recursively fill data on children
	if (level != 0) {
		for (SphericalGrid* g : children) {

			// If contained in child no other child can contain point
			if (g->contains(d)) {
				g->fillData(d, level - 1);
				data.addDatum(d);
				break;
			}
		}
	}
	else {
		data.addDatum(d);
	}
}

// Recursive function for subdiving the tree to given level
void SphericalGrid::subdivideTo(int level) {

	// Only subdivide if a leaf (children already exist otherwise)
	if (children.size() == 0) {
		subdivide();
	}
	// If more leveles need to be created recursively subdivide children
	if (level > 1) {
		for (SphericalGrid* g : children) {
			g->subdivideTo(level - 1);
		}
	}
}

// Subdivides grid into children grids
void SphericalGrid::subdivide() {
	if (info.scheme == Scheme::TERNARY) {
		ternarySubdivide();
	}
	else {
		binarySubdivide();
	}

	// Remove all children that are not in the selection
	auto it = children.begin();

	if (info.mode == SubdivisionMode::SELECTION) {

		// Remove all children outside of selection
		while (it != children.end()) {
			if ((*it)->outsideBounds(info.selection)) {
				children.erase(it);
				it = children.begin();
			}
			else {
				it++;
			}
		}
	}
}

// Subdivision for binary schemes
void SphericalGrid::binarySubdivide() {
	double midLong = (maxLong + minLong) / 2;
	double midRadius;
	double midLat;

	// Set radius and latitude splitting points 
	if (info.scheme == Scheme::NAIVE || info.scheme == Scheme::SDOG || info.scheme == Scheme::OPT_SDOG) {

		//midRadius = (maxRadius + minRadius) / 2;
		//midLat = (maxLat + minLat) / 2;
		//if (info.scheme == Scheme::OPT_SDOG) {

		//	if (type == GridType::NG || type == GridType::LG) {
		//		midRadius = pow((maxRadius*maxRadius*maxRadius + minRadius * minRadius*minRadius) / 2.0, 1.0 / 3.0);
		//	}

		//	if (type == GridType::NG) {
		//		midLat = asin((sin(maxLat) + sin(minLat)) / 2.0);
		//	}
		//}
		midRadius = (maxRadius + minRadius) / 2;
		if (info.scheme == Scheme::OPT_SDOG && type == GridType::SG) {
			midLat = 0.57 * maxLat + 0.43 * minLat;
		}
		else {
			midLat = (maxLat + minLat) / 2;
		}
	}
	else {//(info.scheme == Scheme::VOLUME)
		double num = cbrt(-(pow(maxRadius, 3) + pow(minRadius, 3)) * (sin(maxLat) - sin(minLat)));
		double denom = cbrt(2.0) * cbrt(sin(minLat) - sin(maxLat));

		midRadius = num / denom;
		midLat = asin((sin(maxLat) + sin(minLat)) / 2);
	}

	// Subdivision if different if only showing a representative slice
	if (info.mode != SubdivisionMode::REP_SLICE) {
		repSliceSubdivision(midRadius, midLat, midLong);
	}
	else {
		fullSubdivision(midRadius, midLat, midLong);
	}
}

// Only does representative subdivision for grid
void SphericalGrid::repSliceSubdivision(double midRadius, double midLat, double midLong) {
	if (type == GridType::NG) {
		children = std::vector<SphericalGrid*>(8);

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
		children = std::vector<SphericalGrid*>(6);

		children[0] = new SphericalGrid(GridType::NG, info, maxRadius, midRadius, midLat, minLat, midLong, minLong);
		children[1] = new SphericalGrid(GridType::NG, info, maxRadius, midRadius, midLat, minLat, maxLong, midLong);
		children[2] = new SphericalGrid(GridType::NG, info, midRadius, minRadius, midLat, minLat, midLong, minLong);
		children[3] = new SphericalGrid(GridType::NG, info, midRadius, minRadius, midLat, minLat, maxLong, midLong);

		children[4] = new SphericalGrid(GridType::LG, info, maxRadius, midRadius, maxLat, midLat, maxLong, minLong);
		children[5] = new SphericalGrid(GridType::LG, info, midRadius, minRadius, maxLat, midLat, maxLong, minLong);
	}
	else {//(type == GridType::SG)
		children = std::vector<SphericalGrid*>(4);

		children[0] = new SphericalGrid(GridType::LG, info, maxRadius, midRadius, maxLat, midLat, maxLong, minLong);
		children[1] = new SphericalGrid(GridType::NG, info, maxRadius, midRadius, midLat, minLat, midLong, minLong);
		children[2] = new SphericalGrid(GridType::NG, info, maxRadius, midRadius, midLat, minLat, maxLong, midLong);
		children[3] = new SphericalGrid(GridType::SG, info, midRadius, minRadius, maxLat, minLat, maxLong, minLong);
	}
}

// Fully subdivides grid
void SphericalGrid::fullSubdivision(double midRadius, double midLat, double midLong) {
	if (type == GridType::NG) {
		children = std::vector<SphericalGrid*>(4);

		children[0] = new SphericalGrid(GridType::NG, info, maxRadius, midRadius, midLat, minLat, midLong, minLong);
		children[1] = new SphericalGrid(GridType::NG, info, maxRadius, midRadius, maxLat, midLat, midLong, minLong);
		children[2] = new SphericalGrid(GridType::NG, info, midRadius, minRadius, midLat, minLat, midLong, minLong);
		children[3] = new SphericalGrid(GridType::NG, info, midRadius, minRadius, maxLat, midLat, midLong, minLong);
	}
	else if (type == GridType::LG) {
		children = std::vector<SphericalGrid*>(4);

		children[0] = new SphericalGrid(GridType::NG, info, maxRadius, midRadius, midLat, minLat, midLong, minLong);
		children[1] = new SphericalGrid(GridType::NG, info, midRadius, minRadius, midLat, minLat, midLong, minLong);

		children[2] = new SphericalGrid(GridType::LG, info, maxRadius, midRadius, maxLat, midLat, maxLong, minLong);
		children[3] = new SphericalGrid(GridType::LG, info, midRadius, minRadius, maxLat, midLat, maxLong, minLong);
	}
	else {//(type == GridType::SG)
		children = std::vector<SphericalGrid*>(3);

		children[0] = new SphericalGrid(GridType::LG, info, maxRadius, midRadius, maxLat, midLat, maxLong, minLong);
		children[1] = new SphericalGrid(GridType::NG, info, maxRadius, midRadius, midLat, minLat, midLong, minLong);
		children[2] = new SphericalGrid(GridType::SG, info, midRadius, minRadius, maxLat, minLat, maxLong, minLong);
	}
}

// Subdivision for ternary schemes
void SphericalGrid::ternarySubdivide() {

	float midRadius = (maxRadius + minRadius) / 2;
	float midLat = (maxLat + minLat) / 2;
	float midLong = (maxLong + minLong) / 2;

	float radius1 = (2.0 / 3.0) * minRadius + (1.0 / 3.0) * maxRadius;
	float radius2 = (1.0 / 3.0) * minRadius + (2.0 / 3.0) * maxRadius;
	float lat1    = (2.0 / 3.0) * minLat +    (1.0 / 3.0) * maxLat;
	float lat2    = (1.0 / 3.0) * minLat +    (2.0 / 3.0) * maxLat;
	float long1   = (2.0 / 3.0) * minLong +   (1.0 / 3.0) * maxLong;
	float long2   = (1.0 / 3.0) * minLong +   (2.0 / 3.0) * maxLong;

	if (type == GridType::NG) {
		children = std::vector<SphericalGrid*>(27);

		children[0] = new SphericalGrid(GridType::NG, info, maxRadius, radius2, lat1,   minLat, long1,   minLong);
		children[1] = new SphericalGrid(GridType::NG, info, maxRadius, radius2, lat2,   lat1,   long1,   minLong);
		children[2] = new SphericalGrid(GridType::NG, info, maxRadius, radius2, maxLat, lat2,   long1,   minLong);
		children[3] = new SphericalGrid(GridType::NG, info, maxRadius, radius2, lat1,   minLat, long2,   long1);
		children[4] = new SphericalGrid(GridType::NG, info, maxRadius, radius2, lat2,   lat1,   long2,   long1);
		children[5] = new SphericalGrid(GridType::NG, info, maxRadius, radius2, maxLat, lat2,   long2,   long1);
		children[6] = new SphericalGrid(GridType::NG, info, maxRadius, radius2, lat1,   minLat, maxLong, long2);
		children[7] = new SphericalGrid(GridType::NG, info, maxRadius, radius2, lat2,   lat1,   maxLong, long2);
		children[8] = new SphericalGrid(GridType::NG, info, maxRadius, radius2, maxLat, lat2,   maxLong, long2);

		children[9]  = new SphericalGrid(GridType::NG, info, radius2, radius1, lat1,   minLat, long1,   minLong);
		children[10] = new SphericalGrid(GridType::NG, info, radius2, radius1, lat2,   lat1,   long1,   minLong);
		children[11] = new SphericalGrid(GridType::NG, info, radius2, radius1, maxLat, lat2,   long1,   minLong);
		children[12] = new SphericalGrid(GridType::NG, info, radius2, radius1, lat1,   minLat, long2,   long1);
		children[13] = new SphericalGrid(GridType::NG, info, radius2, radius1, lat2,   lat1,   long2,   long1);
		children[14] = new SphericalGrid(GridType::NG, info, radius2, radius1, maxLat, lat2,   long2,   long1);
		children[15] = new SphericalGrid(GridType::NG, info, radius2, radius1, lat1,   minLat, maxLong, long2);
		children[16] = new SphericalGrid(GridType::NG, info, radius2, radius1, lat2,   lat1,   maxLong, long2);
		children[17] = new SphericalGrid(GridType::NG, info, radius2, radius1, maxLat, lat2,   maxLong, long2);

		children[18] = new SphericalGrid(GridType::NG, info, radius1, minRadius, lat1,   minLat, long1,   minLong);
		children[19] = new SphericalGrid(GridType::NG, info, radius1, minRadius, lat2,   lat1,   long1,   minLong);
		children[20] = new SphericalGrid(GridType::NG, info, radius1, minRadius, maxLat, lat2,   long1,   minLong);
		children[21] = new SphericalGrid(GridType::NG, info, radius1, minRadius, lat1,   minLat, long2,   long1);
		children[22] = new SphericalGrid(GridType::NG, info, radius1, minRadius, lat2,   lat1,   long2,   long1);
		children[23] = new SphericalGrid(GridType::NG, info, radius1, minRadius, maxLat, lat2,   long2,   long1);
		children[24] = new SphericalGrid(GridType::NG, info, radius1, minRadius, lat1,   minLat, maxLong, long2);
		children[25] = new SphericalGrid(GridType::NG, info, radius1, minRadius, lat2,   lat1,   maxLong, long2);
		children[26] = new SphericalGrid(GridType::NG, info, radius1, minRadius, maxLat, lat2,   maxLong, long2);
	}
	else if (type == GridType::LG) {
		children = std::vector<SphericalGrid*>(18);

		children[0] = new SphericalGrid(GridType::NG, info, maxRadius, radius2, lat1,   minLat, long1,   minLong);
		children[1] = new SphericalGrid(GridType::NG, info, maxRadius, radius2, lat1,   minLat, long2,   long1);
		children[2] = new SphericalGrid(GridType::NG, info, maxRadius, radius2, lat1,   minLat, maxLong, long2);
		children[3] = new SphericalGrid(GridType::NG, info, maxRadius, radius2, lat2,   lat1,   maxLong, midLong);
		children[4] = new SphericalGrid(GridType::NG, info, maxRadius, radius2, lat2,   lat1,   midLong, minLong);
		children[5] = new SphericalGrid(GridType::LG, info, maxRadius, radius2, maxLat, lat2,   maxLong, minLong);

		children[6]  = new SphericalGrid(GridType::NG, info, radius2, radius1, lat1,   minLat, long1,   minLong);
		children[7]  = new SphericalGrid(GridType::NG, info, radius2, radius1, lat1,   minLat, long2,   long1);
		children[8]  = new SphericalGrid(GridType::NG, info, radius2, radius1, lat1,   minLat, maxLong, long2);
		children[9]  = new SphericalGrid(GridType::NG, info, radius2, radius1, lat2,   lat1,   maxLong, midLong);
		children[10] = new SphericalGrid(GridType::NG, info, radius2, radius1, lat2,   lat1,   midLong, minLong);
		children[11] = new SphericalGrid(GridType::LG, info, radius2, radius1, maxLat, lat2,   maxLong, minLong);

		children[12] = new SphericalGrid(GridType::NG, info, radius1, minRadius, lat1,   minLat, long1,   minLong);
		children[13] = new SphericalGrid(GridType::NG, info, radius1, minRadius, lat1,   minLat, long2,   long1);
		children[14] = new SphericalGrid(GridType::NG, info, radius1, minRadius, lat1,   minLat, maxLong, long2);
		children[15] = new SphericalGrid(GridType::NG, info, radius1, minRadius, lat2,   lat1,   maxLong, midLong);
		children[16] = new SphericalGrid(GridType::NG, info, radius1, minRadius, lat2,   lat1,   midLong, minLong);
		children[17] = new SphericalGrid(GridType::LG, info, radius1, minRadius, maxLat, lat2,   maxLong, minLong);
	}
	else {//(type == GridType::SG)
		children = std::vector<SphericalGrid*>(10);

		children[0] = new SphericalGrid(GridType::NG, info, maxRadius, radius2, lat1,   minLat, long1,   minLong);
		children[1] = new SphericalGrid(GridType::NG, info, maxRadius, radius2, lat1,   minLat, long2,   long1);
		children[2] = new SphericalGrid(GridType::NG, info, maxRadius, radius2, lat1,   minLat, maxLong, long2);
		children[3] = new SphericalGrid(GridType::NG, info, maxRadius, radius2, lat2,   lat1,   maxLong, midLong);
		children[4] = new SphericalGrid(GridType::NG, info, maxRadius, radius2, lat2,   lat1,   midLong, minLong);
		children[5] = new SphericalGrid(GridType::LG, info, maxRadius, radius2, maxLat, lat2,   maxLong, minLong);

		children[6] = new SphericalGrid(GridType::LG, info, radius2, radius1,   maxLat, midLat, maxLong, minLong);
		children[7] = new SphericalGrid(GridType::NG, info, radius2, radius1,   midLat, minLat, midLong, minLong);
		children[8] = new SphericalGrid(GridType::NG, info, radius2, radius1,   midLat, minLat, maxLong, midLong);
		children[9] = new SphericalGrid(GridType::SG, info, radius1, minRadius, maxLat, minLat, maxLong, minLong);
	}
}

// Recursive function for creating renderable at desired level
void SphericalGrid::createRenderable(Renderable & r, int level, DisplayMode mode) {
	
	// If not at desired level recursively create renderables for children
	if (level != 0) {
		for (SphericalGrid* g : children) {
			g->createRenderable(r, level - 1, mode);
		}
	}
	else if (!info.culling || !outsideBounds(info.cull)) {
		fillRenderable(r, mode);
	}
}

// Return true if grid is in bounds and culling is on
bool SphericalGrid::outsideBounds(GridBounds b) {

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
	return minRadius >= b.maxRadius || rMinLat >= b.maxLat || rMinLong >= b.maxLong ||
	       maxRadius <= b.minRadius || rMaxLat <= b.minLat || rMaxLong <= b.minLong;
}

// Recursive function for getting volumes at desired level
void SphericalGrid::getVolumes(std::vector<float>& volumes, int level) {

	// If not at desired level recursively get volumes for children
	if (level != 0) {
		for (SphericalGrid* g : children) {
			g->getVolumes(volumes, level - 1);
		}
	}
	else {
		float volume = (maxLong - minLong) * (pow(maxRadius, 3) - pow(minRadius, 3)) * (sin(maxLat) - sin(minLat)) / 3.0;
		volumes.push_back(abs(volume));
	}
}

// Recursive function for getting number of grids at desired level
int SphericalGrid::getNumGrids() {
	int toReturn = 0;
	if (children.size() != 0) {
		for (SphericalGrid* g : children) {
			toReturn += g->getNumGrids();
		}
		return toReturn;
	}
	else {
		return 1;
	}
}

// Fills a renderable with geometry for the grid
void SphericalGrid::fillRenderable(Renderable& r, DisplayMode mode) {

	// Different rendering for data, volumes, and lines
	if ((mode == DisplayMode::DATA && data.size() != 0) || mode == DisplayMode::VOLUMES) {
		r.drawMode = GL_TRIANGLES;
		faceRenderable(r);

		// Colour is different for data and volume modes
		glm::vec3 colour = (mode == DisplayMode::DATA) ? getDataColour() : getVolumeColour();

		for (int i = 0; i < 36; i++) {
			r.colours.push_back(colour);
		}
	}
	else if (mode == DisplayMode::LINES) {
		r.drawMode = GL_LINES;
		lineRenderable(r);
	}
}

// Fill renderable as faces
void SphericalGrid::faceRenderable(Renderable& r) {

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
}

// Fill renderable as wireframe outline
void SphericalGrid::lineRenderable(Renderable& r) {
	glm::vec3 origin(0.f, 0.f, 0.f);

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

// Get colour of grid for displaying data
glm::vec3 SphericalGrid::getDataColour() {
	glm::vec3 colour;

	data.calculateStats();
	float selfValue = data.getAvg();
	float min = info.data.getMin();
	float max = info.data.getMax();
	float avg = info.data.getAvg();

	// Set colour
	if (selfValue > avg) {
		float norm = (selfValue - avg) / (max - avg);
		colour = glm::vec3(norm, 0.f, 0.f);
	}
	else if (selfValue < avg) {
		float norm = (selfValue - min) / (avg - min);
		colour = glm::vec3(0.f, 0.f, 1.f - norm);
	}
	else {
		float norm = (selfValue - min) / (max - min);
		colour = glm::vec3(norm, norm, norm);
	}
	return colour;
}

// Get colour of grid for displaying volumes
glm::vec3 SphericalGrid::getVolumeColour() {
	glm::vec3 colour;

	float selfValue = abs((maxLong - minLong) * (pow(maxRadius, 3) - pow(minRadius, 3)) * (sin(maxLat) - sin(minLat)) / 3.0);
	float min = info.volMin;
	float max = info.volMax;
	float avg = info.volAvg;

	// If all volumes the same return grey
	if (max == min) {
		return glm::vec3(0.5f, 0.5f, 0.5f);
	}

	float sat = 0.f;
	float light = 0.5f;
	int hue = 0;

	double z = (selfValue - info.volAvg) / info.volSD;
	if (z > 0.0) {
		hue = 0;
		sat = 0.3 * z;
		float c = (1.f - abs(2.f * light - 1.f)) * sat;
		int h = hue / 60;
		float x = c * (1.f - abs(h % 2 - 1));
		float m = light - 0.5f * c;
		colour = glm::vec3(c + m, x + m, m);
	}
	else {
		hue = 250;
		sat = 0.3 * -z;
		float c = (1.f - abs(2.f * light - 1.f)) * sat;
		int h = hue / 60;
		float x = c * (1.f - abs(h % 2 - 1));
		float m = light - 0.5f * c;
		colour = glm::vec3(x + m, m, c + m);
	}

	// Set colour
	//if ((selfValue / max) > 0.999f && (selfValue / max) < 1.001f) {
	//	colour = glm::vec3(1.f, 0.f, 0.f);
	//}
	//else if ((selfValue / min) > 0.999f && (selfValue / min) < 1.001f) {
	//	colour = glm::vec3(0.f, 0.f, 1.f);
	//}
	//else {
	//	float norm = (selfValue - min) / (max - min);
	//	colour = glm::vec3(norm, norm, norm);
	//}
	return colour;
}
#include "SphericalCell.h"

#include "Geometry.h"

// Creats an spherical cell with given bounds in each (spherical) direction
SphericalCell::SphericalCell(CellType type, const GridInfo& info, double maxRadius, double minRadius,
                             double maxLat, double minLat, double maxLong, double minLong) :
	type(type),
	maxRadius(maxRadius), minRadius(minRadius),
	maxLat(maxLat), minLat(minLat),
	maxLong(maxLong), minLong(minLong),
	children(0), info(info) {}

// Deletes all children recursively (if has any)
SphericalCell::~SphericalCell() {
	for (SphericalCell* g : children) {
		delete g;
	}
}

// Returns if given data point resides in cell
bool SphericalCell::contains(const SphericalDatum& d) {

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

// Adds data point to cell
void SphericalCell::addDataPoint(const SphericalDatum & d, const DataSetInfo & info) {

	// Find index in list of data sets for the point
	int index = -1;
	int i = 0;
	for (DataPoints ds : dataSets) {

		if (ds.info.id == info.id) {
			index = i;
			break;
		}
		i++;
	}

	// If data point belongs to new data set add a new data set
	if (index == -1) {
		dataSets.push_back(DataPoints(info));
		index = dataSets.size() - 1;
	}
	dataSets[index].data.push_back(d);

}

// Recursive function for agregating data located inside cell
void SphericalCell::fillData(const SphericalDatum& d, int level, const DataSetInfo& info) {

	// If not at desired level recursively fill data on children
	if (level != 0) {
		for (SphericalCell* g : children) {

			// If contained in child no other child can contain point
			if (g->contains(d)) {

				g->fillData(d, level - 1, info);
				addDataPoint(d, info);
				break;
			}
		}
	}
	else {
		addDataPoint(d, info);
	}
}

// Recursive function for subdiving the tree to given level
void SphericalCell::subdivideTo(int level) {

	// Only subdivide if a leaf (children already exist otherwise)
	if (children.size() == 0) {
		subdivide();
	}
	// If more leveles need to be created recursively subdivide children
	if (level > 1) {
		for (SphericalCell* g : children) {
			g->subdivideTo(level - 1);
		}
	}
}

// Subdivides cell into children cells
void SphericalCell::subdivide() {
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
void SphericalCell::binarySubdivide() {
	double midLong = (maxLong + minLong) / 2;
	double midRadius;
	double midLat;

	// Set radius and latitude splitting points 
	if (info.scheme == Scheme::NAIVE || info.scheme == Scheme::SDOG || info.scheme == Scheme::OPT_SDOG) {

		midRadius = (maxRadius + minRadius) / 2;
		if (info.scheme == Scheme::OPT_SDOG && type == CellType::SG) {
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

// Only does representative subdivision for cell
void SphericalCell::repSliceSubdivision(double midRadius, double midLat, double midLong) {
	if (type == CellType::NG) {
		children = std::vector<SphericalCell*>(8);

		children[0] = new SphericalCell(CellType::NG, info, maxRadius, midRadius, midLat, minLat, midLong, minLong);
		children[1] = new SphericalCell(CellType::NG, info, maxRadius, midRadius, maxLat, midLat, midLong, minLong);
		children[2] = new SphericalCell(CellType::NG, info, maxRadius, midRadius, maxLat, midLat, maxLong, midLong);
		children[3] = new SphericalCell(CellType::NG, info, maxRadius, midRadius, midLat, minLat, maxLong, midLong);

		children[4] = new SphericalCell(CellType::NG, info, midRadius, minRadius, midLat, minLat, midLong, minLong);
		children[5] = new SphericalCell(CellType::NG, info, midRadius, minRadius, maxLat, midLat, midLong, minLong);
		children[6] = new SphericalCell(CellType::NG, info, midRadius, minRadius, maxLat, midLat, maxLong, midLong);
		children[7] = new SphericalCell(CellType::NG, info, midRadius, minRadius, midLat, minLat, maxLong, midLong);
	}
	else if (type == CellType::LG) {
		children = std::vector<SphericalCell*>(6);

		children[0] = new SphericalCell(CellType::NG, info, maxRadius, midRadius, midLat, minLat, midLong, minLong);
		children[1] = new SphericalCell(CellType::NG, info, maxRadius, midRadius, midLat, minLat, maxLong, midLong);
		children[2] = new SphericalCell(CellType::NG, info, midRadius, minRadius, midLat, minLat, midLong, minLong);
		children[3] = new SphericalCell(CellType::NG, info, midRadius, minRadius, midLat, minLat, maxLong, midLong);

		children[4] = new SphericalCell(CellType::LG, info, maxRadius, midRadius, maxLat, midLat, maxLong, minLong);
		children[5] = new SphericalCell(CellType::LG, info, midRadius, minRadius, maxLat, midLat, maxLong, minLong);
	}
	else {//(type == CellType::SG)
		children = std::vector<SphericalCell*>(4);

		children[0] = new SphericalCell(CellType::LG, info, maxRadius, midRadius, maxLat, midLat, maxLong, minLong);
		children[1] = new SphericalCell(CellType::NG, info, maxRadius, midRadius, midLat, minLat, midLong, minLong);
		children[2] = new SphericalCell(CellType::NG, info, maxRadius, midRadius, midLat, minLat, maxLong, midLong);
		children[3] = new SphericalCell(CellType::SG, info, midRadius, minRadius, maxLat, minLat, maxLong, minLong);
	}
}

// Fully subdivides cell
void SphericalCell::fullSubdivision(double midRadius, double midLat, double midLong) {
	if (type == CellType::NG) {
		children = std::vector<SphericalCell*>(4);

		children[0] = new SphericalCell(CellType::NG, info, maxRadius, midRadius, midLat, minLat, midLong, minLong);
		children[1] = new SphericalCell(CellType::NG, info, maxRadius, midRadius, maxLat, midLat, midLong, minLong);
		children[2] = new SphericalCell(CellType::NG, info, midRadius, minRadius, midLat, minLat, midLong, minLong);
		children[3] = new SphericalCell(CellType::NG, info, midRadius, minRadius, maxLat, midLat, midLong, minLong);
	}
	else if (type == CellType::LG) {
		children = std::vector<SphericalCell*>(4);

		children[0] = new SphericalCell(CellType::NG, info, maxRadius, midRadius, midLat, minLat, midLong, minLong);
		children[1] = new SphericalCell(CellType::NG, info, midRadius, minRadius, midLat, minLat, midLong, minLong);

		children[2] = new SphericalCell(CellType::LG, info, maxRadius, midRadius, maxLat, midLat, maxLong, minLong);
		children[3] = new SphericalCell(CellType::LG, info, midRadius, minRadius, maxLat, midLat, maxLong, minLong);
	}
	else {//(type == CellType::SG)
		children = std::vector<SphericalCell*>(3);

		children[0] = new SphericalCell(CellType::LG, info, maxRadius, midRadius, maxLat, midLat, maxLong, minLong);
		children[1] = new SphericalCell(CellType::NG, info, maxRadius, midRadius, midLat, minLat, midLong, minLong);
		children[2] = new SphericalCell(CellType::SG, info, midRadius, minRadius, maxLat, minLat, maxLong, minLong);
	}
}

// Subdivision for ternary schemes
void SphericalCell::ternarySubdivide() {

	float midRadius = (maxRadius + minRadius) / 2;
	float midLat = (maxLat + minLat) / 2;
	float midLong = (maxLong + minLong) / 2;

	float radius1 = (2.0 / 3.0) * minRadius + (1.0 / 3.0) * maxRadius;
	float radius2 = (1.0 / 3.0) * minRadius + (2.0 / 3.0) * maxRadius;
	float lat1    = (2.0 / 3.0) * minLat +    (1.0 / 3.0) * maxLat;
	float lat2    = (1.0 / 3.0) * minLat +    (2.0 / 3.0) * maxLat;
	float long1   = (2.0 / 3.0) * minLong +   (1.0 / 3.0) * maxLong;
	float long2   = (1.0 / 3.0) * minLong +   (2.0 / 3.0) * maxLong;

	if (type == CellType::NG) {
		children = std::vector<SphericalCell*>(27);

		children[0] = new SphericalCell(CellType::NG, info, maxRadius, radius2, lat1,   minLat, long1,   minLong);
		children[1] = new SphericalCell(CellType::NG, info, maxRadius, radius2, lat2,   lat1,   long1,   minLong);
		children[2] = new SphericalCell(CellType::NG, info, maxRadius, radius2, maxLat, lat2,   long1,   minLong);
		children[3] = new SphericalCell(CellType::NG, info, maxRadius, radius2, lat1,   minLat, long2,   long1);
		children[4] = new SphericalCell(CellType::NG, info, maxRadius, radius2, lat2,   lat1,   long2,   long1);
		children[5] = new SphericalCell(CellType::NG, info, maxRadius, radius2, maxLat, lat2,   long2,   long1);
		children[6] = new SphericalCell(CellType::NG, info, maxRadius, radius2, lat1,   minLat, maxLong, long2);
		children[7] = new SphericalCell(CellType::NG, info, maxRadius, radius2, lat2,   lat1,   maxLong, long2);
		children[8] = new SphericalCell(CellType::NG, info, maxRadius, radius2, maxLat, lat2,   maxLong, long2);

		children[9]  = new SphericalCell(CellType::NG, info, radius2, radius1, lat1,   minLat, long1,   minLong);
		children[10] = new SphericalCell(CellType::NG, info, radius2, radius1, lat2,   lat1,   long1,   minLong);
		children[11] = new SphericalCell(CellType::NG, info, radius2, radius1, maxLat, lat2,   long1,   minLong);
		children[12] = new SphericalCell(CellType::NG, info, radius2, radius1, lat1,   minLat, long2,   long1);
		children[13] = new SphericalCell(CellType::NG, info, radius2, radius1, lat2,   lat1,   long2,   long1);
		children[14] = new SphericalCell(CellType::NG, info, radius2, radius1, maxLat, lat2,   long2,   long1);
		children[15] = new SphericalCell(CellType::NG, info, radius2, radius1, lat1,   minLat, maxLong, long2);
		children[16] = new SphericalCell(CellType::NG, info, radius2, radius1, lat2,   lat1,   maxLong, long2);
		children[17] = new SphericalCell(CellType::NG, info, radius2, radius1, maxLat, lat2,   maxLong, long2);

		children[18] = new SphericalCell(CellType::NG, info, radius1, minRadius, lat1,   minLat, long1,   minLong);
		children[19] = new SphericalCell(CellType::NG, info, radius1, minRadius, lat2,   lat1,   long1,   minLong);
		children[20] = new SphericalCell(CellType::NG, info, radius1, minRadius, maxLat, lat2,   long1,   minLong);
		children[21] = new SphericalCell(CellType::NG, info, radius1, minRadius, lat1,   minLat, long2,   long1);
		children[22] = new SphericalCell(CellType::NG, info, radius1, minRadius, lat2,   lat1,   long2,   long1);
		children[23] = new SphericalCell(CellType::NG, info, radius1, minRadius, maxLat, lat2,   long2,   long1);
		children[24] = new SphericalCell(CellType::NG, info, radius1, minRadius, lat1,   minLat, maxLong, long2);
		children[25] = new SphericalCell(CellType::NG, info, radius1, minRadius, lat2,   lat1,   maxLong, long2);
		children[26] = new SphericalCell(CellType::NG, info, radius1, minRadius, maxLat, lat2,   maxLong, long2);
	}
	else if (type == CellType::LG) {
		children = std::vector<SphericalCell*>(18);

		children[0] = new SphericalCell(CellType::NG, info, maxRadius, radius2, lat1,   minLat, long1,   minLong);
		children[1] = new SphericalCell(CellType::NG, info, maxRadius, radius2, lat1,   minLat, long2,   long1);
		children[2] = new SphericalCell(CellType::NG, info, maxRadius, radius2, lat1,   minLat, maxLong, long2);
		children[3] = new SphericalCell(CellType::NG, info, maxRadius, radius2, lat2,   lat1,   maxLong, midLong);
		children[4] = new SphericalCell(CellType::NG, info, maxRadius, radius2, lat2,   lat1,   midLong, minLong);
		children[5] = new SphericalCell(CellType::LG, info, maxRadius, radius2, maxLat, lat2,   maxLong, minLong);

		children[6]  = new SphericalCell(CellType::NG, info, radius2, radius1, lat1,   minLat, long1,   minLong);
		children[7]  = new SphericalCell(CellType::NG, info, radius2, radius1, lat1,   minLat, long2,   long1);
		children[8]  = new SphericalCell(CellType::NG, info, radius2, radius1, lat1,   minLat, maxLong, long2);
		children[9]  = new SphericalCell(CellType::NG, info, radius2, radius1, lat2,   lat1,   maxLong, midLong);
		children[10] = new SphericalCell(CellType::NG, info, radius2, radius1, lat2,   lat1,   midLong, minLong);
		children[11] = new SphericalCell(CellType::LG, info, radius2, radius1, maxLat, lat2,   maxLong, minLong);

		children[12] = new SphericalCell(CellType::NG, info, radius1, minRadius, lat1,   minLat, long1,   minLong);
		children[13] = new SphericalCell(CellType::NG, info, radius1, minRadius, lat1,   minLat, long2,   long1);
		children[14] = new SphericalCell(CellType::NG, info, radius1, minRadius, lat1,   minLat, maxLong, long2);
		children[15] = new SphericalCell(CellType::NG, info, radius1, minRadius, lat2,   lat1,   maxLong, midLong);
		children[16] = new SphericalCell(CellType::NG, info, radius1, minRadius, lat2,   lat1,   midLong, minLong);
		children[17] = new SphericalCell(CellType::LG, info, radius1, minRadius, maxLat, lat2,   maxLong, minLong);
	}
	else {//(type == CellType::SG)
		children = std::vector<SphericalCell*>(10);

		children[0] = new SphericalCell(CellType::NG, info, maxRadius, radius2, lat1,   minLat, long1,   minLong);
		children[1] = new SphericalCell(CellType::NG, info, maxRadius, radius2, lat1,   minLat, long2,   long1);
		children[2] = new SphericalCell(CellType::NG, info, maxRadius, radius2, lat1,   minLat, maxLong, long2);
		children[3] = new SphericalCell(CellType::NG, info, maxRadius, radius2, lat2,   lat1,   maxLong, midLong);
		children[4] = new SphericalCell(CellType::NG, info, maxRadius, radius2, lat2,   lat1,   midLong, minLong);
		children[5] = new SphericalCell(CellType::LG, info, maxRadius, radius2, maxLat, lat2,   maxLong, minLong);

		children[6] = new SphericalCell(CellType::LG, info, radius2, radius1,   maxLat, midLat, maxLong, minLong);
		children[7] = new SphericalCell(CellType::NG, info, radius2, radius1,   midLat, minLat, midLong, minLong);
		children[8] = new SphericalCell(CellType::NG, info, radius2, radius1,   midLat, minLat, maxLong, midLong);
		children[9] = new SphericalCell(CellType::SG, info, radius1, minRadius, maxLat, minLat, maxLong, minLong);
	}
}

// Recursive function for creating renderable at desired level
void SphericalCell::createRenderable(Renderable & r, int level, DisplayMode mode) {
	
	// If not at desired level recursively create renderables for children
	if (level != 0) {
		for (SphericalCell* g : children) {
			g->createRenderable(r, level - 1, mode);
		}
	}
	else if (!info.culling || !outsideBounds(info.cull)) {
		fillRenderable(r, mode);
	}
}

// Return true if cell is in bounds and culling is on
bool SphericalCell::outsideBounds(CellBounds b) {

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
void SphericalCell::getVolumes(std::vector<float>& volumes, int level) {

	// If not at desired level recursively get volumes for children
	if (level != 0) {
		for (SphericalCell* g : children) {
			g->getVolumes(volumes, level - 1);
		}
	}
	else {
		float volume = (maxLong - minLong) * (pow(maxRadius, 3) - pow(minRadius, 3)) * (sin(maxLat) - sin(minLat)) / 3.0;
		volumes.push_back(abs(volume));
	}
}

// Recursive function for getting number of cells at desired level
int SphericalCell::getNumCells() {
	int toReturn = 0;
	if (children.size() != 0) {
		for (SphericalCell* g : children) {
			toReturn += g->getNumCells();
		}
		return toReturn;
	}
	else {
		return 1;
	}
}

// Fills a renderable with geometry for the cell
void SphericalCell::fillRenderable(Renderable& r, DisplayMode mode) {

	// Different rendering for data, volumes, and lines
	if ((mode == DisplayMode::DATA && dataSets.size() != 0) || mode == DisplayMode::VOLUMES) {

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
void SphericalCell::faceRenderable(Renderable& r) {

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
void SphericalCell::lineRenderable(Renderable& r) {
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

// Get colour of cell for displaying data
glm::vec3 SphericalCell::getDataColour() {
	glm::vec3 colour;

	for (DataPoints dp : dataSets) {
		float selfValue = dp.mean();
		float min = dp.info.min;
		float max = dp.info.max;
		float avg = dp.info.mean;

		float norm;
		if (max - min == 0.0) {
			norm = dp.info.binColors.size() - 1;
		}
		else {
			norm = (selfValue - min) / (max - min);
			norm *= dp.info.binColors.size();
			if (norm >= dp.info.binColors.size()) norm = dp.info.binColors.size() - 1;
		}
		colour += dp.info.binColors[(int) norm];
	}
	return colour / (float) dataSets.size();
}

// Get colour of cell for displaying volumes
glm::vec3 SphericalCell::getVolumeColour() {
	glm::vec3 colour;

	float selfValue = abs((maxLong - minLong) * (pow(maxRadius, 3) - pow(minRadius, 3)) * (sin(maxLat) - sin(minLat)) / 3.0);
	float min = info.volMin;
	float max = info.volMax;
	float avg = info.volAvg;

	// If all volumes the same return grey
	if (max == min) {
		return glm::vec3(0.5f, 0.5f, 0.5f);
	}

	// Set colour
	if ((selfValue / max) > 0.999f && (selfValue / max) < 1.001f) {
		colour = glm::vec3(1.f, 0.f, 0.f);
	}
	else if ((selfValue / min) > 0.999f && (selfValue / min) < 1.001f) {
		colour = glm::vec3(0.f, 0.f, 1.f);
	}
	else {
		float norm = (selfValue - min) / (max - min);
		colour = glm::vec3(norm, norm, norm);
	}
	return colour;
}
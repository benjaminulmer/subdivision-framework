#include "SphericalGrid.h"

#define _USE_MATH_DEFINES
#include <math.h>

// Creates an spherical grid with the given radius
// Does not automatically subdivide
SphericalGrid::SphericalGrid(GridInfo& info) : 
	info(info), numLevels(0) {

	// Create starting octants of SDOG
		octants[0] = new SphericalCell(CellType::SG, info, info.radius, 0.0, -M_PI / 2, 0.0, -M_PI / 2, 0.0);
		octants[1] = new SphericalCell(CellType::SG, info, info.radius, 0.0, -M_PI / 2, 0.0, M_PI / 2, 0.0);
		octants[2] = new SphericalCell(CellType::SG, info, info.radius, 0.0, M_PI / 2, 0.0, -M_PI / 2, 0.0);
		octants[3] = new SphericalCell(CellType::SG, info, info.radius, 0.0, M_PI / 2, 0.0, M_PI / 2, 0.0);

		octants[4] = new SphericalCell(CellType::SG, info, info.radius, 0.0, -M_PI / 2, 0.0, -M_PI, -M_PI / 2);
		octants[5] = new SphericalCell(CellType::SG, info, info.radius, 0.0, -M_PI / 2, 0.0, M_PI, M_PI / 2);
		octants[6] = new SphericalCell(CellType::SG, info, info.radius, 0.0, M_PI / 2, 0.0, -M_PI, -M_PI / 2);
		octants[7] = new SphericalCell(CellType::SG, info, info.radius, 0.0, M_PI / 2, 0.0, M_PI, M_PI / 2);
}

// Delete all children (recursively)
SphericalGrid::~SphericalGrid() {
	for (int i = 0; i < 8; i++) {
		delete octants[i];
	}
}

// Subdivides spherical grid to the desired level (if not already that deep)
void SphericalGrid::subdivideTo(int level) {

	// Check if enough levels are already created
	if (level <= numLevels || level <= 0) {
		return;
	}

	for (int i = 0; i < 8; i++) {
		octants[i]->subdivideTo(level);
	}
}

// Creates a renderable for the spherical grid at the given level
void SphericalGrid::createRenderable(Renderable& r, DisplayMode mode) {

	for (int i = 0; i < 8; i++) {
		octants[i]->createRenderable(r, mode);
	}
}

// Propogates data down tree to maxLevel
void SphericalGrid::fillData(const SphericalData& data) {

	const std::vector<SphericalDatum>& dataPoints = data.getData();
	for (const SphericalDatum& d : dataPoints) {
		for (int i = 0; i < 8; i++) {

			// If contained in octant no other octant can contain point
			if (octants[i]->contains(d)) {
				octants[i]->fillData(d, data.getInfo());
				break;
			}
		}
	}
}

// Number of LG cells at depth (single octanct)
int SphericalGrid::lgCells(int depth) {

	if (depth == 0) {
		return 0;
	}
	else if (depth == 1) {
		return 1;
	}
	else {
		return lgCells(depth - 1) * 2 + 1;
	}
}

// Calculates number of cells at depth (single octanct)
int SphericalGrid::numCells(int depth) {

	if (depth == 0) {
		return 1;
	}
	else {
		int lgCell = lgCells(depth - 1);
		return 8 * (numCells(depth - 1) - lgCell - 1) + 6 * lgCell + 4;
	}
}
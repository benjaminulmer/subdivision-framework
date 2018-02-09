#include "SphericalGrid.h"

#define _USE_MATH_DEFINES
#include <math.h>

// Creates an spherical grid with the given radius
// Does not automatically subdivide
SphericalGrid::SphericalGrid(GridInfo& info) : 
	info(info), numLevels(0) {

	// Create starting octants of SDOG
	if (info.scheme == Scheme::SDOG || info.scheme == Scheme::OPT_SDOG || info.scheme == Scheme::TERNARY) {
		octants[0] = new SphericalCell(CellType::SG, info, info.radius, 0.0, -M_PI / 2, 0.0, -M_PI / 2, 0.0);
		octants[1] = new SphericalCell(CellType::SG, info, info.radius, 0.0, -M_PI / 2, 0.0, M_PI / 2, 0.0);
		octants[2] = new SphericalCell(CellType::SG, info, info.radius, 0.0, M_PI / 2, 0.0, -M_PI / 2, 0.0);
		octants[3] = new SphericalCell(CellType::SG, info, info.radius, 0.0, M_PI / 2, 0.0, M_PI / 2, 0.0);

		octants[4] = new SphericalCell(CellType::SG, info, info.radius, 0.0, -M_PI / 2, 0.0, -M_PI, -M_PI / 2);
		octants[5] = new SphericalCell(CellType::SG, info, info.radius, 0.0, -M_PI / 2, 0.0, M_PI, M_PI / 2);
		octants[6] = new SphericalCell(CellType::SG, info, info.radius, 0.0, M_PI / 2, 0.0, -M_PI, -M_PI / 2);
		octants[7] = new SphericalCell(CellType::SG, info, info.radius, 0.0, M_PI / 2, 0.0, M_PI, M_PI / 2);
	}
	else {//(scheme == Scheme::NAIVE || scheme == Scheme::VOLUME)
		octants[0] = new SphericalCell(CellType::NG, info, info.radius, 0.0, -M_PI / 2, 0.0, -M_PI / 2, 0.0);
		octants[1] = new SphericalCell(CellType::NG, info, info.radius, 0.0, -M_PI / 2, 0.0, M_PI / 2, 0.0);
		octants[2] = new SphericalCell(CellType::NG, info, info.radius, 0.0, M_PI / 2, 0.0, -M_PI / 2, 0.0);
		octants[3] = new SphericalCell(CellType::NG, info, info.radius, 0.0, M_PI / 2, 0.0, M_PI / 2, 0.0);

		octants[4] = new SphericalCell(CellType::NG, info, info.radius, 0.0, -M_PI / 2, 0.0, -M_PI, -M_PI / 2);
		octants[5] = new SphericalCell(CellType::NG, info, info.radius, 0.0, -M_PI / 2, 0.0, M_PI, M_PI / 2);
		octants[6] = new SphericalCell(CellType::NG, info, info.radius, 0.0, M_PI / 2, 0.0, -M_PI, -M_PI / 2);
		octants[7] = new SphericalCell(CellType::NG, info, info.radius, 0.0, M_PI / 2, 0.0, M_PI, M_PI / 2);
	}
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

	if (info.mode != SubdivisionMode::REP_SLICE) {
		for (int i = 0; i < 8; i++) {
			octants[i]->subdivideTo(level);
		}
	}
	else {
		octants[2]->subdivideTo(level);
	}

}

// Creates a renderable for the spherical grid at the given level
void SphericalGrid::createRenderable(Renderable& r, int level, DisplayMode mode) {

	// Create more levels if needed
	if (level > numLevels) {
		subdivideTo(level);
	}
	if (info.mode != SubdivisionMode::REP_SLICE) {
		for (int i = 0; i < 8; i++) {
			octants[i]->createRenderable(r, level, mode);
		}
	}
	else {
		octants[2]->createRenderable(r, level, mode);
	}
}

// Gets the volume of all cell for the spherical grid at the given level
void SphericalGrid::getVolumes(std::vector<float>& volumes, int level) {

	// Create more levels if needed
	if (level > numLevels) {
		subdivideTo(level);
	}
	octants[2]->getVolumes(volumes, level);
}

// Returns the total number of cells currently in the hierarchy
int SphericalGrid::getNumCells() {
	int toReturn = 0;
	for (int i = 0; i < 8; i++) {
		toReturn += octants[i]->getNumCells();
	}
	return toReturn;
}

// Propogates data down tree to maxLevel
void SphericalGrid::fillData(int level) {

	// Create more levels if needed
	if (level > numLevels) {
		subdivideTo(level);
	}

	const std::vector<SphericalDatum>& data = info.data.getData();
	for (const SphericalDatum& d : data) {
		for (int i = 0; i < 8; i++) {

			// If contained in octant no other octant can contain point
			if (octants[i]->contains(d)) {
				octants[i]->fillData(d, level);
				break;
			}
		}
	}
}

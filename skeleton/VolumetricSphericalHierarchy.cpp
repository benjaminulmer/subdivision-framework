#include "VolumetricSphericalHierarchy.h"

// Creates an SDOG with the given radius
// Does not automatically subdivide
VolumetricSphericalHierarchy::VolumetricSphericalHierarchy(GridInfo& info) : 
	info(info), numLevels(0) {

	// Create starting octants of SDOG
	if (info.scheme == Scheme::SDOG || info.scheme == Scheme::TERNARY) {
		octants[0] = new SphericalGrid(GridType::SG, info, info.radius, 0.0, -M_PI / 2, 0.0, -M_PI / 2, 0.0);
		octants[1] = new SphericalGrid(GridType::SG, info, info.radius, 0.0, -M_PI / 2, 0.0, M_PI / 2, 0.0);
		octants[2] = new SphericalGrid(GridType::SG, info, info.radius, 0.0, M_PI / 2, 0.0, -M_PI / 2, 0.0);
		octants[3] = new SphericalGrid(GridType::SG, info, info.radius, 0.0, M_PI / 2, 0.0, M_PI / 2, 0.0);

		octants[4] = new SphericalGrid(GridType::SG, info, info.radius, 0.0, -M_PI / 2, 0.0, -M_PI, -M_PI / 2);
		octants[5] = new SphericalGrid(GridType::SG, info, info.radius, 0.0, -M_PI / 2, 0.0, M_PI, M_PI / 2);
		octants[6] = new SphericalGrid(GridType::SG, info, info.radius, 0.0, M_PI / 2, 0.0, -M_PI, -M_PI / 2);
		octants[7] = new SphericalGrid(GridType::SG, info, info.radius, 0.0, M_PI / 2, 0.0, M_PI, M_PI / 2);
	}
	else {//(scheme == Scheme::NAIVE || scheme == Scheme::VOLUME)
		octants[0] = new SphericalGrid(GridType::NG, info, info.radius, 0.0, -M_PI / 2, 0.0, -M_PI / 2, 0.0);
		octants[1] = new SphericalGrid(GridType::NG, info, info.radius, 0.0, -M_PI / 2, 0.0, M_PI / 2, 0.0);
		octants[2] = new SphericalGrid(GridType::NG, info, info.radius, 0.0, M_PI / 2, 0.0, -M_PI / 2, 0.0);
		octants[3] = new SphericalGrid(GridType::NG, info, info.radius, 0.0, M_PI / 2, 0.0, M_PI / 2, 0.0);

		octants[4] = new SphericalGrid(GridType::NG, info, info.radius, 0.0, -M_PI / 2, 0.0, -M_PI, -M_PI / 2);
		octants[5] = new SphericalGrid(GridType::NG, info, info.radius, 0.0, -M_PI / 2, 0.0, M_PI, M_PI / 2);
		octants[6] = new SphericalGrid(GridType::NG, info, info.radius, 0.0, M_PI / 2, 0.0, -M_PI, -M_PI / 2);
		octants[7] = new SphericalGrid(GridType::NG, info, info.radius, 0.0, M_PI / 2, 0.0, M_PI, M_PI / 2);
	}
}

// Delete all children (recursively)
VolumetricSphericalHierarchy::~VolumetricSphericalHierarchy() {
	for (int i = 0; i < 8; i++) {
		delete octants[i];
	}
}

// Subdivides SDOG to the desired level (if not already that deep)
void VolumetricSphericalHierarchy::subdivideTo(int level) {

	// Check if enough levels are already created
	if (level <= numLevels || level <= 0) {
		return;
	}

	if (info.mode == SubdivisionMode::FULL) {
		for (int i = 0; i < 8; i++) {
			octants[i]->subdivideTo(level);
		}
	}
	else {
		octants[2]->subdivideTo(level);
	}

}

// Creates a renderable for the SDOG at the given level
void VolumetricSphericalHierarchy::createRenderable(Renderable& r, int level, DisplayMode mode) {

	// Create more levels if needed
	if (level > numLevels) {
		subdivideTo(level);
	}
	if (info.mode == SubdivisionMode::FULL) {
		for (int i = 0; i < 8; i++) {
			octants[i]->createRenderable(r, level, mode);
		}
	}
	else {
		octants[2]->createRenderable(r, level, mode);
	}
}

// Gets the volume of all cell for the SDOG at the given level
void VolumetricSphericalHierarchy::getVolumes(std::vector<float>& volumes, int level) {

	// Create more levels if needed
	if (level > numLevels) {
		subdivideTo(level);
	}
	octants[2]->getVolumes(volumes, level);
}

// Propogates data down tree to maxLevel
void VolumetricSphericalHierarchy::fillData(int level) {

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

int VolumetricSphericalHierarchy::numberOfCells(int depth) {
	if (depth == 1) {
		return 4;
	}
	else if (depth == 0) {
		return 1;
	}
	else {
		return 2 * pow(8, depth - 1) + 2 * numCellsHelper(depth) + numberOfCells(depth - 1);
	}
}

int VolumetricSphericalHierarchy::numCellsHelper(int depth) {
	if (depth == 1) {
		return 1;
	}
	else {
		return 4 * pow(8, depth - 2) + 2 * numCellsHelper(depth - 1);
	}
}

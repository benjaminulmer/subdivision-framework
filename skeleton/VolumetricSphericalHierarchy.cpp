#include "VolumetricSphericalHierarchy.h"

// Creates an SDOG with the given radius
// Does not automatically subdivide
VolumetricSphericalHierarchy::VolumetricSphericalHierarchy(GridInfo& info) : 
	info(info), numLevels(0) {

	// Create starting octants of SDOG
	if (info.scheme == Scheme::SDOG || info.scheme == Scheme::VOLUME_SDOG || info.scheme == Scheme::SDOG_OPT) {
		octants[0] = new SphericalGrid(GridType::SG, info, info.radius, 0.0, -M_PI / 2, 0.0, -M_PI / 2, 0.0);
		octants[1] = new SphericalGrid(GridType::SG, info, info.radius, 0.0, -M_PI / 2, 0.0, M_PI / 2, 0.0);
		octants[2] = new SphericalGrid(GridType::SG, info, info.radius, 0.0, M_PI / 2, 0.0, -M_PI / 2, 0.0);
		octants[3] = new SphericalGrid(GridType::SG, info, info.radius, 0.0, M_PI / 2, 0.0, M_PI / 2, 0.0);

		octants[4] = new SphericalGrid(GridType::SG, info, info.radius, 0.0, -M_PI / 2, 0.0, -M_PI, -M_PI / 2);
		octants[5] = new SphericalGrid(GridType::SG, info, info.radius, 0.0, -M_PI / 2, 0.0, M_PI, M_PI / 2);
		octants[6] = new SphericalGrid(GridType::SG, info, info.radius, 0.0, M_PI / 2, 0.0, -M_PI, -M_PI / 2);
		octants[7] = new SphericalGrid(GridType::SG, info, info.radius, 0.0, M_PI / 2, 0.0, M_PI, M_PI / 2);
	}
	else { // scheme == Scheme::NAIVE || scheme == Scheme::VOLUME
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
void VolumetricSphericalHierarchy::subdivideTo(int level, bool wholeSphere) {

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
void VolumetricSphericalHierarchy::createRenderable(Renderable& r, int level, bool lines, bool wholeSphere) {

	// Create more levels if needed
	if (level > numLevels) {
		subdivideTo(level, wholeSphere);
	}

	if (wholeSphere) {
		for (int i = 0; i < 8; i++) {
			octants[i]->createRenderable(r, level, lines);
		}
	}
	// Only for one octant
	else {
		octants[2]->createRenderable(r, level, lines);
	}
}

// Gets the volume of all cell for the SDOG at the given level
void VolumetricSphericalHierarchy::getVolumes(std::vector<float>& volumes, int level, bool wholeSphere) {

	// Create more levels if needed
	if (level > numLevels) {
		subdivideTo(level, wholeSphere);
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

// Propogates data down tree to maxLevel
void VolumetricSphericalHierarchy::fillData(int level) {

	// Create more levels if needed
	if (level > numLevels) {
		subdivideTo(level, true);
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
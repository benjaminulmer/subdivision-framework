#include "SphericalGrid.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <string>
#include "Constants.h"

// Creates an spherical grid with the given radius
// Does not automatically subdivide
SphericalGrid::SphericalGrid(GridInfo& info) {

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

#include <iostream>

std::string SphericalGrid::codeForPos(double latDeg, double longDeg, double radius, int level) {

	std::string code = "";

	double minLat, maxLat, minLong, maxLong, minRad, maxRad;
	minLat = 0.0;
	maxLat = 90.0;
	minRad = 0.0;
	maxRad = 2.0 * RADIUS_EARTH_KM;

	int octCode = 0;
	if (latDeg < 0.0) {
		octCode += 4;
	}
	if (longDeg < 0.0) {
		octCode += 2;
	}
	if (abs(longDeg) > 90.0) {
		octCode += 1;
		minLong = 90.0;
		maxLong = 180.0;
	}
	else {
		minLong = 0.0;
		maxLong = 90.0;
	}
	latDeg = abs(latDeg);
	longDeg = abs(longDeg);

	code += std::to_string(octCode);

	CellType curType = CellType::SG;

	for (int i = 0; i < level; i++) {
		
		int childCode;
		double midLat = 0.5 * minLat + 0.5 * maxLat;
		double midLong = 0.5 * minLong + 0.5 * maxLong;
		double midRad = 0.5 * minRad + 0.5 * maxRad;

		if (curType == CellType::NG) {

		}
		else if (curType == CellType::LG) {

		}
		else {// curType == CellType::SG

			if (radius > midRad) {

				minRad = midRad;

				if (latDeg < midLat) {

					maxLat = midLat;
					curType == CellType::NG;

					if (longDeg < midLong) {

						childCode = 0;
						maxLong = midLong;
					}
					else {

						childCode = 1;
						minLong = midLong;
					}

				}
				else {
					childCode = 2;
					minLat = midLat;
					curType == CellType::LG;
				}

			}
			else {
				childCode = 3;
				maxRad = midRad;
				// curType == CellType::SG;
			}

		}

		code += std::to_string(childCode);
	}

	return code;
}




// Subdivides spherical grid to the desired level (if not already that deep)
void SphericalGrid::subdivide() {

	for (int i = 0; i < 8; i++) {
		octants[i]->subdivide();
	}
}

// Creates a renderable for the spherical grid at the given level
void SphericalGrid::createRenderable(Renderable& r, int level, DisplayMode mode) {

	for (int i = 0; i < 8; i++) {
		octants[i]->createRenderable(r, level, mode);
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

// Returns number of leaf cells in tree
int SphericalGrid::countLeafs() {
	int count = 0;

	for (int i = 0; i < 8; i++) {
		count += octants[i]->countLeafs();
	}
	return count;
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
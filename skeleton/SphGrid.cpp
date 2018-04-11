#include "SphGrid.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "Constants.h"

SphCell::SphCell() :
	minLat(0.0), maxLat(0.0), minLong(0.0), maxLong(0.0), minRad(0.0), maxRad(0.0) {}

SphCell::SphCell(double minLat, double maxLat, double minLong, double maxLong, double minRad, double maxRad) :
	minLat(minLat), maxLat(maxLat), minLong(minLong), maxLong(maxLong), minRad(minRad), maxRad(maxRad) {}

CellType SphCell::type() {
	if (minRad == 0.0) {
		return CellType::SG;
	}
	else if (maxLat == M_PI_2 || maxLat == -M_PI_2) {
		return CellType::LG;
	}
	else {
		return CellType::NG;
	}
}

SphGrid::SphGrid(double radius) : maxDepth(0), maxRadius(radius) {

	map["0"] = SphCell(0.0, M_PI_2, 0.0, M_PI_2, 0.0, radius);
	map["1"] = SphCell(0.0, M_PI_2, M_PI_2, M_PI, 0.0, radius);
	map["3"] = SphCell(0.0, M_PI_2, 0.0, -M_PI_2, 0.0, radius);
	map["4"] = SphCell(0.0, M_PI_2, -M_PI_2, -M_PI, 0.0, radius);

	map["5"] = SphCell(0.0, -M_PI_2, 0.0, M_PI_2, 0.0, radius);
	map["6"] = SphCell(0.0, -M_PI_2, M_PI_2, M_PI, 0.0, radius);
	map["7"] = SphCell(0.0, -M_PI_2, 0.0, -M_PI_2, 0.0, radius);
	map["8"] = SphCell(0.0, -M_PI_2, -M_PI_2, -M_PI, 0.0, radius);
}

void SphGrid::subdivideTo(int level) {

	for (; maxDepth < level; maxDepth++) {
		subdivide();
	}
	maxDepth = level;
}

void SphGrid::subdivide() {

	for (std::pair<std::string, SphCell> p : map) {
		if (p.first.size() == maxDepth + 1) {
			subdivideCell(p.first, p.second);
		}
	}
}

void SphGrid::subdivideCell(std::string code, SphCell cell) {

	double midLat = 0.5 * cell.minLat + 0.5 * cell.maxLat;
	double midLong = 0.5 * cell.minLong + 0.5 * cell.maxLong;
	double midRad = 0.5 * cell.minRad + 0.5 * cell.maxRad;

	if (cell.type() == CellType::NG) {
		map[code + "0"] = SphCell(cell.minLat, midLat, cell.minLong, midLong, midRad, cell.maxRad);
		map[code + "1"] = SphCell(cell.minLat, midLat, midLong, cell.maxLong, midRad, cell.maxRad);
		map[code + "2"] = SphCell(midLat, cell.maxLat, cell.minLong, midLong, midRad, cell.maxRad);
		map[code + "3"] = SphCell(midLat, cell.maxLat, midLong, cell.maxLong, midRad, cell.maxRad);

		map[code + "4"] = SphCell(cell.minLat, midLat, cell.minLong, midLong, cell.minRad, midRad);
		map[code + "5"] = SphCell(cell.minLat, midLat, midLong, cell.maxLong, cell.minRad, midRad);
		map[code + "6"] = SphCell(midLat, cell.maxLat, cell.minLong, midLong, cell.minRad, midRad);
		map[code + "7"] = SphCell(midLat, cell.maxLat, midLong, cell.maxLong, cell.minRad, midRad);
	}
	else if (cell.type() == CellType::LG) {
		map[code + "0"] = SphCell(cell.minLat, midLat, cell.minLong, midLong, midRad, cell.maxRad);
		map[code + "1"] = SphCell(cell.minLat, midLat, midLong, cell.maxLong, midRad, cell.maxRad);
		map[code + "2"] = SphCell(midLat, cell.maxLat, cell.minLong, cell.maxLong, midRad, cell.maxRad);

		map[code + "3"] = SphCell(cell.minLat, midLat, cell.minLong, midLong, cell.minRad, midRad);
		map[code + "4"] = SphCell(cell.minLat, midLat, midLong, cell.maxLong, cell.minRad, midRad);
		map[code + "5"] = SphCell(midLat, cell.maxLat, cell.minLong, cell.maxLong, cell.minRad, midRad);
	}
	else {// cell.type() == CellType::SG
		map[code + "0"] = SphCell(cell.minLat, midLat, cell.minLong, midLong, midRad, cell.maxRad);
		map[code + "1"] = SphCell(cell.minLat, midLat, midLong, cell.maxLong, midRad, cell.maxRad);
		map[code + "2"] = SphCell(midLat, cell.maxLat, cell.minLong, cell.maxLong, midRad, cell.maxRad);

		map[code + "3"] = SphCell(cell.minLat, cell.maxLat, cell.minLong, cell.maxLong, cell.minRad, midRad);
	}
}

void SphGrid::fillData(const SphericalData& data) {

}

std::string SphGrid::codeForPos(double latDeg, double longDeg, double radius, int level) {

	std::string code = "";

	double minLat, maxLat, minLong, maxLong, minRad, maxRad;
	minLat = 0.0;
	maxLat = 90.0;
	minRad = 0.0;
	maxRad = maxRadius;

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

		int childCode = 0;
		double midLat = 0.5 * minLat + 0.5 * maxLat;
		double midLong = 0.5 * minLong + 0.5 * maxLong;
		double midRad = 0.5 * minRad + 0.5 * maxRad;

		if (curType == CellType::NG) {

			if (radius > midRad) {
				minRad = midRad;
			}
			else {
				childCode += 4;
				maxRad = midRad;
			}

			if (latDeg < midLat) {
				maxLat = midLat;
			}
			else {
				childCode += 2;
				minLat = midLat;
			}
			if (longDeg < midLong) {
				maxLong = midLong;
			}
			else {
				childCode += 1;
				minLong = midLong;
			}
			// type doesn't change
		}
		else if (curType == CellType::LG) {

			if (radius > midRad) {
				minRad = midRad;
			}
			else {
				maxRad = midRad;
				childCode += 3;
			}

			if (latDeg < midLat) {

				curType = CellType::NG;

				if (longDeg < midLong) {
					maxLong = midLong;
				}
				else {
					childCode += 1;
					minLong = midLong;
				}

			}
			else {
				childCode += 2;
				minLat = midLat;
				// type doesn't change
			}

		}
		else {// curType == CellType::SG

			if (radius > midRad) {

				minRad = midRad;

				if (latDeg < midLat) {
					maxLat = midLat;
					curType = CellType::NG;

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
					curType = CellType::LG;
				}

			}
			else {
				childCode = 3;
				maxRad = midRad;
				// type doesn't change
			}

		}
		code += std::to_string(childCode);
	}

	return code;
}

SphCell SphGrid::cellFromCode(std::string code) {
	if (map.count(code) == 1) {
		return map[code];
	}
	else {
		return SphCell();
	}
}
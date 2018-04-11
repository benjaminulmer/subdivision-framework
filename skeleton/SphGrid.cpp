#include "SphGrid.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "Constants.h"
#include "Geometry.h"


#include <iostream>

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

	map["0"] = new SphCell(0.0, M_PI_2, 0.0, M_PI_2, 0.0, radius);
	map["1"] = new SphCell(0.0, M_PI_2, M_PI_2, M_PI, 0.0, radius);
	map["2"] = new SphCell(0.0, M_PI_2, 0.0, -M_PI_2, 0.0, radius);
	map["3"] = new SphCell(0.0, M_PI_2, -M_PI_2, -M_PI, 0.0, radius);

	map["4"] = new SphCell(0.0, -M_PI_2, 0.0, M_PI_2, 0.0, radius);
	map["5"] = new SphCell(0.0, -M_PI_2, M_PI_2, M_PI, 0.0, radius);
	map["6"] = new SphCell(0.0, -M_PI_2, 0.0, -M_PI_2, 0.0, radius);
	map["7"] = new SphCell(0.0, -M_PI_2, -M_PI_2, -M_PI, 0.0, radius);
}

void SphGrid::subdivideTo(int level) {

	for (int i = maxDepth; i < level; i++) {
		subdivide();
	}
}

void SphGrid::subdivide() {

	for (std::pair<std::string, SphCell*> p : map) {
		if (p.first.size() == maxDepth + 1 && p.second->dataSets.size() != 0) {
			subdivideCell(p.first, p.second);
		}
	}
	maxDepth++;
}

void SphGrid::subdivideCell(std::string code, SphCell* cell) {

	double midLat = 0.5 * cell->minLat + 0.5 * cell->maxLat;
	double midLong = 0.5 * cell->minLong + 0.5 * cell->maxLong;
	double midRad = 0.5 * cell->minRad + 0.5 * cell->maxRad;

	if (cell->type() == CellType::NG) {
		map[code + "0"] = new SphCell(cell->minLat, midLat, cell->minLong, midLong, midRad, cell->maxRad);
		map[code + "1"] = new SphCell(cell->minLat, midLat, midLong, cell->maxLong, midRad, cell->maxRad);
		map[code + "2"] = new SphCell(midLat, cell->maxLat, cell->minLong, midLong, midRad, cell->maxRad);
		map[code + "3"] = new SphCell(midLat, cell->maxLat, midLong, cell->maxLong, midRad, cell->maxRad);

		map[code + "4"] = new SphCell(cell->minLat, midLat, cell->minLong, midLong, cell->minRad, midRad);
		map[code + "5"] = new SphCell(cell->minLat, midLat, midLong, cell->maxLong, cell->minRad, midRad);
		map[code + "6"] = new SphCell(midLat, cell->maxLat, cell->minLong, midLong, cell->minRad, midRad);
		map[code + "7"] = new SphCell(midLat, cell->maxLat, midLong, cell->maxLong, cell->minRad, midRad);
	}
	else if (cell->type() == CellType::LG) {
		map[code + "0"] = new SphCell(cell->minLat, midLat, cell->minLong, midLong, midRad, cell->maxRad);
		map[code + "1"] = new SphCell(cell->minLat, midLat, midLong, cell->maxLong, midRad, cell->maxRad);
		map[code + "2"] = new SphCell(midLat, cell->maxLat, cell->minLong, cell->maxLong, midRad, cell->maxRad);

		map[code + "3"] = new SphCell(cell->minLat, midLat, cell->minLong, midLong, cell->minRad, midRad);
		map[code + "4"] = new SphCell(cell->minLat, midLat, midLong, cell->maxLong, cell->minRad, midRad);
		map[code + "5"] = new SphCell(midLat, cell->maxLat, cell->minLong, cell->maxLong, cell->minRad, midRad);
	}
	else {// cell.type() == CellType::SG
		map[code + "0"] = new SphCell(cell->minLat, midLat, cell->minLong, midLong, midRad, cell->maxRad);
		map[code + "1"] = new SphCell(cell->minLat, midLat, midLong, cell->maxLong, midRad, cell->maxRad);
		map[code + "2"] = new SphCell(midLat, cell->maxLat, cell->minLong, cell->maxLong, midRad, cell->maxRad);

		map[code + "3"] = new SphCell(cell->minLat, cell->maxLat, cell->minLong, cell->maxLong, cell->minRad, midRad);
	}
}

void SphGrid::fillData(const SphericalData& data) {

	const std::vector<SphericalDatum>& dataPoints = data.getData();
	const DataSetInfo& info = data.getInfo();

	for (const SphericalDatum& d : dataPoints) {
		
		std::string code = codeForPos(d.latitude, d.longitude, d.radius, maxDepth);
		SphCell* c = map[code];

		// Find index in list of data sets for the point
		int index = -1;
		int i = 0;
		for (DataPoints& ds : c->dataSets) {

			if (ds.info.id == info.id) {
				index = i;
				break;
			}
			i++;
		}

		// If data point belongs to new data set add a new data set
		if (index == -1) {
			c->dataSets.push_back(DataPoints(info));
			index = c->dataSets.size() - 1;
		}
		c->dataSets[index].data.push_back(d);
	}
}

void SphGrid::createRenderable(Renderable& r, int level) {

	for (std::pair<std::string, SphCell*> p : map) {

		if (p.first.size() == level + 1) {

			double minLong = p.second->minLong; double maxLong = p.second->maxLong;
			double minLat = p.second->minLat; double maxLat = p.second->maxLat;
			double minRad = p.second->minRad; double maxRad = p.second->maxRad;

			// fill renderable here
			r.drawMode = GL_LINES;
			glm::vec3 origin(0.f, 0.f, 0.f);

			// Outer points
			glm::vec3 o1 = glm::vec3(sin(minLong)*cos(minLat), sin(minLat), cos(minLong)*cos(minLat)) * (float)maxRad;
			glm::vec3 o2 = glm::vec3(sin(maxLong)*cos(minLat), sin(minLat), cos(maxLong)*cos(minLat)) * (float)maxRad;
			glm::vec3 o3 = glm::vec3(sin(minLong)*cos(maxLat), sin(maxLat), cos(minLong)*cos(maxLat)) * (float)maxRad;
			glm::vec3 o4 = glm::vec3(sin(maxLong)*cos(maxLat), sin(maxLat), cos(maxLong)*cos(maxLat)) * (float)maxRad;

			// Inner points
			glm::vec3 i1 = glm::vec3(sin(minLong)*cos(minLat), sin(minLat), cos(minLong)*cos(minLat)) * (float)minRad;
			glm::vec3 i2 = glm::vec3(sin(maxLong)*cos(minLat), sin(minLat), cos(maxLong)*cos(minLat)) * (float)minRad;
			glm::vec3 i3 = glm::vec3(sin(minLong)*cos(maxLat), sin(maxLat), cos(minLong)*cos(maxLat)) * (float)minRad;
			glm::vec3 i4 = glm::vec3(sin(maxLong)*cos(maxLat), sin(maxLat), cos(maxLong)*cos(maxLat)) * (float)minRad;

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
			Geometry::createArcR(o1, o2, glm::vec3(0.f, sin(minLat), 0.f) * (float)maxRad, r);
			Geometry::createArcR(o3, o4, glm::vec3(0.f, sin(maxLat), 0.f) * (float)maxRad, r);
			Geometry::createArcR(i1, i2, glm::vec3(0.f, sin(minLat), 0.f) * (float)minRad, r);
			Geometry::createArcR(i3, i4, glm::vec3(0.f, sin(maxLat), 0.f) * (float)minRad, r);
		}
	}
}

int SphGrid::countLeafs() {

	int count = 0;
	for (std::pair<std::string, SphCell*> p : map) {
		if (p.first.size() == maxDepth + 1) {
			count++;
		}
	}
	return count;
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

const SphCell* SphGrid::cellFromCode(std::string code) {
	if (map.count(code) == 1) {
		return map[code];
	}
	else {
		return nullptr;
	}
}
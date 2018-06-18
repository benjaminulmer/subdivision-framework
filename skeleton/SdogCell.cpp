#define _USE_MATH_DEFINES
#include "SdogCell.h"

#include <algorithm>
#include <cmath>

#include <iostream>

// Constructs the SDOG cell for the given code
//
// code - index code of the desired SDOG cell
// maxRadius - maximum radius of the SDOG grid the cell belongs to
SdogCell::SdogCell(const std::string& code, double gridRadius) : code(code), gridRadius(gridRadius) {

	minRad = 0.0;
	maxRad = gridRadius;

	// Set initial values based on which octanct cell is in
	if (code[0] == '0') {
		minLat = 0.0;
		maxLat = M_PI_2;
		minLong = 0.0;
		maxLong = M_PI_2;
	}
	else if (code[0] == '1') {
		minLat = 0.0;
		maxLat = M_PI_2;
		minLong = M_PI_2;
		maxLong = M_PI;
	}
	else if (code[0] == '2') {
		minLat = 0.0;
		maxLat = M_PI_2;
		minLong = 0.0;
		maxLong = -M_PI_2;
	}
	else if (code[0] == '3') {
		minLat = 0.0;
		maxLat = M_PI_2;
		minLong = -M_PI_2;
		maxLong = -M_PI;
	}
	else if (code[0] == '4') {
		minLat = 0.0;
		maxLat = -M_PI_2;
		minLong = 0.0;
		maxLong = M_PI_2;
	}
	else if (code[0] == '5') {
		minLat = 0.0;
		maxLat = -M_PI_2;
		minLong = M_PI_2;
		maxLong = M_PI;
	}
	else if (code[0] == '6') {
		minLat = 0.0;
		maxLat = -M_PI_2;
		minLong = 0.0;
		maxLong = -M_PI_2;
	}
	else if (code[0] == '7') {
		minLat = 0.0;
		maxLat = -M_PI_2;
		minLong = -M_PI_2;
		maxLong = -M_PI;
	}
	else {
		// error
	}

	// Loop for each char in code and determine properties based on code
	type = SdogCellType::SG;
	for (unsigned int i = 1; i < code.length(); i++) {

		double midLat = 0.5 * minLat + 0.5 * maxLat;
		double midLong = 0.5 * minLong + 0.5 * maxLong;
		double midRad = 0.5 * minRad + 0.5 * maxRad;

		if (type == SdogCellType::NG) {

			if (code[i] == '0') {
				minRad = midRad;
				maxLat = midLat;
				maxLong = midLong;
			}
			else if (code[i] == '1') {
				minRad = midRad;
				maxLat = midLat;
				minLong = midLong;
			}
			else if (code[i] == '2') {
				minRad = midRad;
				minLat = midLat;
				maxLong = midLong;
			}
			else if (code[i] == '3') {
				minRad = midRad;
				minLat = midLat;
				minLong = midLong;
			}
			else if (code[i] == '4') {
				maxRad = midRad;
				maxLat = midLat;
				maxLong = midLong;
			}
			else if (code[i] == '5') {
				maxRad = midRad;
				maxLat = midLat;
				minLong = midLong;
			}
			else if (code[i] == '6') {
				maxRad = midRad;
				minLat = midLat;
				maxLong = midLong;
			}
			else if (code[i] == '7') {
				maxRad = midRad;
				minLat = midLat;
				minLong = midLong;
			}
			else {
				type = SdogCellType::INVALID;
				break;
			}
			// type doesn't change
		}
		else if (type == SdogCellType::LG) {

			if (code[i] == '0') {
				minRad = midRad;
				maxLat = midLat;
				maxLong = midLong;
				type = SdogCellType::NG;
			}
			else if (code[i] == '1') {
				minRad = midRad;
				maxLat = midLat;
				minLong = midLong;
				type = SdogCellType::NG;
			}
			else if (code[i] == '2') {
				minRad = midRad;
				minLat = midLat;
				// type doesn't change
			}
			else if (code[i] == '3') {
				maxRad = midRad;
				maxLat = midLat;
				maxLong = midLong;
				type = SdogCellType::NG;
			}
			else if (code[i] == '4') {
				maxRad = midRad;
				maxLat = midLat;
				minLong = midLong;
				type = SdogCellType::NG;
			}
			else if (code[i] == '5') {
				maxRad = midRad;
				minLat = midLat;
				// type doesn't change
			}
			else {
				type = SdogCellType::INVALID;
				break;
			}
		}
		else {// type == CellType::SG

			if (code[i] == '0') {
				minRad = midRad;
				maxLat = midLat;
				maxLong = midLong;
				type = SdogCellType::NG;
			}
			else if (code[i] == '1') {
				minRad = midRad;
				maxLat = midLat;
				minLong = midLong;
				type = SdogCellType::NG;
			}
			else if (code[i] == '2') {
				minRad = midRad;
				minLat = midLat;
				type = SdogCellType::LG;
			}
			else if (code[i] == '3') {
				maxRad = midRad;
				// type doesn't change
			}
			else {
				type = SdogCellType::INVALID;
				break;
			}
		}
	}
}


// Returns if the cell contains the provided point
//
// latRad - latitude of point in radians
// longRad - longitude of point in radians
// radius - radius of point
bool SdogCell::contains(double latRad, double longRad, double radius) {
	
	if (type == SdogCellType::INVALID) return false;

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
	return (latRad <= rMaxLat && latRad > rMinLat && longRad <= rMaxLong && longRad > rMinLong &&
		radius <= maxRad && radius > minRad);
}


// Gets the codes of the children of the cell
//
// out - output vector that stores the children - treats as empty
void SdogCell::children(std::vector<std::string>& out) {

	if (type == SdogCellType::INVALID) return;

	if (type == SdogCellType::NG) {
		out.push_back(code + "0");
		out.push_back(code + "1");
		out.push_back(code + "2");
		out.push_back(code + "3");
		out.push_back(code + "4");
		out.push_back(code + "5");
		out.push_back(code + "6");
		out.push_back(code + "7");
	}
	else if (type == SdogCellType::LG) {
		out.push_back(code + "0");
		out.push_back(code + "1");
		out.push_back(code + "2");
		out.push_back(code + "3");
		out.push_back(code + "4");
		out.push_back(code + "5");
	}
	else {// type == SdogCellType::SG
		out.push_back(code + "0");
		out.push_back(code + "1");
		out.push_back(code + "2");
		out.push_back(code + "3");
	}
}


// Gets the codes of the neighbours that share a face with the cell
//
// out - output vector that stores the neighbours - treats as empty
void SdogCell::neighbours(std::vector<std::string>& out) {

	if (type == SdogCellType::INVALID) return;

	unsigned int level = (unsigned int)code.length() - 1;

	double midLat = 0.5 * minLat + 0.5 * maxLat;
	double midLong = 0.5 * minLong + 0.5 * maxLong;
	double midRad = 0.5 * minRad + 0.5 * maxRad;

	double latDist = maxLat - minLat;
	double longDist = maxLong - minLong;
	double radDist = maxRad - minRad;

	// Get codes for location of all posible neighbours
	out.push_back(codeForPos(midLat + latDist, midLong, midRad, gridRadius, level));
	out.push_back(codeForPos(midLat, midLong + longDist, midRad, gridRadius, level));
	out.push_back(codeForPos(midLat, midLong - longDist, midRad, gridRadius, level));
	out.push_back(codeForPos(midLat, midLong, midRad - radDist, gridRadius, level));

	out.push_back(codeForPos(midLat - latDist, midLong + 0.01 * longDist, midRad, gridRadius, level));
	out.push_back(codeForPos(midLat - latDist, midLong - 0.01 * longDist, midRad, gridRadius, level));

	out.push_back(codeForPos(midLat + 0.01 * latDist, midLong + 0.01 * longDist, midRad + radDist, gridRadius, level));
	out.push_back(codeForPos(midLat + 0.01 * latDist, midLong - 0.01 * longDist, midRad + radDist, gridRadius, level));
	out.push_back(codeForPos(midLat - 0.01 * latDist, midLong + 0.01 * longDist, midRad + radDist, gridRadius, level));
	out.push_back(codeForPos(midLat - 0.01 * latDist, midLong - 0.01 * longDist, midRad + radDist, gridRadius, level));

	// Remove duplicates and self
	std::sort(out.begin(), out.end());
	out.erase(std::unique(out.begin(), out.end()), out.end());
	out.erase(std::remove(out.begin(), out.end(), code), out.end());

	//std::cout << out.size() << std::endl;
}

void SdogCell::getUpNeighbours(std::vector<std::string>& out) {

	if (type == SdogCellType::INVALID) return;

	unsigned int level = (unsigned int)code.length() - 1;

	double midLat = 0.5 * minLat + 0.5 * maxLat;
	double midLong = 0.5 * minLong + 0.5 * maxLong;
	double midRad = 0.5 * minRad + 0.5 * maxRad;

	double latDist = maxLat - minLat;
	double longDist = maxLong - minLong;
	double radDist = maxRad - minRad;

	// Get codes for location of all posible neighbours
	out.push_back(codeForPos(midLat + latDist, midLong, midRad, gridRadius, level));
}

void SdogCell::getDownNeighbours(std::vector<std::string>& out) {

	if (type == SdogCellType::INVALID) return;

	unsigned int level = (unsigned int)code.length() - 1;

	double midLat = 0.5 * minLat + 0.5 * maxLat;
	double midLong = 0.5 * minLong + 0.5 * maxLong;
	double midRad = 0.5 * minRad + 0.5 * maxRad;

	double latDist = maxLat - minLat;
	double longDist = maxLong - minLong;
	double radDist = maxRad - minRad;

	// Get codes for location of all posible neighbours
	out.push_back(codeForPos(midLat - latDist, midLong + 0.01 * longDist, midRad, gridRadius, level));
	out.push_back(codeForPos(midLat - latDist, midLong - 0.01 * longDist, midRad, gridRadius, level));

	// Remove duplicates and self
	std::sort(out.begin(), out.end());
	out.erase(std::unique(out.begin(), out.end()), out.end());
	out.erase(std::remove(out.begin(), out.end(), code), out.end());
}

void SdogCell::getInNeighbours(std::vector<std::string>& out) {

	if (type == SdogCellType::INVALID) return;

	unsigned int level = (unsigned int)code.length() - 1;

	double midLat = 0.5 * minLat + 0.5 * maxLat;
	double midLong = 0.5 * minLong + 0.5 * maxLong;
	double midRad = 0.5 * minRad + 0.5 * maxRad;

	double latDist = maxLat - minLat;
	double longDist = maxLong - minLong;
	double radDist = maxRad - minRad;

	// Get codes for location of all posible neighbours
	out.push_back(codeForPos(midLat, midLong, midRad - radDist, gridRadius, level));
}

void SdogCell::getOutNeighbours(std::vector<std::string>& out) {

	if (type == SdogCellType::INVALID) return;

	unsigned int level = (unsigned int)code.length() - 1;

	double midLat = 0.5 * minLat + 0.5 * maxLat;
	double midLong = 0.5 * minLong + 0.5 * maxLong;
	double midRad = 0.5 * minRad + 0.5 * maxRad;

	double latDist = maxLat - minLat;
	double longDist = maxLong - minLong;
	double radDist = maxRad - minRad;

	// Get codes for location of all posible neighbours
	out.push_back(codeForPos(midLat + 0.01 * latDist, midLong + 0.01 * longDist, midRad + radDist, gridRadius, level));
	out.push_back(codeForPos(midLat + 0.01 * latDist, midLong - 0.01 * longDist, midRad + radDist, gridRadius, level));
	out.push_back(codeForPos(midLat - 0.01 * latDist, midLong + 0.01 * longDist, midRad + radDist, gridRadius, level));
	out.push_back(codeForPos(midLat - 0.01 * latDist, midLong - 0.01 * longDist, midRad + radDist, gridRadius, level));

	// Remove duplicates and self
	std::sort(out.begin(), out.end());
	out.erase(std::unique(out.begin(), out.end()), out.end());
	out.erase(std::remove(out.begin(), out.end(), code), out.end());
}

void SdogCell::getRightNeighbours(std::vector<std::string>& out) {

	if (type == SdogCellType::INVALID) return;

	unsigned int level = (unsigned int)code.length() - 1;

	double midLat = 0.5 * minLat + 0.5 * maxLat;
	double midLong = 0.5 * minLong + 0.5 * maxLong;
	double midRad = 0.5 * minRad + 0.5 * maxRad;

	double latDist = maxLat - minLat;
	double longDist = maxLong - minLong;
	double radDist = maxRad - minRad;

	// Get codes for location of all posible neighbours
	out.push_back(codeForPos(midLat, midLong - longDist, midRad, gridRadius, level));
}

void SdogCell::getLeftNeighbours(std::vector<std::string>& out) {

	if (type == SdogCellType::INVALID) return;

	unsigned int level = (unsigned int)code.length() - 1;

	double midLat = 0.5 * minLat + 0.5 * maxLat;
	double midLong = 0.5 * minLong + 0.5 * maxLong;
	double midRad = 0.5 * minRad + 0.5 * maxRad;

	double latDist = maxLat - minLat;
	double longDist = maxLong - minLong;
	double radDist = maxRad - minRad;

	// Get codes for location of all posible neighbours
	out.push_back(codeForPos(midLat, midLong + longDist, midRad, gridRadius, level));
}
//
//
//void SdogCell::checkNeighbors() {
//	if (type == SdogCellType::INVALID) return;
//
//	unsigned int level = (unsigned int)code.length() - 1;
//
//	double midLat = 0.5 * minLat + 0.5 * maxLat;
//	double midLong = 0.5 * minLong + 0.5 * maxLong;
//	double midRad = 0.5 * minRad + 0.5 * maxRad;
//
//	double latDist = maxLat - minLat;
//	double longDist = maxLong - minLong;
//	double radDist = maxRad - minRad;
//
//	// Get codes for location of all posible neighbours
//	out.push_back(codeForPos(midLat + latDist, midLong, midRad, gridRadius, level));
//	out.push_back(codeForPos(midLat, midLong + longDist, midRad, gridRadius, level));
//	out.push_back(codeForPos(midLat, midLong - longDist, midRad, gridRadius, level));
//	out.push_back(codeForPos(midLat, midLong, midRad - radDist, gridRadius, level));
//
//	out.push_back(codeForPos(midLat - latDist, midLong + 0.01 * longDist, midRad, gridRadius, level));
//	out.push_back(codeForPos(midLat - latDist, midLong - 0.01 * longDist, midRad, gridRadius, level));
//
//	out.push_back(codeForPos(midLat + 0.01 * latDist, midLong + 0.01 * longDist, midRad + radDist, gridRadius, level));
//	out.push_back(codeForPos(midLat + 0.01 * latDist, midLong - 0.01 * longDist, midRad + radDist, gridRadius, level));
//	out.push_back(codeForPos(midLat - 0.01 * latDist, midLong + 0.01 * longDist, midRad + radDist, gridRadius, level));
//	out.push_back(codeForPos(midLat - 0.01 * latDist, midLong - 0.01 * longDist, midRad + radDist, gridRadius, level));
//
//	// Remove duplicates and self
//	std::sort(out.begin(), out.end());
//	out.erase(std::unique(out.begin(), out.end()), out.end());
//	out.erase(std::remove(out.begin(), out.end(), code), out.end());
//}

void SdogCell::addToRenderable(Renderable& r, const glm::vec3& colour, Renderable& p) {
	// Anything using 'p' is temporary - used to visualize borders of individual cells

	glm::vec3 o1 = glm::vec3(sin(minLong)*cos(minLat), sin(minLat), cos(minLong)*cos(minLat)) * (float)maxRad;
	glm::vec3 o2 = glm::vec3(sin(maxLong)*cos(minLat), sin(minLat), cos(maxLong)*cos(minLat)) * (float)maxRad;
	glm::vec3 o3 = glm::vec3(sin(minLong)*cos(maxLat), sin(maxLat), cos(minLong)*cos(maxLat)) * (float)maxRad;
	glm::vec3 o4 = glm::vec3(sin(maxLong)*cos(maxLat), sin(maxLat), cos(maxLong)*cos(maxLat)) * (float)maxRad;

	// Inner points
	glm::vec3 i1 = glm::vec3(sin(minLong)*cos(minLat), sin(minLat), cos(minLong)*cos(minLat)) * (float)minRad;
	glm::vec3 i2 = glm::vec3(sin(maxLong)*cos(minLat), sin(minLat), cos(maxLong)*cos(minLat)) * (float)minRad;
	glm::vec3 i3 = glm::vec3(sin(minLong)*cos(maxLat), sin(maxLat), cos(minLong)*cos(maxLat)) * (float)minRad;
	glm::vec3 i4 = glm::vec3(sin(maxLong)*cos(maxLat), sin(maxLat), cos(maxLong)*cos(maxLat)) * (float)minRad;

	// Outside and inside
	// Outside
	r.verts.push_back(o1); r.verts.push_back(o2); r.verts.push_back(o4);
	r.verts.push_back(o1); r.verts.push_back(o3); r.verts.push_back(o4);
	
	p.verts.push_back(o1); p.verts.push_back(o2); 
	p.verts.push_back(o2); p.verts.push_back(o4);
	p.verts.push_back(o4); p.verts.push_back(o1);

	p.verts.push_back(o1); p.verts.push_back(o3); 
	p.verts.push_back(o3); p.verts.push_back(o4);
	p.verts.push_back(o4); p.verts.push_back(o1);

	// Inside
	r.verts.push_back(i1); r.verts.push_back(i2); r.verts.push_back(i4);
	r.verts.push_back(i1); r.verts.push_back(i3); r.verts.push_back(i4);
	
	p.verts.push_back(i1); p.verts.push_back(i2);
	p.verts.push_back(i2); p.verts.push_back(i4);
	p.verts.push_back(i4); p.verts.push_back(i1);

	p.verts.push_back(i1); p.verts.push_back(i3); 
	p.verts.push_back(i3); p.verts.push_back(i4);
	p.verts.push_back(i4); p.verts.push_back(i1);

	// Sides

	// Right side
	r.verts.push_back(o1); r.verts.push_back(i1); r.verts.push_back(i3);
	r.verts.push_back(o1); r.verts.push_back(o3); r.verts.push_back(i3);

	// Left side
	r.verts.push_back(o2); r.verts.push_back(i2); r.verts.push_back(i4);
	r.verts.push_back(o2); r.verts.push_back(o4); r.verts.push_back(i4);
	
	p.verts.push_back(o1); p.verts.push_back(i1); 
	p.verts.push_back(i1); p.verts.push_back(i3);
	p.verts.push_back(i3); p.verts.push_back(o1);

	p.verts.push_back(o1); p.verts.push_back(o3);
	p.verts.push_back(o3); p.verts.push_back(i3);
	p.verts.push_back(i3); p.verts.push_back(o1);


	p.verts.push_back(o2); p.verts.push_back(i2); 
	p.verts.push_back(i2); p.verts.push_back(i4);
	p.verts.push_back(i4); p.verts.push_back(o2);

	p.verts.push_back(o2); p.verts.push_back(o4); 
	p.verts.push_back(o4); p.verts.push_back(i4);
	p.verts.push_back(i4); p.verts.push_back(o2);

	// Top and bottom
	// Top
	r.verts.push_back(o3); r.verts.push_back(i3); r.verts.push_back(i4);
	r.verts.push_back(o3); r.verts.push_back(o4); r.verts.push_back(i4);

	// Bottom
	r.verts.push_back(o1); r.verts.push_back(i1); r.verts.push_back(i2);
	r.verts.push_back(o1); r.verts.push_back(o2); r.verts.push_back(i2);
	
	p.verts.push_back(o3); p.verts.push_back(i3); 
	p.verts.push_back(i3); p.verts.push_back(i4);
	p.verts.push_back(i4); p.verts.push_back(o3);

	p.verts.push_back(o3); p.verts.push_back(o4); 
	p.verts.push_back(o4); p.verts.push_back(i4);
	p.verts.push_back(i4); p.verts.push_back(o3);


	p.verts.push_back(o1); p.verts.push_back(i1); 
	p.verts.push_back(i1); p.verts.push_back(i2);
	p.verts.push_back(i2); p.verts.push_back(o1);

	p.verts.push_back(o1); p.verts.push_back(o2); 
	p.verts.push_back(o2); p.verts.push_back(i2);
	p.verts.push_back(i2); p.verts.push_back(o1);

	for (int i = 0; i < 36; i++) {
		r.colours.push_back(colour);
	}
	for (int i = 0; i < 64; i++) {
		p.colours.push_back(glm::vec3(0.f, 0.f, 0.f));
	}
}

void SdogCell::addToSigmetRenderable(Renderable& r, const glm::vec3& colour, const AirSigmetCells* cell, Renderable& p) {

	// Get one neighboring cell in each direction
	
	if (type == SdogCellType::INVALID) return;

	unsigned int level = (unsigned int)code.length() - 1;

	double midLat = 0.5 * minLat + 0.5 * maxLat;
	double midLong = 0.5 * minLong + 0.5 * maxLong;
	double midRad = 0.5 * minRad + 0.5 * maxRad;

	double latDist = maxLat - minLat;
	double longDist = maxLong - minLong;
	double radDist = maxRad - minRad;
	
	// Anything using 'p' is temporary - used to visualize borders of individual cells
	double rCount = 0;
	double pCount = 0;

	glm::vec3 o1 = glm::vec3(sin(minLong)*cos(minLat), sin(minLat), cos(minLong)*cos(minLat)) * (float)maxRad;
	glm::vec3 o2 = glm::vec3(sin(maxLong)*cos(minLat), sin(minLat), cos(maxLong)*cos(minLat)) * (float)maxRad;
	glm::vec3 o3 = glm::vec3(sin(minLong)*cos(maxLat), sin(maxLat), cos(minLong)*cos(maxLat)) * (float)maxRad;
	glm::vec3 o4 = glm::vec3(sin(maxLong)*cos(maxLat), sin(maxLat), cos(maxLong)*cos(maxLat)) * (float)maxRad;

	// Inner points
	glm::vec3 i1 = glm::vec3(sin(minLong)*cos(minLat), sin(minLat), cos(minLong)*cos(minLat)) * (float)minRad;
	glm::vec3 i2 = glm::vec3(sin(maxLong)*cos(minLat), sin(minLat), cos(maxLong)*cos(minLat)) * (float)minRad;
	glm::vec3 i3 = glm::vec3(sin(minLong)*cos(maxLat), sin(maxLat), cos(minLong)*cos(maxLat)) * (float)minRad;
	glm::vec3 i4 = glm::vec3(sin(maxLong)*cos(maxLat), sin(maxLat), cos(maxLong)*cos(maxLat)) * (float)minRad;

	// Outside and inside
	// Outside
	if (!renderNeighbors.outside) {
		r.verts.push_back(o1); r.verts.push_back(o2); r.verts.push_back(o4);
		r.verts.push_back(o1); r.verts.push_back(o3); r.verts.push_back(o4);

		p.verts.push_back(o1); p.verts.push_back(o2);
		p.verts.push_back(o2); p.verts.push_back(o4);
		p.verts.push_back(o4); p.verts.push_back(o1);

		p.verts.push_back(o1); p.verts.push_back(o3);
		p.verts.push_back(o3); p.verts.push_back(o4);
		p.verts.push_back(o4); p.verts.push_back(o1);

		rCount += 6;
		pCount += 12;
	}
	// Inside
	if (!renderNeighbors.inside) {
		r.verts.push_back(i1); r.verts.push_back(i2); r.verts.push_back(i4);
		r.verts.push_back(i1); r.verts.push_back(i3); r.verts.push_back(i4);

		p.verts.push_back(i1); p.verts.push_back(i2);
		p.verts.push_back(i2); p.verts.push_back(i4);
		p.verts.push_back(i4); p.verts.push_back(i1);

		p.verts.push_back(i1); p.verts.push_back(i3);
		p.verts.push_back(i3); p.verts.push_back(i4);
		p.verts.push_back(i4); p.verts.push_back(i1);

		rCount += 6;
		pCount += 12;
	}

	// Sides
	// Right side
	if (!renderNeighbors.right) {
		r.verts.push_back(o1); r.verts.push_back(i1); r.verts.push_back(i3);
		r.verts.push_back(o1); r.verts.push_back(o3); r.verts.push_back(i3);

		p.verts.push_back(o1); p.verts.push_back(i1);
		p.verts.push_back(i1); p.verts.push_back(i3);
		p.verts.push_back(i3); p.verts.push_back(o1);

		p.verts.push_back(o1); p.verts.push_back(o3);
		p.verts.push_back(o3); p.verts.push_back(i3);
		p.verts.push_back(i3); p.verts.push_back(o1);

		rCount += 6;
		pCount += 12;
	}

	// Left side
	if (!renderNeighbors.left) {
		r.verts.push_back(o2); r.verts.push_back(i2); r.verts.push_back(i4);
		r.verts.push_back(o2); r.verts.push_back(o4); r.verts.push_back(i4);

		p.verts.push_back(o2); p.verts.push_back(i2);
		p.verts.push_back(i2); p.verts.push_back(i4);
		p.verts.push_back(i4); p.verts.push_back(o2);

		p.verts.push_back(o2); p.verts.push_back(o4);
		p.verts.push_back(o4); p.verts.push_back(i4);
		p.verts.push_back(i4); p.verts.push_back(o2);

		rCount += 6;
		pCount += 12;
	}

	// Top and bottom
	// Top
	if (!renderNeighbors.top) {
		r.verts.push_back(o3); r.verts.push_back(i3); r.verts.push_back(i4);
		r.verts.push_back(o3); r.verts.push_back(o4); r.verts.push_back(i4);

		p.verts.push_back(o3); p.verts.push_back(i3);
		p.verts.push_back(i3); p.verts.push_back(i4);
		p.verts.push_back(i4); p.verts.push_back(o3);

		p.verts.push_back(o3); p.verts.push_back(o4);
		p.verts.push_back(o4); p.verts.push_back(i4);
		p.verts.push_back(i4); p.verts.push_back(o3);

		rCount += 6;
		pCount += 12;
	}

	// Bottom
	if (!renderNeighbors.bottom) {
		r.verts.push_back(o1); r.verts.push_back(i1); r.verts.push_back(i2);
		r.verts.push_back(o1); r.verts.push_back(o2); r.verts.push_back(i2);

		p.verts.push_back(o1); p.verts.push_back(i1);
		p.verts.push_back(i1); p.verts.push_back(i2);
		p.verts.push_back(i2); p.verts.push_back(o1);

		p.verts.push_back(o1); p.verts.push_back(o2);
		p.verts.push_back(o2); p.verts.push_back(i2);
		p.verts.push_back(i2); p.verts.push_back(o1);

		rCount += 6;
		pCount += 12;
	}

	for (int i = 0; i < rCount; i++) {
		r.colours.push_back(colour);
	}
	for (int i = 0; i < pCount; i++) {
		p.colours.push_back(glm::vec3(0.f, 0.f, 0.f));
	}
}


// Returns the SDOG cell code for the provided spherical point at a given subdivision level
//
// latRad - latitude of point, in radians
// longRad - longitude of point, in radians
// radius - radius of point (0 is centre of the sphere)
// gridRadius - radius of the grid the cell belongs to
// return - code for the cell that contains the point
std::string SdogCell::codeForPos(double latRad, double longRad, double radius, double gridRadius, unsigned int level) {

	std::string code = "";
	if (longRad < -M_PI) {
		longRad += 2.0 * M_PI;
	}
	if (longRad > M_PI) {
		longRad -= 2.0 * M_PI;
	}

	double minLat, maxLat, minLong, maxLong, minRad, maxRad;
	minLat = 0.0;
	maxLat = M_PI_2;
	minRad = 0.0;
	maxRad = gridRadius;

	// Determine which otcant the point is in
	int octCode = 0;
	if (latRad < 0.0) {
		octCode += 4;
	}
	if (longRad < 0.0) {
		octCode += 2;
	}
	if (abs(longRad) > M_PI_2) {
		octCode += 1;
		minLong = M_PI_2;
		maxLong = M_PI;
	}
	else {
		minLong = 0.0;
		maxLong = M_PI_2;
	}
	latRad = abs(latRad);
	longRad = abs(longRad);

	code += std::to_string(octCode);

	// Loop for desired number of levels and determine
	// which child point is in for each itteration
	SdogCellType curType = SdogCellType::SG;
	for (unsigned int i = 0; i < level; i++) {

		int childCode = 0;
		double midLat = 0.5 * minLat + 0.5 * maxLat;
		double midLong = 0.5 * minLong + 0.5 * maxLong;
		double midRad = 0.5 * minRad + 0.5 * maxRad;

		if (curType == SdogCellType::NG) {

			if (radius > midRad) {
				minRad = midRad;
			}
			else {
				childCode += 4;
				maxRad = midRad;
			}
			if (latRad < midLat) {
				maxLat = midLat;
			}
			else {
				childCode += 2;
				minLat = midLat;
			}
			if (longRad < midLong) {
				maxLong = midLong;
			}
			else {
				childCode += 1;
				minLong = midLong;
			}
			// type doesn't change
		}
		else if (curType == SdogCellType::LG) {

			if (radius > midRad) {
				minRad = midRad;
			}
			else {
				maxRad = midRad;
				childCode += 3;
			}
			if (latRad < midLat) {
				maxLat = midLat;
				curType = SdogCellType::NG;

				if (longRad < midLong) {
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
		else {// curType == SdogCellType::SG

			if (radius > midRad) {

				minRad = midRad;

				if (latRad < midLat) {
					maxLat = midLat;
					curType = SdogCellType::NG;

					if (longRad < midLong) {
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
					curType = SdogCellType::LG;
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


// Returns if the provided SDOG code is a valid cell or not
bool SdogCell::codeIsValid(std::string code) {

	if (code.length() < 1) {
		return false;
	}

	// Ensure octant code is valid
	char o = code[0];
	if (!(o == '0' || o == '1' || o == '2' || o == '3' || o == '4' || o == '5' || o == '6' || o == '7')) {
		return false;
	}

	// Loop through code to ensure each character is valid given the cell type of the previous element
	SdogCellType prevType = SdogCellType::SG;
	for (unsigned int i = 1; i < code.length(); i++) {

		char c = code[1];
		if (prevType == SdogCellType::NG) {
			if (!(c == '0' || c == '1' || c == '2' || c == '3' || c == '4' || c == '5' || c == '6' || c == '7')) {
				return false;
			}
		}
		else if (prevType == SdogCellType::LG) {
			if (c == '0' || c == '1' || c == '3' || c == '4') {
				prevType = SdogCellType::NG;
			}
			else if (c == '2' || c == '5') {
				prevType = SdogCellType::LG;
			}
			else {
				return false;
			}
		}
		else {// prevType == SdogCellType::SG
			if (c == '0' || c == '1') {
				prevType = SdogCellType::NG;
			}
			else if (c == '2') {
				prevType = SdogCellType::LG;
			}
			else if (c == '3') {
				prevType = SdogCellType::SG;
			}
			else {
				return false;
			}
		}
	}
	return true;
}
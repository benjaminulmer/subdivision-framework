#pragma once

//temp
#include "SphericalCell.h"
//

#include "SphericalData.h"

#include <unordered_map>
#include <string>

class SphCell {

public:
	SphCell();
	SphCell(double minLat, double maxLat, double minLong, double maxLong, double minRad, double maxRad);

	double maxRad, minRad;
	double maxLat, minLat;
	double maxLong, minLong;

	CellType type();
};

class SphGrid {

public:
	SphGrid(double radius);

	void subdivideTo(int level);
	void subdivide();
	void subdivideCell(std::string code, SphCell cell);
	void fillData(const SphericalData& data);

	std::string codeForPos(double lat, double longi, double radius, int level);
	SphCell cellFromCode(std::string code);

private:
	int maxDepth;
	double maxRadius;

	std::unordered_map<std::string, SphCell> map;
};


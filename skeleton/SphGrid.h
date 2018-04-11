#pragma once

//temp
#include "SphericalCell.h"
//

#include "SphericalData.h"
#include "Renderable.h"

#include <unordered_map>
#include <string>
#include <vector>

class SphCell {

public:
	SphCell();
	SphCell(double minLat, double maxLat, double minLong, double maxLong, double minRad, double maxRad);

	double maxRad, minRad;
	double maxLat, minLat;
	double maxLong, minLong;

	std::vector<DataPoints> dataSets;

	CellType type();
};

class SphGrid {

public:
	SphGrid(double radius);

	void subdivideTo(int level);
	void subdivide();
	void subdivideCell(std::string code, SphCell* cell);
	void fillData(const SphericalData& data);
	void createRenderable(Renderable& r, int level);
	int countLeafs();

	std::string codeForPos(double lat, double longi, double radius, int level);
	const SphCell* cellFromCode(std::string code);

private:
	int maxDepth;
	double maxRadius;

	std::unordered_map<std::string, SphCell*> map;
};


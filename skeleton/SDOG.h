#pragma once

#include "Renderable.h"
#include "SphericalData.h"

#include <string>
#include <unordered_map>
#include <vector>

enum class CellType {
	NG,
	LG,
	SG
};

struct SphCellInfo {

	double minRad, maxRad;
	double minLat, maxLat;
	double minLong, maxLong;

	CellType type;
};

class SphCell {

public:
	SphCell();
	SphCell(double minLat, double maxLat, double minLong, double maxLong, double minRad, double maxRad);

	double minRad, maxRad;
	double minLat, maxLat;
	double minLong, maxLong;

	std::vector<DataPoints> dataSets;

	CellType type() const;
};

class SphGrid {

public:
	SphGrid(double radius);

	void subdivideTo(int level);
	void subdivide();
	void fillData(const SphericalData& data);
	void createRenderable(Renderable& r, int level);

	void createRenderable(Renderable& r, std::vector<std::string>& codes);

	int size() { return map.size(); }

	std::string codeForPos(double latRad, double longRad, double radius, int level);
	bool neighbours(const std::string& code, std::vector<std::string>& out);
	const SphCell* getCell(const std::string& code);

private:
	int maxDepth;
	double maxRadius;

	std::unordered_map<std::string, SphCell*> map;

	void subdivideCell(const std::string& code, const SphCell* cell, std::vector<std::pair<std::string, SphCell*>>& toAdd);
	bool cellInfoFromCode(const std::string& code, SphCellInfo& out);
};


#pragma once

#include "SphericalCell.h"

#include <unordered_map>

class SphericalGrid {

public:
	SphericalGrid(GridInfo& info);
	virtual ~SphericalGrid();


	std::string codeForPos(double lat, double longi, double depthKM, int level);

	void subdivide();
	void createRenderable(Renderable& r, int level, DisplayMode mode);
	void fillData(const SphericalData& data);

	int countLeafs();
	static int numCells(int depth);

private:
	SphericalCell* octants[8];

	std::unordered_map<std::string, SphericalCell*> map;

	static int lgCells(int depth);
};
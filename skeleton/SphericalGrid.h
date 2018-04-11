#pragma once

#include "SphericalCell.h"

#include <unordered_map>

class SphericalGrid {

public:
	SphericalGrid(GridInfo& info);
	virtual ~SphericalGrid();

	void subdivide();
	void createRenderable(Renderable& r, int level, DisplayMode mode);
	void fillData(const SphericalData& data);

	int countLeafs();
	static int numCells(int depth);

private:
	SphericalCell* octants[8];

	static int lgCells(int depth);
};
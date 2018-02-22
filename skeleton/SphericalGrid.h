#pragma once

#include "SphericalCell.h"

class SphericalGrid {

public:
	SphericalGrid(GridInfo& info);
	virtual ~SphericalGrid();

	void subdivide();
	void createRenderable(Renderable& r, DisplayMode mode);
	void fillData(const SphericalData& data);

	int countLeafs();
	static int numCells(int depth);

private:
	SphericalCell* octants[8];
	GridInfo info;

	static int lgCells(int depth);
};
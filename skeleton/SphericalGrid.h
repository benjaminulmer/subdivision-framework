#pragma once

#include "SphericalCell.h"

class SphericalGrid {

public:
	SphericalGrid(GridInfo& info);
	virtual ~SphericalGrid();

	void subdivideTo(int level);
	void createRenderable(Renderable& r, int level, DisplayMode mode);
	void fillData(int level, const SphericalData& data);

	static int numCells(int depth);

private:
	SphericalCell* octants[8];
	GridInfo info;
	int numLevels;

	static int lgCells(int depth);
};
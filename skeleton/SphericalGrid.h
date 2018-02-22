#pragma once

#include "SphericalCell.h"

class SphericalGrid {

public:
	SphericalGrid(GridInfo& info);
	virtual ~SphericalGrid();

	void subdivideTo(int level);
	void createRenderable(Renderable& r, DisplayMode mode);
	void fillData(const SphericalData& data);

	static int numCells(int depth);

private:
	SphericalCell* octants[8];
	GridInfo info;
	int numLevels;

	static int lgCells(int depth);
};
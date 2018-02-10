#pragma once

#include "SphericalCell.h"

class SphericalGrid {

public:
	SphericalGrid(GridInfo& info);
	virtual ~SphericalGrid();

	void subdivideTo(int level);
	void createRenderable(Renderable& r, int level, DisplayMode mode);
	void getVolumes(std::vector<float>& volumes, int level);
	int getNumCells();

	void fillData(int level, const SphericalData& data);

private:
	SphericalCell* octants[8];
	GridInfo info;
	int numLevels;
};
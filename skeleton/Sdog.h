#pragma once

#include "Geometry.h"
#include "RenderEngine.h"

enum class GridType {
	NG,
	LG,
	SG
};

class SdogGrid {
	
public:
	SdogGrid(GridType type, double maxRadius, double minRadius, 
	         double maxLat, double minLat, double maxLong, double minLong);

	void subdivideTo(int level);
	void createRenderable(Renderable& r, int level);

	void getVolumes(std::vector<float>& volumes, int level);

private:
	GridType type;

	double maxRadius, minRadius;
	double maxLat, minLat;
	double maxLong, minLong;

	SdogGrid* children[8];
	int numChildren;
	bool leaf;

	void subdivide();
	void fillRenderable(Renderable& r);
};

class Sdog {

public:
	Sdog(double radius);

	void subdivideTo(int level);
	void createRenderable(Renderable& r, int level, bool wholeSphere = false);
	
	void getVolumes(std::vector<float>& volumes, int level, bool wholeSphere = false);

private:
	double radius;
	SdogGrid* octants[8];

	int numLevels;
};


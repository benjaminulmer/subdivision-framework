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

	void subdivide(int level);
	void createRenderable(Renderable& r);
	void draw(std::vector<Renderable>& objects, int level);

private:
	GridType type;

	double maxRadius, minRadius;
	double maxLat, minLat;
	double maxLong, minLong;

	SdogGrid* children[8];
	int numChildren;
	bool leaf;
};

class Sdog {

public:
	Sdog(double radius);

	void subdivideTo(int level);
	void draw(std::vector<Renderable>& objects, int level);

private:
	double radius;
	SdogGrid* octants[8];

	int numLevels;
};


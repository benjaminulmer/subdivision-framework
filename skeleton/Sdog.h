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

	void subdivide();
	void createRenderable(Renderable& r);

	SdogGrid* getChild(int i) {
		return children[i];
	}

private:
	GridType type;

	double maxRadius, minRadius;
	double maxLat, minLat;
	double maxLong, minLong;

	SdogGrid* children[8];
};

class Sdog {

public:
	Sdog(double radius);

	void subdivideTo(int level);
	void draw(std::vector<Renderable>& objects);

private:
	double radius;
	SdogGrid* octants[8];

	int numLevels;
};


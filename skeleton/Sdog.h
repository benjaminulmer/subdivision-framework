#pragma once

#include "Geometry.h"

enum class GridType {
	NG,
	LG,
	SG
};

class SdogGrid {
	
public:
	SdogGrid(double maxRadius, double minRadius, double maxLat,
	         double minLat, double maxLong, double minLong);

	void createRenderable(Renderable& r);

private:
	GridType type;

	double maxRadius, minRadius;
	double maxLat, minLat;
	double maxLong, minLong;
};

class Sdog {

public:
	Sdog();

private:
	double radius;
};


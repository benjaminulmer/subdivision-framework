#pragma once

#include "Geometry.h"

enum class GridType {
	NG,
	LG,
	SG
};

struct SdogGrid {
	GridType type;

	double maxRadius, minRadius;
	double maxLat, minLat;
	double maxLong, minLong;

	void createRenderable(Renderable& r);
};

class Sdog {

public:
	Sdog();

private:
	double radius;
};


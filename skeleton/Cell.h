#pragma once

#include "d128Vec2.h"
#include "d128Vec3.h"
#include "double128/double128.h"

class Cell {
public:
	Cell();
	Cell(unsigned int index, d128Vec3 v1, d128Vec3 v2, d128Vec3 v3, cgiDouble128NS::double128 populationDensity);
	~Cell();

private:
	unsigned int index;
	d128Vec3 v1;
	d128Vec3 v2;
	d128Vec3 v3;
	d128Vec2 uv1;
	d128Vec2 uv2;
	d128Vec2 uv3;
	cgiDouble128NS::double128 populationDensity;
};


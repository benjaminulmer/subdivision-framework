#pragma once

#include <glm/glm.hpp>

#include <iostream>
#include <fstream>
#include <cstring>

#include <vector>
#include <string>

#include "Renderable.h"


class ContentReadWrite {

public:
	static bool loadOBJ(const char* path, Renderable& r);

	static std::vector<double>* loadPopulationData(const char* path, unsigned int* ncols, unsigned int* nrows, double* xllcorner, double* yllcorner, double* cellsize);
};


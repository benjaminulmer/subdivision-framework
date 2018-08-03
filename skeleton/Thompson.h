#pragma once

#include <stdlib.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

#include "Geometry.h"
#include "Renderable.h"

class Thompson {

public:
	Thompson(int numPoints, int numIterations);
	~Thompson();

	void fillRenderable(Renderable& r);

	void outputCSV();

	double getCoulombEnergy(std::vector<glm::vec3> pts);
	void getForces();

	void iterate();

	double frand(void) {return ((rand() - (RAND_MAX / 2)) / (RAND_MAX / 2.)); }

private:
	int numPoints;
	int numIterations;

	double step = 0.000001;
	double minimumStep = 1.e-100;

	std::vector<glm::vec3> points;
	std::vector<glm::vec3> forces;
};


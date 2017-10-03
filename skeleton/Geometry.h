#pragma once

#include <glm/glm.hpp>
#define _USE_MATH_DEFINES
#include <math.h>

#include "Renderable.h"

class Geometry {

public:
	static void createArc(glm::vec3 p1, glm::vec3 p2, glm::vec3 centre, Renderable& r);
	static void createLine(glm::vec3 p1, glm::vec3 p2, Renderable& r);
};


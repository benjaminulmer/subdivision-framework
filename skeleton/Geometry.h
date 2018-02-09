#pragma once

#include <glm/glm.hpp>

#include "Renderable.h"

class Geometry {

public:
	static glm::vec3 geomSlerp(glm::vec3 v1, glm::vec3 v2, float t);
	
	static void createArcR(glm::vec3 p1, glm::vec3 p2, glm::vec3 centre, Renderable& r);
	static void createLineR(glm::vec3 p1, glm::vec3 p2, Renderable& r);
};


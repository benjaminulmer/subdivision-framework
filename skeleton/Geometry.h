#pragma once

#include <poly2tri/poly2tri.h>
#include <glm/glm.hpp>

#include "Renderable.h"

class Geometry {

public:
	static glm::vec3 geomSlerp(const glm::vec3& v1, const glm::vec3& v2, float t);
	
	static void createArcR(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& centre, Renderable& r);
	static void createLineR(const glm::vec3& p1, const glm::vec3& p2, Renderable& r);

	static void createWallR(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& maxp1, const glm::vec3& maxp2, const glm::vec3& centre, Renderable& r);

	static std::vector<glm::vec2> triangulatePolygon(const std::vector<glm::vec2> coords);
};


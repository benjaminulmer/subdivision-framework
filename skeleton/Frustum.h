#pragma once

#include <glm/glm.hpp>

#include "Camera.h"

class Frustum {

public:
	Frustum() = default;
	Frustum(const Camera& camera, float fovY, float aspectRatio, float near, float far);

	bool inside(const glm::vec3& point);

private:
	glm::vec3 leftP, rightP, topP, bottomP, nearP, farP;
	glm::vec3 leftN, rightN, topN, bottomN, nearN, farN;

};


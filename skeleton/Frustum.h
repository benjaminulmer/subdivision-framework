#pragma once

#include <glm/glm.hpp>

class Camera;
class SphericalCell;

class Frustum {

public:
	Frustum() = default;
	Frustum(const Camera& camera, float fovY, float aspectRatio, float near, float far);

	bool inside(const glm::vec3& point) const;
	bool inside(const SphericalCell& cell, float scale) const;

private:
	glm::vec3 leftP, rightP, topP, bottomP, nearP, farP;
	glm::vec3 leftN, rightN, topN, bottomN, nearN, farN;

	bool allPointsOutsidePlane(const glm::vec3& o1, const glm::vec3& o2, const glm::vec3& o3, const glm::vec3& o4, 
						       const glm::vec3& i1, const glm::vec3& i2, const glm::vec3& i3, const glm::vec3& i4,
						       const glm::vec3& pPoint, const glm::vec3& pNorm) const;

};


#include "Frustum.h"

#include <glm/gtx/rotate_vector.hpp>

// Creates view frustum in world space from camera and projection info
Frustum::Frustum(const Camera& camera, float fovY, float aspectRatio, float near, float far) {

	float fovX = fovY * aspectRatio;

	glm::vec3 camPos = camera.getPosition();
	glm::vec3 lookDir = camera.getLookDir();
	glm::vec3 up = camera.getUp();
	glm::vec3 biNorm = glm::cross(lookDir, up);

	leftP = rightP = topP = bottomP = camPos;

	nearP = camPos + near * lookDir;
	nearN = lookDir;

	farP = camPos + far * lookDir;
	farN = -lookDir;

	leftN = glm::rotate(biNorm, fovX / 2.f, up);

	rightN = glm::rotate(-biNorm, -fovX / 2.f, up);

	topN = glm::rotate(-up, fovY / 2.f, biNorm);

	bottomN = glm::rotate(up, -fovY / 2.f, biNorm);
}

// Returns if point is inside frustum
bool Frustum::inside(const glm::vec3& point) {
	return (glm::dot(point - leftP, leftN) > 0) && (glm::dot(point - rightP, rightN) > 0) &&
	       (glm::dot(point - topP, topN) > 0) && (glm::dot(point - bottomP, bottomN) > 0) && 
	       (glm::dot(point - nearP, nearN) > 0) && (glm::dot(point - farP, farN) > 0);
}

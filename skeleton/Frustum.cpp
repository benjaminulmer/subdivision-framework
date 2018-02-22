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

	farP = glm::vec3(0.f, 0.f, 0.f) + 7.f * -lookDir;
	farN = -lookDir;

	leftN = glm::rotate(biNorm, fovX / 2.f, up);

	rightN = glm::rotate(-biNorm, -fovX / 2.f, up);

	topN = glm::rotate(-up, fovY / 2.f, biNorm);

	bottomN = glm::rotate(up, -fovY / 2.f, biNorm);
}

// Returns if all points are outside of given plane
inline bool Frustum::allPointsOutsidePlane(const glm::vec3& o1, const glm::vec3& o2, const glm::vec3& o3, const glm::vec3& o4,
							               const glm::vec3& i1, const glm::vec3& i2, const glm::vec3& i3, const glm::vec3& i4,
							               const glm::vec3& pPoint, const glm::vec3& pNorm) {

	return (glm::dot(o1 - pPoint, pNorm) < 0 && glm::dot(o2 - pPoint, pNorm) < 0 && 
			glm::dot(o3 - pPoint, pNorm) < 0 && glm::dot(o4 - pPoint, pNorm) < 0 && 
			glm::dot(i1 - pPoint, pNorm) < 0 && glm::dot(i2 - pPoint, pNorm) < 0 &&
			glm::dot(i3 - pPoint, pNorm) < 0 && glm::dot(i4 - pPoint, pNorm) < 0);
}

// Returns if point is inside frustum
bool Frustum::inside(const glm::vec3& point) {
	return (glm::dot(point - leftP, leftN) > 0) && (glm::dot(point - rightP, rightN) > 0) &&
	       (glm::dot(point - topP, topN) > 0) && (glm::dot(point - bottomP, bottomN) > 0) && 
	       (glm::dot(point - nearP, nearN) > 0) && (glm::dot(point - farP, farN) > 0);
}

// Returns if cell is inside or overlaps with frustum
bool Frustum::inside(const SphericalCell& cell) {
	
	glm::vec3 o1, o2, o3, o4, i1, i2, i3, i4;
	cell.cornerPoints(o1, o2, o3, o4, i1, i2, i3, i4);

	// Test to see if all point are on wrong side of same plane
	if (allPointsOutsidePlane(o1, o2, o3, o4, i1, i2, i3, i4, leftP, leftN) || 
			allPointsOutsidePlane(o1, o2, o3, o4, i1, i2, i3, i4, rightP, rightN) ||
			allPointsOutsidePlane(o1, o2, o3, o4, i1, i2, i3, i4, topP, topN) ||
			allPointsOutsidePlane(o1, o2, o3, o4, i1, i2, i3, i4, bottomP, bottomN) ||
			allPointsOutsidePlane(o1, o2, o3, o4, i1, i2, i3, i4, nearP, nearN) ||
			allPointsOutsidePlane(o1, o2, o3, o4, i1, i2, i3, i4, farP, farN)) {

		return false;
	}
	else {
		return true;
	}
}
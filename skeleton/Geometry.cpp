#include "Geometry.h"

#include <iostream>

// Creates a renderable  for an arc defined by p1, p2, and centre points
// Assumes p1 - centre and p2 - centre are same length ie circular arcs
void Geometry::createArc(glm::vec3 p1, glm::vec3 p2, glm::vec3 centre, Renderable& r) {
	glm::vec3 v1 = p1 - centre;
	glm::vec3 v2 = p2 - centre;

	float dot = glm::dot(glm::normalize(v1), glm::normalize(v2));
	float theta = acos(dot);
	int angleDeg = (int) (theta * 180.f / M_PI);

	for (int i = 0; i < angleDeg; i++) {
		float t = (float)i / angleDeg;
		
		glm::vec3 term1 = (sin((1.f - t) * theta) / sin(theta)) * v1;
		glm::vec3 term2 = (sin(t * theta) / sin(theta)) * v2;
		glm::vec3 result = term1 + term2;

		std::cout << result.x << ", " << result.y << ", " << result.z << std::endl;
		
		r.verts.push_back(result);
		r.normals.push_back(glm::normalize(result));
	}
	r.drawMode = GL_LINE_STRIP;
}

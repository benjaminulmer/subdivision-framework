#include "Geometry.h"

// Geometric slerp between two vectors
glm::vec3 Geometry::geomSlerp(glm::vec3 v1, glm::vec3 v2, float t) {
	float theta = acos(glm::dot(glm::normalize(v1), glm::normalize(v2)));

	glm::vec3 term1 = (sin((1.f - t) * theta) / sin(theta)) * v1;
	glm::vec3 term2 = (sin(t * theta) / sin(theta)) * v2;
	return term1 + term2;
}

// Creates a renderable for an arc defined by p1, p2, and centre points
void Geometry::createArcR(glm::vec3 p1, glm::vec3 p2, glm::vec3 centre, Renderable& r) {
	glm::vec3 v1 = p1 - centre;
	glm::vec3 v2 = p2 - centre;

	// Points on centre, cannot create arc (and will crash)
	if (glm::length2(v1) < 0.0001 || glm::length2(v2) < 0.0001) {
		return;
	}

	// Compute angle between vectors
	float theta = acos(glm::dot(glm::normalize(v1), glm::normalize(v2)));
	int angleDeg = (int) (theta * 180.f / M_PI);

	// Points are very close on arc, just draw line between them
	if (angleDeg == 0) {
		r.verts.push_back(p1);
		r.verts.push_back(p2);
		r.colours.push_back(glm::vec3(0.f, 1.f, 0.f));
		r.colours.push_back(glm::vec3(0.f, 1.f, 0.f));
	}

	// #num line segments ~= angle of arc in degrees / 4
	angleDeg /= 6;
	angleDeg++;
	for (int i = 0; i <= angleDeg; i++) {
		float t = (float)i / angleDeg;
		
		glm::vec3 result = geomSlerp(v1, v2, t);

		r.verts.push_back(centre + result);
		r.colours.push_back(glm::vec3(0.f, 1.f, 0.f));
		if (i != 0 && i != angleDeg) {
			r.verts.push_back(centre + result);
			r.colours.push_back(glm::vec3(0.f, 1.f, 0.f));
		}
	}
	r.drawMode = GL_LINES;
}

// Creates a renderable for a line defined by p1 and p2
void Geometry::createLineR(glm::vec3 p1, glm::vec3 p2, Renderable & r) {
	r.verts.push_back(p1);
	r.verts.push_back(p2);
	r.colours.push_back(glm::vec3(0.f, 1.f, 0.f));
	r.colours.push_back(glm::vec3(0.f, 1.f, 0.f));
	r.drawMode = GL_LINES;
}

// Creates vector of points on an arc defined by p1, p2, and centre points
std::vector<glm::vec3> Geometry::createArc(glm::vec3 p1, glm::vec3 p2, glm::vec3 centre) {
	std::vector<glm::vec3> verts;

	glm::vec3 v1 = p1 - centre;
	glm::vec3 v2 = p2 - centre;

	// Points on centre, cannot create arc (and will crash)
	if (glm::length2(v1) < 0.0001 || glm::length2(v2) < 0.0001) {
		return verts;
	}

	// Compute angle between vectors
	float theta = acos(glm::dot(glm::normalize(v1), glm::normalize(v2)));
	int angleDeg = (int)(theta * 180.f / M_PI);

	// Points are very close on arc, just draw line between them
	if (angleDeg == 0) {
		verts.push_back(p1);
		verts.push_back(p2);
	}

	// #num line segments ~= angle of arc in degrees / 4
	angleDeg /= 6;
	angleDeg++;
	for (int i = 0; i <= angleDeg; i++) {
		float t = (float)i / angleDeg;
		glm::vec3 result = geomSlerp(v1, v2, t);

		verts.push_back(centre + result);
	}
	return verts;
}

void Geometry::createPatchR(std::vector<glm::vec3>& u1, std::vector<glm::vec3>& u2, std::vector<glm::vec3>& v1, std::vector<glm::vec3>& v2, Renderable & r) {
	
	// Cannot make patch
	if (u1.size() != u2.size() || v1.size() != v2.size()) {
		return;
	}



}
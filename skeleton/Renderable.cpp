#include "Renderable.h"

LineVerts::~LineVerts() {
	// Remove data from GPU
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteVertexArrays(1, &vao);
}

Renderable::~Renderable() {
	// Remove data from GPU
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &normalBuffer);
	glDeleteBuffers(1, &uvBuffer);
	glDeleteBuffers(1, &indexBuffer);
	glDeleteVertexArrays(1, &vao);
}

// Returns dimension of objects bounding box
glm::vec3 Renderable::getDimensions() {
	if (!boundingBoxComputed) {
		computeBoundingBox();
		boundingBoxComputed = true;
	}
	return dimensions;
}

// Returns centre of objects bounding box
glm::vec3 Renderable::getPosition() {
	if (!boundingBoxComputed) {
		computeBoundingBox();
		boundingBoxComputed = true;
	}
	return position;
}

// Computed objects bounding box stored as dimensions and centre of box
void Renderable::computeBoundingBox() {
	float minX, maxX;
	float minY, maxY;
	float minZ, maxZ;
	minX = maxX = verts.at(0).x;
	minY = maxY = verts.at(0).y;
	minZ = maxZ = verts.at(0).z;

	// Loop over all verts to find max and mins
	for (glm::vec3 v : verts) {
		minX = glm::min(minX, v.x);
		maxX = glm::max(maxX, v.x);
		minY = glm::min(minY, v.y);
		maxY = glm::max(maxY, v.y);
		minZ = glm::min(minZ, v.z);
		maxZ = glm::max(maxZ, v.z);
	}
	dimensions = glm::vec3(glm::abs(maxX - minX), glm::abs(maxY - minY), glm::abs(maxZ - minZ));
	position = glm::vec3(0.5f * (maxX + minX), 0.5f * (maxY + minY), 0.5f * (maxZ + minZ));
}

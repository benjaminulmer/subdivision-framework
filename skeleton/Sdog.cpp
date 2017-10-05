#include "Sdog.h"

// Creats a renderable of the boundaries of the Sdog grid
// Completely generalized for all 3 grid types
void SdogGrid::createRenderable(Renderable & r) {
	glm::vec3 origin(0.f, 0.f, 0.f);

	// Outer points
	glm::vec3 o1 = glm::vec3(cos(minLong)*cos(minLat), sin(minLat), sin(minLong)*cos(minLat)) * (float)maxRadius;
	glm::vec3 o2 = glm::vec3(cos(maxLong)*cos(minLat), sin(minLat), sin(maxLong)*cos(minLat)) * (float)maxRadius;
	glm::vec3 o3 = glm::vec3(cos(minLong)*cos(maxLat), sin(maxLat), sin(minLong)*cos(maxLat)) * (float)maxRadius;
	glm::vec3 o4 = glm::vec3(cos(maxLong)*cos(maxLat), sin(maxLat), sin(maxLong)*cos(maxLat)) * (float)maxRadius;

	// Inner points
	glm::vec3 i1 = glm::vec3(cos(minLong)*cos(minLat), sin(minLat), sin(minLong)*cos(minLat)) * (float)minRadius;
	glm::vec3 i2 = glm::vec3(cos(maxLong)*cos(minLat), sin(minLat), sin(maxLong)*cos(minLat)) * (float)minRadius;
	glm::vec3 i3 = glm::vec3(cos(minLong)*cos(maxLat), sin(maxLat), sin(minLong)*cos(maxLat)) * (float)minRadius;
	glm::vec3 i4 = glm::vec3(cos(maxLong)*cos(maxLat), sin(maxLat), sin(maxLong)*cos(maxLat)) * (float)minRadius;

	// Straight lines connect each inner point to coresponding outer point
	Geometry::createLine(i1, o1, r);
	Geometry::createLine(i2, o2, r);
	Geometry::createLine(i3, o3, r);
	Geometry::createLine(i4, o4, r);

	// Great circle arcs connect points on same longtitude line
	Geometry::createArc(o1, o3, origin, r);
	Geometry::createArc(o2, o4, origin, r);
	Geometry::createArc(i1, i3, origin, r);
	Geometry::createArc(i2, i4, origin, r);

	// Small circle arcs connect points on same latitude line
	Geometry::createArc(o1, o2, glm::vec3(0.f, sin(minLat), 0.f), r);
	Geometry::createArc(o3, o4, glm::vec3(0.f, sin(maxLat), 0.f), r);
	Geometry::createArc(i1, i2, glm::vec3(0.f, sin(minLat), 0.f), r);
	Geometry::createArc(i3, i4, glm::vec3(0.f, sin(maxLat), 0.f), r);
}

Sdog::Sdog() {

}


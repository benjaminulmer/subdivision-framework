#pragma once

#include <glm/glm.hpp>

class SphCoord {

public:
	SphCoord();
	SphCoord(double latitude, double longitude, bool radians = true);
	SphCoord(double latitude, double longitude, double radius, bool radians = true);
	SphCoord(const glm::vec3& point);

	glm::vec3 toCartesian();
	double latitudeDeg();
	double longitudeDeg();

	double latitude;  // in radians
	double longitude; // in radians
	double radius;    // in world units

	// Static members
	static bool greatCircleArc2Intersect(const SphCoord& a0, const SphCoord& a1, const SphCoord& b0, const SphCoord& b1, SphCoord& intersection);
	static bool greatCircleArcLatIntersect(const SphCoord& a0, const SphCoord& a1, double latRad, SphCoord& intersection);
};


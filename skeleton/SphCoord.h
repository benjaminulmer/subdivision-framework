#pragma once

#include <glm/glm.hpp>

class SphCoord {

public:
	SphCoord();
	SphCoord(double latitude, double longitude, bool radians = true);
	SphCoord(const glm::vec3& point);
	SphCoord(float x, float y, float z);

	double arcLength(const SphCoord& other) const;
	glm::vec3 toCartesian(double radius) const;
	double latitudeDeg() const;
	double longitudeDeg() const;

	double latitude;  // in radians
	double longitude; // in radians
	double radius;

	// Static members
	static bool greatCircleArc2Intersect(const SphCoord& a0, const SphCoord& a1, const SphCoord& b0, const SphCoord& b1, SphCoord& intersection);
	static bool greatCircleArcLatIntersect(const SphCoord& a0, const SphCoord& a1, double latRad, double minLongRad, double maxLongRad, SphCoord& intersection);
};


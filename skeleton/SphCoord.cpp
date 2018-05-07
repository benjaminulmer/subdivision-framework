#define _USE_MATH_DEFINES
#include "SphCoord.h"

#include <glm/gtc/epsilon.hpp>
#include <glm/gtx/intersect.hpp>

#include <cmath>

// Zero all fields
SphCoord::SphCoord() : latitude(0.0), longitude(0.0) {}


// Construct using provided lat long and radius of 1. If radians is false values are treated as degrees and converted to radians
SphCoord::SphCoord(double latitude, double longitude, bool radians) {

	if (radians) {
		this->longitude = longitude;
		this->latitude = latitude;
	}
	else {
		this->longitude = longitude * M_PI / 180.0;
		this->latitude = latitude * M_PI / 180.0;
	}
}


// Constructs from a cartesian point. Assumes sphere is at origin
SphCoord::SphCoord(const glm::vec3& point) {
	float radius = glm::length(point);
	latitude = asin(point.y / radius);
	longitude = atan2(point.x, point.z);
}


// Returns the arc length between this and another spherical point
//
// other - other spherical point to calculate arc length between
double SphCoord::arcLength(const SphCoord& other) const {

	glm::vec3 cart1 = this->toCartesian(1.0);
	glm::vec3 cart2 = other.toCartesian(1.0);
	return acos(glm::dot(cart1, cart2));
}


// Converts spherical coordinates to cartesian with a given spherical radius. Assumes sphere is at origin
//
// radius - radius of sphere
glm::vec3 SphCoord::toCartesian(double radius) const {
	return glm::vec3( sin(longitude) * cos(latitude), sin(latitude), cos(longitude) * cos(latitude) ) * (float)radius;
}


// Returns latitude in degrees
double SphCoord::latitudeDeg() const {
	return latitude * 180.0 / M_PI;
}


// Returns longitude in degrees
double SphCoord::longitudeDeg() const {
	return longitude * 180.0 / M_PI;
}


// Returns if two provided great circle arcs intersect. If they do, the result is stored in intersection
//
// a0 - start of first arc
// a1 - end of first arc
// b0 - start of second arc
// b1 - end of second arc
// intersection - output for intersection point
bool SphCoord::greatCircleArc2Intersect(const SphCoord& a0, const SphCoord& a1, const SphCoord& b0, const SphCoord& b1, SphCoord& intersection) {

	// Calculate planes for the two arcs
	glm::vec3 planeA = glm::cross(a0.toCartesian(1.0), a1.toCartesian(1.0));
	glm::vec3 planeB = glm::cross(b0.toCartesian(1.0), b1.toCartesian(1.0));

	// If planes are equal, treat as no intersection
	glm::bvec3 equal = glm::epsilonEqual(planeA, planeB, 0.0001f);
	if (equal.x && equal.y && equal.z) {
		return false;
	}

	// Planes are not the same, get the line of intersection between them
	glm::vec3 lineDir = glm::cross(planeA, planeB);

	// Find the candidate intersection points by intersecting the line with the sphere
	glm::vec3 inter1, inter2, norm1, norm2;
	if (glm::intersectLineSphere(glm::vec3(), lineDir, glm::vec3(), 1.0, inter1, norm1, inter2, norm2)) {

		double arcA = a0.arcLength(a1);
		double arcB = b0.arcLength(b1);

		// Test if point 1 is on both arcs
		double distA0 = a0.arcLength(SphCoord(inter1));
		double distA1 = a1.arcLength(SphCoord(inter1));
		double distB0 = b0.arcLength(SphCoord(inter1));
		double distB1 = b1.arcLength(SphCoord(inter1));

		double eps = 0.0001;
		if (abs(arcA - distA0 - distA1) < eps && abs(arcB - distB0 - distB1) < eps) {
			intersection = SphCoord(inter1);
			return true;
		}

		// Test if point 2 is on both arcs
		distA0 = a0.arcLength(SphCoord(inter2));
		distA1 = a1.arcLength(SphCoord(inter2));
		distB0 = b0.arcLength(SphCoord(inter2));
		distB1 = b1.arcLength(SphCoord(inter2));

		if (abs(arcA - distA0 - distA1) < eps && abs(arcB - distB0 - distB1) < eps) {
			intersection = SphCoord(inter2);
			return true;
		}
	}
	else {
		// Should never happen
		return false;
	}
}

// Returns if a great circle arc and line of latitude intersect. If they do, the result is stored in intersection
// 
// a0 - start of arc
// a1 - end of arc
// latRad - line of latitude in radians
// minLongRad - minimum longitude for latitude line in radians
// maxLongRad - maximum longitude for latitude line in radians
// intersection - output for intersection point
bool SphCoord::greatCircleArcLatIntersect(const SphCoord& a0, const SphCoord& a1, double latRad, double minLongRad, double maxLongRad, SphCoord& intersection) {


}
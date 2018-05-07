#define _USE_MATH_DEFINES
#include "SphCoord.h"

#include <cmath>

// Zero all fields
SphCoord::SphCoord() : latitude(0.0), longitude(0.0), radius(0.0) {}


// Construct using provided lat long and radius of 1. If radians is false values are treated as degrees and converted to radians
SphCoord::SphCoord(double latitude, double longitude, bool radians) : SphCoord(latitude, longitude, 1.0, radians) {}


// Construct using provided values. If radians is false values are treated as degrees and converted to radians
SphCoord::SphCoord(double latitude, double longitude, double radius, bool radians) : radius(radius) {

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
	radius = glm::length(point);
	latitude = asin(point.y / radius);

	if (point.z > 0.0) {
		longitude = atan(point.x / point.z);
	}
	else {
		if (point.x > 0.0) {
			longitude = atan(point.x / point.z) + M_PI;
		}
		else {
			longitude = atan(point.x / point.z) - M_PI;
		}
	}
}


// Converts spherical coordinated to cartesian. Assumes sphere is at origin
glm::vec3 SphCoord::toCartesian() {
	return glm::vec3( sin(longitude) * cos(latitude), sin(latitude), cos(longitude) * cos(latitude) ) * (float)radius;
}


// Returns latitude in degrees
double SphCoord::latitudeDeg() {
	return latitude * 180.0 / M_PI;
}


// Returns longitude in degrees
double SphCoord::longitudeDeg() {
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

}

// Returns if a great circle arc and line of latitude intersect. If they do, the result is stored in intersection
// 
// a0 - start of arc
// a1 - end of arc
// latitude - line of latitude in radians
// intersection - output for intersection point
bool SphCoord::greatCircleArcLatIntersect(const SphCoord& a0, const SphCoord& a1, double latRad, SphCoord& intersection) {

}
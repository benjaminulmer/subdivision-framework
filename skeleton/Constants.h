#pragma once
#define RADIUS_EARTH_KM 6371.0
#define RADIUS_EARTH_VIEW 10.0
#define RADIAL_SCALE_FACTOR 1.0//12.0

inline double altToAbs(double alt) {
	return RADIUS_EARTH_KM + alt * RADIAL_SCALE_FACTOR;
}
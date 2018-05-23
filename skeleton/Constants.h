#pragma once
#define RADIUS_EARTH_KM 6371.0f
#define RADIUS_EARTH_VIEW 10.0f
#define RADIAL_SCALE_FACTOR 12.f

inline double altToAbs(double alt) {
	return RADIUS_EARTH_KM + alt * RADIAL_SCALE_FACTOR;
}
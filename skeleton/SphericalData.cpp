#include "SphericalData.h"

// Default constructor
SphericalData::SphericalData() {}

// Fake volume "data"
SphericalData::SphericalData(std::vector<float> volumes) {
	for (float v : volumes) {
		data.push_back(SphericalDatum(v));
	}
	calculateStats();
}

// Calculates statistics about data (max, min, avg)
void SphericalData::calculateStats() {
	max = -FLT_MAX;
	min = FLT_MAX;
	avg = 0.f;

	for (SphericalDatum d : data) {

		double v = d.datum;

		avg += v;
		max = (v > max) ? v : max;
		min = (v < min) ? v : min;
	}
	avg /= data.size();
}
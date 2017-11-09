#include "SphericalData.h"

#include <iostream>

// Default constructor
SphericalData::SphericalData() {

	std::mt19937 gen;
	gen.seed(5);
	std::normal_distribution<double> dist1(0.8, .4);
	std::normal_distribution<double> dist2(2, .1);

	for (int i = 0; i < 1000; i++) {
		double latitude = dist1(gen);
		double longitude = -dist1(gen);
		double raidus = dist2(gen);
		double datum = dist1(gen);

		data.push_back(SphericalDatum(latitude, longitude, raidus, datum));
	}
	calculateStats();
}

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
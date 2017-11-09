#include "SphericalData.h"

#include "Geometry.h"

// Default constructor
SphericalData::SphericalData() {}

// Dummy data
SphericalData::SphericalData(int dummy) {

	std::mt19937 gen;
	gen.seed(5);
	std::uniform_real_distribution<double> dist1(-M_PI / 2, M_PI / 2);
	std::normal_distribution<double> dist2(2, .1);
	std::uniform_real_distribution<double> dist3(-0.3, 0.3);

	for (int i = 0; i < 5000; i++) {
		double latitude = dist1(gen);
		double longitude = -2.0 * dist1(gen);
		double raidus = dist2(gen);
		double datum = (M_PI / 2 - latitude) + dist3(gen);

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
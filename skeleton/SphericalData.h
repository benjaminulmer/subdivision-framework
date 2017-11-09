#pragma once

#include <vector>
#include <random>

struct SphericalDatum {

	SphericalDatum(double latitude, double longitude, double radius, float datum) :
		latitude(latitude), longitude(longitude), radius(radius), datum(datum) {}

	// Temp
	SphericalDatum(float datum) :
		latitude(0.0), longitude(0.0), radius(0.0), datum(datum) {
	}

	double latitude, longitude, radius;
	float datum;
};

class SphericalData {

public:
	SphericalData();
	SphericalData(int dummy);
	SphericalData(std::vector<float> volumes);

	const std::vector<SphericalDatum>& getData() const {
		return data;
	}

	float getMin() const {
		return min;
	}
	float getMax() const {
		return max;
	}
	float getAvg() const {
		return avg;
	}

private:
	std::vector<SphericalDatum> data;
	float min, max, avg;

	void calculateStats();
};


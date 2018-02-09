#pragma once

#include <glm/glm.hpp>
#include <rapidjson/document.h>

#include <vector>

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


struct DataSetInfo {
	int id;
	std::vector<glm::vec3> binColors;
	float mean, max, min;
};

struct DataSetPoints {
	DataSetPoints(const DataSetInfo& info) : info(info) {}

	std::vector<SphericalDatum> points;
	const DataSetInfo& info;

	float mean() {
		float mean = 0.f;
		for (SphericalDatum d : points) {
			mean += d.datum;
		}
		return mean / points.size();
	}
};

class SphericalData {

public:
	SphericalData() = default;
	SphericalData(rapidjson::Document& d);
	SphericalData(std::vector<float> volumes);

	const std::vector<SphericalDatum>& getData() const {
		return data;
	}

	int size() {
		return (int) data.size();
	}

	void addDatum(const SphericalDatum& d) {
		data.push_back(d);
	}

	float getMin() const {
		return info.min;
	}
	float getMax() const {
		return info.max;
	}
	float getMean() const {
		return info.mean;
	}

	const DataSetInfo& getInfo() {
		return info;
	}

	void calculateStats();

private:
	std::vector<SphericalDatum> data;
	DataSetInfo info;
};


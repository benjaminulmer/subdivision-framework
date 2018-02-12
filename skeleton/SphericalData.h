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

struct DataPoints {
	DataPoints(const DataSetInfo& info) : info(info) {}

	std::vector<SphericalDatum> data;
	const DataSetInfo& info;

	float mean() {
		float mean = 0.f;
		for (SphericalDatum d : data) {
			mean += d.datum;
		}
		return mean / data.size();
	}
};

class SphericalData {

public:
	SphericalData() = default;
	SphericalData(rapidjson::Document& d, int num);
	SphericalData(std::vector<float> volumes);

	const std::vector<SphericalDatum>& getData() const {
		return data;
	}

	const DataSetInfo& getInfo() const {
		return info;
	}

	void calculateStats();

private:
	static int count;

	std::vector<SphericalDatum> data;
	DataSetInfo info;
};


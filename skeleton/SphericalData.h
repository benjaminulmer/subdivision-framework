#pragma once

#include <glm/glm.hpp>
#include <rapidjson/document.h>

#include <vector>

struct SphericalDatum {

	SphericalDatum(double latitude, double longitude, double radius, float datum) :
		latitude(latitude), longitude(longitude), radius(radius), datum(datum) {}

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
	SphericalData(rapidjson::Document& d, rapidjson::Document& metaD);

	const std::vector<SphericalDatum>& getData() const {
		return data;
	}

	const DataSetInfo& getInfo() const {
		return info;
	}

	void linSub() {
		std::vector<SphericalDatum> nData(data.size() * 2 - 1, SphericalDatum(0.0, 0.0, 0.0, 0.f));

		for (int i = 0; i < nData.size(); i++) {

			if (i % 2 == 0) {
				nData[i] = data[i/2];
			}
			else {
				double lat = data[i/2].latitude * 0.5 + data[i/2+1].latitude * 0.5;

				double l1 = data[i/2].longitude;
				double l2 = data[i/2+1].longitude;
				if (l1 < 0) l1 += 360.0;
				if (l2 < 0) l2 += 360.0;

				double longg = 0.5 * l1 + 0.5 * l2;

				if (longg > 180.0) longg -=  360.0;

				double rad = data[i/2].radius * 0.5 + data[i/2+1].radius * 0.5;
				float datum = data[i/2].datum * 0.5f + data[i/2+1].datum * 0.5f;
				nData[i] = SphericalDatum(lat, longg, rad, datum);
			}
		}
		data = nData;
	}

	void calculateStats();

private:
	static int count;

	std::vector<SphericalDatum> data;
	DataSetInfo info;

	void readAndPushDatum(rapidjson::Value& feature, const char* keyword);
	void readAndPushDatum(rapidjson::Value& feature, const char* keyword, int j);
	void readAndPushDatum(rapidjson::Value& feature, const char* keyword, int j, int k);
};


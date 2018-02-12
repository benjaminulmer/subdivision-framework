#include "SphericalData.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "Constants.h"

// Load data from GeoJson document

int SphericalData::count = 0;

SphericalData::SphericalData(rapidjson::Document & d, int num) {

	rapidjson::Value& featuresArray = d["features"];

	// Loop over all earthquakes in data file
	for (rapidjson::SizeType i = 0; i < featuresArray.Size(); i++) {
		double longitude = featuresArray[i]["geometry"]["coordinates"][0].GetDouble() * M_PI / 180.0;
		double latitude = featuresArray[i]["geometry"]["coordinates"][1].GetDouble() * M_PI / 180.0;
		double radius = RADIUS_EARTH_KM - featuresArray[i]["geometry"]["coordinates"][2].GetDouble();
		radius = radius / RADIUS_EARTH_KM * MODEL_SCALE;

		if (radius > MODEL_SCALE) radius = MODEL_SCALE;

		float datum = featuresArray[i]["properties"]["mag"].GetDouble();

		data.push_back(SphericalDatum(latitude, longitude, radius, datum));
	}
	calculateStats();

	if (num == 0) {
		info.binColors.push_back(glm::vec3(254.f/255, 229.f/255, 217.f/255));
		info.binColors.push_back(glm::vec3(252.f/255, 187.f/255, 161.f/255));
		info.binColors.push_back(glm::vec3(252.f/255, 146.f/255, 114.f/255));
		info.binColors.push_back(glm::vec3(251.f/255, 106.f/255, 74.f/255));
		info.binColors.push_back(glm::vec3(222.f/255, 45.f/255, 38.f/255));
		info.binColors.push_back(glm::vec3(165.f/255, 15.f/255, 21.f/255));
	}
	else {
		info.binColors.push_back(glm::vec3(241.f/255, 228.f/255, 246.f/255));
		info.binColors.push_back(glm::vec3(208.f/255, 209.f/255, 230.f/255));
		info.binColors.push_back(glm::vec3(166.f/255, 189.f/255, 219.f/255));
		info.binColors.push_back(glm::vec3(116.f/255, 169.f/255, 207.f/255));
		info.binColors.push_back(glm::vec3(43.f/255, 140.f/255, 190.f/255));
		info.binColors.push_back(glm::vec3(4.f/255, 90.f/255, 141.f/255));
	}
	info.id = count;
	count++;
}

// Fake volume "data"
SphericalData::SphericalData(std::vector<float> volumes) {
	for (float v : volumes) {
		data.push_back(SphericalDatum(v));
	}
	calculateStats();
	info.id = count;
	count++;
}

// Calculates statistics about data (max, min, avg)
void SphericalData::calculateStats() {
	info.max = -FLT_MAX;
	info.min = FLT_MAX;
	info.mean = 0.f;

	for (SphericalDatum d : data) {

		float v = d.datum;

		info.mean += v;
		info.max = (v > info.max) ? v : info.max;
		info.min = (v < info.min) ? v : info.min;
	}
	info.mean /= data.size();
}
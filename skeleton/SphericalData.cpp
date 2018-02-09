#include "SphericalData.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "Constants.h"

// Load data from GeoJson document
SphericalData::SphericalData(rapidjson::Document & d) {

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
	info.id = 0;
	calculateStats();
}

// Fake volume "data"
SphericalData::SphericalData(std::vector<float> volumes) {
	for (float v : volumes) {
		data.push_back(SphericalDatum(v));
	}
	info.id = 0;
	calculateStats();
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
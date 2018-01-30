#include "SphericalData.h"

#include "Geometry.h"
#include "Constants.h"

// Default constructor
SphericalData::SphericalData() {}

// Dummy data
SphericalData::SphericalData(int dummy) {

	std::mt19937 gen;
	gen.seed(5);
	std::uniform_real_distribution<double> dist1(0, M_PI / 2);
	std::normal_distribution<double> dist2(2, .1);
	std::uniform_real_distribution<float> dist3(-0.3, 0.3);

	for (int i = 0; i < 100000; i++) {
		double latitude = dist1(gen);
		double longitude = -dist1(gen);
		double raidus = dist2(gen);
		float datum = (M_PI / 2 - latitude) + dist3(gen);

		data.push_back(SphericalDatum(latitude, longitude, raidus, datum));
	}
	calculateStats();
}

// Load data from GeoJson document
SphericalData::SphericalData(rapidjson::Document & d) {

	rapidjson::Value& featuresArray = d["features"];

	// Loop over all earthquakes in data file
	for (rapidjson::SizeType i = 0; i < featuresArray.Size(); i++) {
		double longitude = featuresArray[i]["geometry"]["coordinates"][0].GetDouble() * M_PI / 180.0;
		double latitude = featuresArray[i]["geometry"]["coordinates"][1].GetDouble() * M_PI / 180.0;
		double radius = RADIUS_EARTH_KM - featuresArray[i]["geometry"]["coordinates"][2].GetDouble();
		radius = radius / RADIUS_EARTH_KM * MODEL_SCALE;

		float datum = featuresArray[i]["properties"]["mag"].GetDouble();

		data.push_back(SphericalDatum(latitude, longitude, radius, datum));
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

		float v = d.datum;

		avg += v;
		max = (v > max) ? v : max;
		min = (v < min) ? v : min;
	}
	avg /= data.size();
}
#include "SphericalData.h"

#include <limits>
#define _USE_MATH_DEFINES
#include <math.h>
#include <string>

#include "Constants.h"

// Load data from GeoJson document

int SphericalData::count = 0;

SphericalData::SphericalData(rapidjson::Document& d, rapidjson::Document& metaD) {

	rapidjson::Value& featuresArray = d["features"];
	const char* keyword = metaD["keyword"].GetString();

	// Loop over all features in the file
	for (rapidjson::SizeType i = 0; i < featuresArray.Size(); i++) {

		rapidjson::Value& feature = featuresArray[i];
		rapidjson::Value& geometry = feature["geometry"];
		rapidjson::Value& coords = geometry["coordinates"];

		// Points just have one data point
		if (std::string(geometry["type"].GetString()) == "Point") {
			readAndPushDatum(feature, keyword);
		}
		// Loop over all points in line
		else if (std::string(geometry["type"].GetString()) == "LineString") {

			for (rapidjson::SizeType j = 0; j < coords.Size(); j++) {
				readAndPushDatum(feature, keyword, j);
			}
		}
		// Double loop over all lines then all points in lines
		else if (std::string(geometry["type"].GetString()) == "MultiLineString") {

			for (rapidjson::SizeType j = 0; j < coords.Size(); j++) {

				for (rapidjson::SizeType k = 0; k < coords[j].Size(); k++) {
					readAndPushDatum(feature, keyword, j, k);
				}
			}
		}
	}

	rapidjson::Value& coloursArray = metaD["colours"];

	// Loop over colours to create colours for bins
	for (rapidjson::SizeType i = 0; i < coloursArray.Size(); i++) {

		float r = coloursArray[i]["r"].GetInt() / 255.f;
		float g = coloursArray[i]["g"].GetInt() / 255.f;
		float b = coloursArray[i]["b"].GetInt() / 255.f;

		info.binColors.push_back(glm::vec3(r, g, b));
	}

	calculateStats();
	info.id = count;
	count++;
}

// Reads datum and pushes to data vector
void SphericalData::readAndPushDatum(rapidjson::Value& feature, const char* keyword) {

	rapidjson::Value& geometry = feature["geometry"];
	rapidjson::Value& coords = geometry["coordinates"];

	double longitude = coords[0].GetDouble() * M_PI / 180.0;
	double latitude = coords[1].GetDouble() * M_PI / 180.0;
	double radius;

	// Depth info
	if (coords.Size() == 3) {
		radius = RADIUS_EARTH_KM - coords[2].GetDouble();
		radius = radius / RADIUS_EARTH_KM * RADIUS_EARTH_MODEL;
		//if (radius > RADIUS_EARTH_MODEL) radius = RADIUS_EARTH_MODEL;
	}
	// No depth info
	else {
		radius = RADIUS_EARTH_MODEL + 10.f * std::numeric_limits<float>::epsilon();
	}

	float datum;
	if (feature["properties"][keyword].IsDouble() || feature["properties"][keyword].IsInt()) {
		datum = (float) feature["properties"][keyword].GetDouble();
	}
	else {
		datum = (float) atof(feature["properties"][keyword].GetString());
	}
	data.push_back(SphericalDatum(latitude, longitude, radius, datum));
}

// Reads datum and pushes to data vector
void SphericalData::readAndPushDatum(rapidjson::Value& feature, const char* keyword, int j) {

	rapidjson::Value& geometry = feature["geometry"];
	rapidjson::Value& coords = geometry["coordinates"];

	double longitude = coords[j][0].GetDouble() * M_PI / 180.0;
	double latitude = coords[j][1].GetDouble() * M_PI / 180.0;
	double radius;

	// Depth info
	if (coords[j].Size() == 3) {
		radius = RADIUS_EARTH_KM - coords[j][2].GetDouble();
		radius = radius / RADIUS_EARTH_KM * RADIUS_EARTH_MODEL;
		//if (radius > RADIUS_EARTH_MODEL) radius = RADIUS_EARTH_MODEL;
	}
	// No depth info
	else {
		radius = RADIUS_EARTH_MODEL + 10.f * std::numeric_limits<float>::epsilon();
	}

	float datum;
	if (feature["properties"][keyword].IsDouble() || feature["properties"][keyword].IsInt()) {
		datum = (float) feature["properties"][keyword].GetDouble();
	}
	else {
		datum = (float) atof(feature["properties"][keyword].GetString());
	}
	data.push_back(SphericalDatum(latitude, longitude, radius, datum));
}

// Reads datum and pushes to data vector
void SphericalData::readAndPushDatum(rapidjson::Value& feature, const char* keyword, int j, int k) {

	rapidjson::Value& geometry = feature["geometry"];
	rapidjson::Value& coords = geometry["coordinates"];

	double longitude = coords[j][k][0].GetDouble() * M_PI / 180.0;
	double latitude = coords[j][k][1].GetDouble() * M_PI / 180.0;
	double radius;

	// Depth info
	if (coords[j][k].Size() == 3) {
		radius = RADIUS_EARTH_KM - coords[j][k][2].GetDouble();
		radius = radius / RADIUS_EARTH_KM * RADIUS_EARTH_MODEL;
		//if (radius > RADIUS_EARTH_MODEL) radius = RADIUS_EARTH_MODEL;
	}
	// No depth info
	else {
		radius = RADIUS_EARTH_MODEL + 10.f * std::numeric_limits<float>::epsilon();
	}

	float datum;
	if (feature["properties"][keyword].IsDouble() || feature["properties"][keyword].IsInt()) {
		datum = (float) feature["properties"][keyword].GetDouble();
	}
	else {
		datum = (float) atof(feature["properties"][keyword].GetString());
	}
	data.push_back(SphericalDatum(latitude, longitude, radius, datum));
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
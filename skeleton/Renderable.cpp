#define _USE_MATH_DEFINES
#include "Renderable.h"

#include "Constants.h"
#include "Geometry.h"

#include <algorithm>
#include <cmath>
#include <limits>

// Creates a renderable of the geometry specified in json document
Renderable::Renderable(const rapidjson::Document& d) : Renderable() {
	drawMode = GL_LINES;

	const rapidjson::Value& featuresArray = d["features"];

	// Loop over lines in data file
	for (rapidjson::SizeType i = 0; i < featuresArray.Size(); i++) {
		const rapidjson::Value& coordArray = featuresArray[i]["geometry"]["coordinates"];

		for (rapidjson::SizeType j = 0; j < coordArray.Size(); j++) {
			float lng = (float)( coordArray[j][0].GetDouble() * M_PI / 180.f );
			float lat = (float)( coordArray[j][1].GetDouble() * M_PI / 180.f );

			verts.push_back(glm::dvec3(sin(lng)*cos(lat), sin(lat), cos(lng)*cos(lat)) * RADIUS_EARTH_KM);
			colours.push_back(glm::vec3(0.0));

			if (j != 0 && j != coordArray.Size() - 1) {
				verts.push_back(glm::dvec3(sin(lng)*cos(lat), sin(lat), cos(lng)*cos(lat)) * RADIUS_EARTH_KM);
				colours.push_back(glm::vec3(0.f, 0.f, 0.f));
			}
		}
	}
}

void Renderable::vertsToRenderVerts() {

	double minX, maxX;
	double minY, maxY;
	double minZ, maxZ;
	maxX = maxY = maxZ = -std::numeric_limits<double>::max();
	minX = minY = minZ = std::numeric_limits<double>::max();

	for (const glm::dvec3& v : verts) {

		minX = std::min(minX, v.x);
		maxX = std::max(maxX, v.x);
		minY = std::min(minY, v.y);
		maxY = std::max(maxY, v.y);
		minZ = std::min(minZ, v.z);
		maxZ = std::max(maxZ, v.z);
	}
	centre = glm::dvec3((maxX + minX) / 2.0, (maxY + minY) / 2.0, (maxZ + minZ) / 2.0);

	for (const glm::dvec3& v : verts) {
		renderVerts.push_back(v - centre);
	}
}
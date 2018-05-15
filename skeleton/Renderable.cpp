#define _USE_MATH_DEFINES
#include "Renderable.h"

#include "Constants.h"
#include "Geometry.h"

#include <cmath>

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

			verts.push_back(glm::vec3(sin(lng)*cos(lat), sin(lat), cos(lng)*cos(lat)) * RADIUS_EARTH_KM);
			colours.push_back(glm::vec3(0.f, 0.f, 0.f));

			if (j != 0 && j != coordArray.Size() - 1) {
				verts.push_back(glm::vec3(sin(lng)*cos(lat), sin(lat), cos(lng)*cos(lat)) * RADIUS_EARTH_KM);
				colours.push_back(glm::vec3(0.f, 0.f, 0.f));
			}
		}
	}
}

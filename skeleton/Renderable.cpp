#include "Renderable.h"

#include "Constants.h"
#include "Geometry.h"

#define _USE_MATH_DEFINES
#include <math.h>

// Creates a renderable of the geometry specified in json document
Renderable::Renderable(rapidjson::Document& d) : Renderable() {
	drawMode = GL_LINES;

	rapidjson::Value& featuresArray = d["features"];

	// Loop over lines in data file
	for (rapidjson::SizeType i = 0; i < featuresArray.Size(); i++) {
		rapidjson::Value& coordArray = featuresArray[i]["geometry"]["coordinates"];

		for (rapidjson::SizeType j = 0; j < coordArray.Size(); j++) {
			float lng = coordArray[j][0].GetDouble() * M_PI / 180.0;
			float lat = coordArray[j][1].GetDouble() * M_PI / 180.0;

			verts.push_back(glm::vec3(sin(lng)*cos(lat), sin(lat), cos(lng)*cos(lat)) * 1.001f * (float) RADIUS_EARTH_MODEL);
			colours.push_back(glm::vec3(0.f, 0.f, 0.f));

			if (j != 0 && j != coordArray.Size() - 1) {
				verts.push_back(glm::vec3(sin(lng)*cos(lat), sin(lat), cos(lng)*cos(lat)) * 1.001f * (float) RADIUS_EARTH_MODEL);
				colours.push_back(glm::vec3(0.f, 0.f, 0.f));
			}
		}
	}
}

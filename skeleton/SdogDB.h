#pragma once

//
#include "Renderable.h"
#include "SdogCell.h"
#include "Geometry.h"
//

#include <sqlite\sqlite3.h>

#include <string>
#include <vector>

// Class for interfacing with an SQLite3 DB storing information about an SDOG grid
class SdogDB {

public:
	SdogDB(const std::string& path);
	SdogDB(const std::string& path, double radius);
	virtual ~SdogDB();

	void insertAirSigment(const std::vector<std::string>& interior, const std::vector<std::string>& boundary);

private:
	sqlite3* db;

	double radius;

	void insertCell(const std::string& code);
	void insertCells(const std::vector<std::string>& codes);

public:
	static void createRenderable(Renderable& r, const std::vector<std::string>& codes, double gridRad) {

		for (const std::string& code : codes) {

			SdogCell c(code, gridRad);
			float minLong = c.getMinLong(); float maxLong = c.getMaxLong();
			float minLat = c.getMinLat(); float maxLat = c.getMaxLat();
			float minRad = c.getMinRad(); float maxRad = c.getMaxRad();

			r.drawMode = GL_LINES;
			glm::vec3 origin(0.f, 0.f, 0.f);

			// Outer points
			glm::vec3 o1 = glm::vec3(sin(minLong)*cos(minLat), sin(minLat), cos(minLong)*cos(minLat)) * (float)maxRad;
			glm::vec3 o2 = glm::vec3(sin(maxLong)*cos(minLat), sin(minLat), cos(maxLong)*cos(minLat)) * (float)maxRad;
			glm::vec3 o3 = glm::vec3(sin(minLong)*cos(maxLat), sin(maxLat), cos(minLong)*cos(maxLat)) * (float)maxRad;
			glm::vec3 o4 = glm::vec3(sin(maxLong)*cos(maxLat), sin(maxLat), cos(maxLong)*cos(maxLat)) * (float)maxRad;

			// Inner points
			glm::vec3 i1 = glm::vec3(sin(minLong)*cos(minLat), sin(minLat), cos(minLong)*cos(minLat)) * (float)minRad;
			glm::vec3 i2 = glm::vec3(sin(maxLong)*cos(minLat), sin(minLat), cos(maxLong)*cos(minLat)) * (float)minRad;
			glm::vec3 i3 = glm::vec3(sin(minLong)*cos(maxLat), sin(maxLat), cos(minLong)*cos(maxLat)) * (float)minRad;
			glm::vec3 i4 = glm::vec3(sin(maxLong)*cos(maxLat), sin(maxLat), cos(maxLong)*cos(maxLat)) * (float)minRad;

			// Straight lines connect each inner point to coresponding outer point
			Geometry::createLineR(i1, o1, r);
			Geometry::createLineR(i2, o2, r);
			Geometry::createLineR(i3, o3, r);
			Geometry::createLineR(i4, o4, r);

			// Great circle arcs connect points on same longtitude line
			Geometry::createArcR(o1, o3, origin, r);
			Geometry::createArcR(o2, o4, origin, r);
			Geometry::createArcR(i1, i3, origin, r);
			Geometry::createArcR(i2, i4, origin, r);

			// Small circle arcs connect points on same latitude line
			Geometry::createArcR(o1, o2, glm::vec3(0.f, sin(minLat), 0.f) * (float)maxRad, r);
			Geometry::createArcR(o3, o4, glm::vec3(0.f, sin(maxLat), 0.f) * (float)maxRad, r);
			Geometry::createArcR(i1, i2, glm::vec3(0.f, sin(minLat), 0.f) * (float)minRad, r);
			Geometry::createArcR(i3, i4, glm::vec3(0.f, sin(maxLat), 0.f) * (float)minRad, r);
		}
	}
};
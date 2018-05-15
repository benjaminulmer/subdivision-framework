#pragma once

#include <glm/glm.hpp>

#include <string>
#include <vector>

#include "Renderable.h"

// Enum for the three different SDOG cell types plus an invalid cell code flag
enum class SdogCellType {
	NG,
	LG,
	SG,
	INVALID
};

// Class for storing information about an SDOG cell and performing queries
class SdogCell {

public:
	SdogCell(const std::string& code, double gridRadius);

	std::string getCode() { return code; }
	double getMinLat() { return minLat; }
	double getMaxLat() { return maxLat; }
	double getMinLong() { return minLong; }
	double getMaxLong() { return maxLong; }
	double getMinRad() { return minRad; }
	double getMaxRad() { return maxRad; }
	double getGridRad() { return gridRadius; }
	SdogCellType getType() { return type; }

	bool contains(double latRad, double longRad, double radius);

	void children(std::vector<std::string>& out);
	void neighbours(std::vector<std::string>& out);

	void addToRenderable(Renderable& r, const glm::vec3& colour);

	static std::string codeForPos(double latRad, double longRad, double radius, double gridRadius, unsigned int level);
	static bool codeIsValid(std::string code);

private:
	std::string code;

	double minLat, maxLat;
	double minLong, maxLong;
	double minRad, maxRad;
	double gridRadius;

	SdogCellType type;
};
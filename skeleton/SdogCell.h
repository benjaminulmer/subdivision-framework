#pragma once

#include "Renderable.h"
#include "SphCoord.h"

#include <glm/glm.hpp>

#include <string>
#include <vector>

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

	std::string getCode() const { return code; }
	double getMinLat() const { return minLat; }
	double getMaxLat() const { return maxLat; }
	double getMinLong() const { return minLong; }
	double getMaxLong() const { return maxLong; }
	double getMinRad() const { return minRad; }
	double getMaxRad() const { return maxRad; }
	double getGridRad() const { return gridRadius; }
	SdogCellType getType() const { return type; }
	std::pair<SphCoord, double> getMidPoint() const;

	bool contains(double latRad, double longRad, double radius);

	std::vector<std::string> children();
	std::vector<std::string> faceNeighbours();
	std::vector<std::string> allNeighbours();

	void addToRenderable(Renderable& r, const glm::vec3& colour, bool face);

	static std::string codeForPos(double latRad, double longRad, double radius, double gridRadius, unsigned int level);
	static bool codeIsValid(std::string code);
	operator std::string() const { return getCode(); }

private:
	std::string code;

	double minLat, maxLat;
	double minLong, maxLong;
	double minRad, maxRad;
	double gridRadius;

	SdogCellType type;
};

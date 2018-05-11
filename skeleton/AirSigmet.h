#pragma once

#include <string>
#include <vector>

#include "SphCoord.h"

class AirSigmet {

public:
	double minRad;
	double maxRad;
	std::vector<SphCoord> polygon;

	void gridInsertion(double gridRadius, int maxDepth, std::vector<std::string>& interior, std::vector<std::string>& boundary);
};


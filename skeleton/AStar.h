#pragma once

#include "SphCoord.h"
#include "SdogCell.h"

#include <functional>
#include <map>
#include <string>

namespace AStar {
	typedef std::function<double(std::string, std::string)> ScoreFunction;
	std::vector<std::string> findPath(
		SphCoord startCoord,
		SphCoord endCoord,
		ScoreFunction g,
		ScoreFunction h,
		double radius,
		double gridRadius,
		unsigned int level
	);
	std::vector<std::string> reconstructPath(
		std::map<std::string, std::string> cameFrom,
		std::string current
	);
};

#pragma once

#include <glm/glm.hpp>
#include <rapidjson/document.h>

#include <string>
#include <vector>

// Class for storing wind information in a 2D grid
class WindGrid {

public:
	WindGrid(int numRows, int numCols, double altM, double delta);

	glm::vec2& operator()(int r, int c);
	const glm::vec2& operator()(int r, int c) const;
	bool operator<(const WindGrid& r) const;

	static void gridInsertion(double gridRadius, int depth, const std::vector<WindGrid>& grids, std::vector<std::pair<std::string, glm::vec2>>& out);
	static void readFromJson(const rapidjson::Document& d, std::vector<WindGrid>& out);

private:
	int numRows;
	int numCols;
	double delta;
	double altM;

	glm::vec2* data;
};
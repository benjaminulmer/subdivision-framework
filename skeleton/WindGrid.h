#pragma once

#include <glm/glm.hpp>
#include <rapidjson/document.h>

#include <string>
#include <vector>

class WindGrid {

public:
	WindGrid(int numRows, int numCols, double altKM, double delta);

	glm::vec2& operator()(int r, int c);
	const glm::vec2& operator()(int r, int c) const;
	bool operator<(const WindGrid& r) const;

	static void gridInsertion(double gridRadius, int maxDepth, const std::vector<WindGrid>& grids, std::vector<std::pair<std::string, glm::vec2>>& out);
	static void readFromJson(const rapidjson::Document& d, std::vector<WindGrid>& out);

private:
	int numRows;
	int numCols;
	double delta;
	double altKM;

	glm::vec2* data;
};
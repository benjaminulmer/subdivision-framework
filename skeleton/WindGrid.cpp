#define _USE_MATH_DEFINES
#include "WindGrid.h"

#include "Constants.h"
#include "SdogCell.h"

#include <algorithm>
#include <cmath>


WindGrid::WindGrid(int numRows, int numCols, double altKM, double delta) : numRows(numRows), numCols(numCols), altKM(altKM), delta(delta) {
	data = new glm::vec2[numRows * numCols];
}

glm::vec2& WindGrid::operator()(int r, int c) {
	return data[r * numCols + c];
}

const glm::vec2& WindGrid::operator()(int r, int c) const {
	return data[r * numCols + c];
}

// Order wind grids by increasing altitude
bool WindGrid::operator<(const WindGrid & r) const {
	return altKM < r.altKM;
}

#include <iostream>
void WindGrid::gridInsertion(double gridRadius, int maxDepth, const std::vector<WindGrid>& grids, std::vector<std::pair<std::string, glm::vec2>>& out) {

	double minAltKM = grids[0].altKM;
	double maxAltKM = grids[grids.size() - 1].altKM;

	double delta = grids[0].delta;

	for (const WindGrid& w : grids) {
		std::cout << w.altKM << std::endl;
	}

	std::vector<SdogCell> toTest;
	toTest.push_back(SdogCell("0", gridRadius));
	toTest.push_back(SdogCell("1", gridRadius));
	toTest.push_back(SdogCell("2", gridRadius));
	toTest.push_back(SdogCell("3", gridRadius));
	toTest.push_back(SdogCell("4", gridRadius));
	toTest.push_back(SdogCell("5", gridRadius));
	toTest.push_back(SdogCell("6", gridRadius));
	toTest.push_back(SdogCell("7", gridRadius));

	while (toTest.size() != 0) {

		SdogCell c = toTest[toTest.size() - 1];
		toTest.pop_back();

		if (c.getCode().length() < maxDepth + 1) {

			std::vector<std::string> children;
			c.children(children);

			for (const std::string& childCode : children) {

				SdogCell child(childCode, gridRadius);

				if (child.getMaxRad() > altToAbs(minAltKM) && child.getMinRad() < altToAbs(maxAltKM)) {
					toTest.push_back(child);
				}
			}
		}
		else {
			double midRad = 0.5 * c.getMinRad() + 0.5 * c.getMaxRad();
			double midLat = 0.5 * c.getMinLat() + 0.5 * c.getMaxLat();
			double midLong = 0.5 * c.getMinLong() + 0.5 * c.getMaxLong();

			if (midRad < altToAbs(minAltKM) || midRad > altToAbs(maxAltKM)) {
				continue;
			}

			double midLatDeg = midLat * (180.0 / M_PI);
			double midLongDeg = midLong * (180.0 / M_PI);

			int midLatTrunc = (int)(midLatDeg * delta);
			int midLongTrunc = (int)(midLongDeg * delta);

			int radIndex = 0;
			for (int i = 1; i < grids.size(); i++) {

				if (midRad > altToAbs(grids[i].altKM)) {
					radIndex++;
				}
				else {
					break;
				}
			}

			double latPerc = midLatDeg * delta - midLatTrunc;
			double longPerc = midLongDeg * delta - midLongTrunc;
			double radPerc = 1.0 - (midRad - altToAbs(grids[radIndex].altKM)) / (altToAbs(grids[radIndex + 1].altKM) - altToAbs(grids[radIndex].altKM));

			if (latPerc < 0.0) latPerc = 1.0 + latPerc;
			if (longPerc < 0.0) longPerc = 1.0 + longPerc;

			int latIndex = -1 * midLatTrunc + 90 * delta;
			int longIndex = midLongTrunc;
			if (longIndex < 0) {
				longIndex += 360 * delta;
			}

			glm::vec2 _100 = grids[radIndex](latIndex, longIndex);
			glm::vec2 _110 = grids[radIndex](latIndex, (longIndex + 1) % (int)(360 * delta));
			glm::vec2 _000 = grids[radIndex](latIndex + 1, longIndex);
			glm::vec2 _010 = grids[radIndex](latIndex + 1, (longIndex + 1) % (int)(360 * delta));
			glm::vec2 _101 = grids[radIndex + 1](latIndex, longIndex);
			glm::vec2 _111 = grids[radIndex + 1](latIndex, (longIndex + 1) % (int)(360 * delta));
			glm::vec2 _001 = grids[radIndex + 1](latIndex + 1, longIndex);
			glm::vec2 _011 = grids[radIndex + 1](latIndex + 1, (longIndex + 1) % (int)(360 * delta));

			_000 *= (1.0 - latPerc) * (1.0 - longPerc) * (1.0 - radPerc);
			_001 *= (1.0 - latPerc) * (1.0 - longPerc) * radPerc;
			_010 *= (1.0 - latPerc) * longPerc * (1.0 - radPerc);
			_011 *= (1.0 - latPerc) * longPerc * radPerc;
			_100 *= latPerc * (1.0 - longPerc) * (1.0 - radPerc);
			_101 *= latPerc * (1.0 - longPerc) * radPerc;
			_110 *= latPerc * longPerc * (1.0 - radPerc);
			_111 *= latPerc * longPerc * radPerc;

			glm::vec2 _da = _000 + _001 + _010 + _011 + _100 + _101 + _110 + _111;
			out.push_back(std::pair<std::string, glm::vec2>(c.getCode(), _da));
		}
	}
}


void WindGrid::readFromJson(const rapidjson::Document& d, std::vector<WindGrid>& out) {

	for (rapidjson::SizeType i = 0; i < d.Size(); i += 2) {

		// Get header information and U and V data
		const rapidjson::Value& header = d[i]["header"];
		const rapidjson::Value& dataU = d[i]["data"];
		const rapidjson::Value& dataV = d[i+1]["data"];

		double mb = header["surface1Value"].GetDouble() / 100.0;

		double altFT = 145366.45 * (1.0 - pow(mb / 1013.25, 0.190284)); // mb to feet
		double altKM = altFT * (0.3048 / 1000.0); // feet to KM

		double delta = 1.0 / header["dx"].GetDouble();

		int numRows = 180 * delta + 1;
		int numCols = 360 * delta;

		WindGrid layer(numRows, numCols, altKM, delta);

		// Populate array from data
		for (int r = 0; r < numRows; r++) {
			for (int c = 0; c < numCols; c++) {

				int index = r * numCols + c;
				layer(r, c).x = dataU[index].GetDouble();
				layer(r, c).y = dataV[index].GetDouble();
			}
		}	

		out.push_back(layer);
	}
	// Sort list by increasing altitude
	std::sort(out.begin(), out.end());
}
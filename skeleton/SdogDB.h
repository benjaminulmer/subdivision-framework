#pragma once

#include "AirSigmet.h"

#include <sqlite\sqlite3.h>

#include <string>
#include <vector>

// Class for interfacing with an SQLite3 DB storing information about an SDOG grid
class SdogDB {

public:
	SdogDB(const std::string& path);
	SdogDB(const std::string& path, double radius);
	virtual ~SdogDB();

	void insertAirSigmet(const std::vector<std::string>& interior, const std::vector<std::string>& boundary, const AirSigmet& airSigmet);
	void insertWindData(const std::vector<std::pair<std::string, glm::vec2>>& list);
	void getAirSigmetCells(std::vector<AirSigmetCells>& out);
	void getWindCells(std::vector<std::pair<std::string, glm::vec2>>& out);

private:
	sqlite3* db;

	double radius;

	void insertCell(const std::string& code);
	void insertCells(const std::vector<std::string>& codes);
};
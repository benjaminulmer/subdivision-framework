#pragma once

//
#include <iostream>
//

#include <sqlite\sqlite3.h>

#include <string>
#include <vector>

// Class for interfacing with an SQLite3 DB storing information about an SDOG grid
class SdogDB {

public:
	SdogDB(const std::string& path);
	SdogDB(const std::string& path, int depth);
	virtual ~SdogDB();

private:
	sqlite3* db;

	int maxDepth;
	double maxRadius;

	void insertCell(const std::string& code);
	void insertCells(const std::vector<std::string>& codes);
};
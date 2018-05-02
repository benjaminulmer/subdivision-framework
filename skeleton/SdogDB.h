#pragma once

//
#include <iostream>
//

#include <sqlite\sqlite3.h>

#include <string>
#include <vector>

// Enum for the three different SDOG cell types
enum class SdogCellType{
	NG,
	LG,
	SG,
	INVALID
};

// Class for storing information about an SDOG cell and performing queries
class SdogCell {

public:
	SdogCell(const std::string& code, double maxRadius);

	std::string getCode() { return code; }
	double getMinLat() { return minLat; }
	double getMaxLat() { return maxLat; }
	double getMinLong() { return minLong; }
	double getMaxLong() { return maxLong; }
	double getMinRad() { return minRad; }
	double getMaxRad() { return maxRad; }
	SdogCellType getType() { return type; }

private:
	std::string code;

	double minLat, maxLat;
	double minLong, maxLong;
	double minRad, maxRad;

	SdogCellType type;
};

// Class for interfacing with an SQLite3 DB storing information about an SDOG grid
class SdogDB {

public:
	SdogDB(const std::string& path);
	SdogDB(const std::string& path, int depth);
	virtual ~SdogDB();

	std::string codeForPos(double latRad, double longRad, double radius, unsigned int level);
	bool neighbours(const std::string& code, std::vector<std::string>& out);

	static bool codeIsValid(std::string code);

private:
	sqlite3* db;

	int maxDepth;
	double maxRadius;

	void insertCell(const std::string& code);
	void insertCells(const std::vector<std::string>& codes);
};


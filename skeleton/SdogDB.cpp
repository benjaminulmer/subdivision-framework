#define _USE_MATH_DEFINES
#include "SdogDB.h"

#include <cmath>
#include <algorithm>

// Constructs the SDOG cell for the given code
//
// code - index code of the desired SDOG cell
// maxRadius - maximum radius of the SDOG grid the cell belongs to
SdogCell::SdogCell(const std::string& code, double maxRadius) : code(code) {

	minRad = 0.0;
	maxRad = maxRadius;

	// Set initial values based on which octanct cell is in
	if (code[0] == '0') {
		minLat = 0.0;
		maxLat = M_PI_2;
		minLong = 0.0;
		maxLong = M_PI_2;
	}
	else if (code[0] == '1') {
		minLat = 0.0;
		maxLat = M_PI_2;
		minLong = M_PI_2;
		maxLong = M_PI;
	}
	else if (code[0] == '2') {
		minLat = 0.0;
		maxLat = M_PI_2;
		minLong = 0.0;
		maxLong = -M_PI_2;
	}
	else if (code[0] == '3') {
		minLat = 0.0;
		maxLat = M_PI_2;
		minLong = -M_PI_2;
		maxLong = -M_PI;
	}
	else if (code[0] == '4') {
		minLat = 0.0;
		maxLat = -M_PI_2;
		minLong = 0.0;
		maxLong = M_PI_2;
	}
	else if (code[0] == '5') {
		minLat = 0.0;
		maxLat = -M_PI_2;
		minLong = M_PI_2;
		maxLong = M_PI;
	}
	else if (code[0] == '6') {
		minLat = 0.0;
		maxLat = -M_PI_2;
		minLong = 0.0;
		maxLong = -M_PI_2;
	}
	else if (code[0] == '7') {
		minLat = 0.0;
		maxLat = -M_PI_2;
		minLong = -M_PI_2;
		maxLong = -M_PI;
	}
	else {
		// error
	}

	// Loop for each char in code and determine properties based on code
	type = SdogCellType::SG;
	for (unsigned int i = 1; i < code.length(); i++) {

		double midLat = 0.5 * minLat + 0.5 * maxLat;
		double midLong = 0.5 * minLong + 0.5 * maxLong;
		double midRad = 0.5 * minRad + 0.5 * maxRad;

		if (type == SdogCellType::NG) {

			if (code[i] == '0') {
				minRad = midRad;
				maxLat = midLat;
				maxLong = midLong;
			}
			else if (code[i] == '1') {
				minRad = midRad;
				maxLat = midLat;
				minLong = midLong;
			}
			else if (code[i] == '2') {
				minRad = midRad;
				minLat = midLat;
				maxLong = midLong;
			}
			else if (code[i] == '3') {
				minRad = midRad;
				minLat = midLat;
				minLong = midLong;
			}
			else if (code[i] == '4') {
				maxRad = midRad;
				maxLat = midLat;
				maxLong = midLong;
			}
			else if (code[i] == '5') {
				maxRad = midRad;
				maxLat = midLat;
				minLong = midLong;
			}
			else if (code[i] == '6') {
				maxRad = midRad;
				minLat = midLat;
				maxLong = midLong;
			}
			else if (code[i] == '7') {
				maxRad = midRad;
				minLat = midLat;
				minLong = midLong;
			}
			else {
				type = SdogCellType::INVALID;
				break;
			}
			// type doesn't change
		}
		else if (type == SdogCellType::LG) {

			if (code[i] == '0') {
				minRad = midRad;
				maxLat = midLat;
				maxLong = midLong;
				type = SdogCellType::NG;
			}
			else if (code[i] == '1') {
				minRad = midRad;
				maxLat = midLat;
				minLong = midLong;
				type = SdogCellType::NG;
			}
			else if (code[i] == '2') {
				minRad = midRad;
				minLat = midLat;
				// type doesn't change
			}
			else if (code[i] == '3') {
				maxRad = midRad;
				maxLat = midLat;
				maxLong = midLong;
				type = SdogCellType::NG;
			}
			else if (code[i] == '4') {
				maxRad = midRad;
				maxLat = midLat;
				minLong = midLong;
				type = SdogCellType::NG;
			}
			else if (code[i] == '5') {
				maxRad = midRad;
				minLat = midLat;
				// type doesn't change
			}
			else {
				type = SdogCellType::INVALID;
				break;
			}
		}
		else {// type == CellType::SG

			if (code[i] == '0') {
				minRad = midRad;
				maxLat = midLat;
				maxLong = midLong;
				type = SdogCellType::NG;
			}
			else if (code[i] == '1') {
				minRad = midRad;
				maxLat = midLat;
				minLong = midLong;
				type = SdogCellType::NG;
			}
			else if (code[i] == '2') {
				minRad = midRad;
				minLat = midLat;
				type = SdogCellType::LG;
			}
			else if (code[i] == '3') {
				maxRad = midRad;
				// type doesn't change
			}
			else {
				type = SdogCellType::INVALID;
				break;
			}
		}
	}
}


// Creates connection to the provided database. Does not add any
// new data to the DB. Used for connecting with an existing DB file.
//
// path - path to SQLite3 DB file
SdogDB::SdogDB(const std::string& path) {
	sqlite3_open(path.c_str(), &db);
}


// Creates connection to the provided database and creates cells
// up to the provided depth. Used for populating a new DB file.
//
// path - path to SQLite3 DB file
// depth - depth to subdivide grid to
SdogDB::SdogDB(const std::string& path, int depth) {

	sqlite3_open(path.c_str(), &db);

//	sqlite3_exec(db, "PRAGMA schema.journal_mode = OFF", NULL, NULL, NULL);
//	sqlite3_exec(db, "PRAGMA schema.synchronous = OFF", NULL, NULL, NULL);

	char* sql = "CREATE TABLE Cells (CellID INTEGER PRIMARY KEY AUTOINCREMENT, Code TEXT NOT NULL UNIQUE);";
	sqlite3_exec(db, sql, NULL, NULL, NULL);

	insertCell("0");
	insertCell("1");
	insertCell("2");
	insertCell("3");
	insertCell("4");
	insertCell("5");
	insertCell("6");
	insertCell("7");
	maxDepth = 0;

	while (maxDepth < depth) {

		constexpr int batchSize = 1000000;
		std::vector<std::string> toAdd;

		char* sqlEmpty = "SELECT Code FROM cells WHERE length(Code) = %i;";
		char* sqlFilled = sqlite3_mprintf(sqlEmpty, maxDepth + 1);

		sqlite3_stmt* stmt;
		sqlite3_prepare_v2(db, sqlFilled, -1, &stmt, NULL);

		while (sqlite3_step(stmt) != SQLITE_DONE) {

			std::string code = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
			SdogCell cell(code, maxRadius);

			if (toAdd.size() >= batchSize) {
				insertCells(toAdd);
				toAdd.clear();
			}

			if (cell.getType() == SdogCellType::NG) {
				toAdd.push_back(code + "0");
				toAdd.push_back(code + "1");
				toAdd.push_back(code + "2");
				toAdd.push_back(code + "3");
				toAdd.push_back(code + "4");
				toAdd.push_back(code + "5");
				toAdd.push_back(code + "6");
				toAdd.push_back(code + "7");
			}
			else if (cell.getType() == SdogCellType::LG) {
				toAdd.push_back(code + "0");
				toAdd.push_back(code + "1");
				toAdd.push_back(code + "2");
				toAdd.push_back(code + "3");
				toAdd.push_back(code + "4");
				toAdd.push_back(code + "5");
			}
			else {// cell.getType() == SdogCellType::SG
				toAdd.push_back(code + "0");
				toAdd.push_back(code + "1");
				toAdd.push_back(code + "2");
				toAdd.push_back(code + "3");
			}
		}
		insertCells(toAdd);
		maxDepth++;
	}
}


// Ends connection with the database
SdogDB::~SdogDB() {
	sqlite3_close(db);
}


// Inserts the given code into the DB
//
// code - index code of cell to insert
void SdogDB::insertCell(const std::string& code) {

	char* sqlEmpty = "INSERT OR IGNORE into Cells (Code) VALUES ('%s');";
	char* sqlFilled = sqlite3_mprintf(sqlEmpty, code.c_str());

	sqlite3_exec(db, sqlFilled, NULL, NULL, NULL);
	sqlite3_free(sqlFilled);
}


// Inserts a list of codes into the DB
//
// codes - vector of codes for cells to insert
void SdogDB::insertCells(const std::vector<std::string>& codes) {

	sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
	char* sqlEmpty = "INSERT OR IGNORE into Cells (Code) VALUES (@CO);";
	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(db, sqlEmpty, -1, &stmt, NULL);

	for (const std::string& code : codes) {
		
		sqlite3_bind_text(stmt, 1, code.c_str(), -1, SQLITE_STATIC);
		sqlite3_step(stmt);

		sqlite3_clear_bindings(stmt);
		sqlite3_reset(stmt);
	}
	sqlite3_exec(db, "END TRANSACTION;", NULL, NULL, NULL);
}


// Returns the SDOG cell code for the provided spherical point at a given subdivision level
//
// latRad - latitude of point, in radians
// longRad - longitude of point, in radians
// radius - radius of point (0 is centre of the sphere), in the units the grid was constructed with
// return - code for the cell that contains the point
std::string SdogDB::codeForPos(double latRad, double longRad, double radius, unsigned int level) {

	std::string code = "";
	if (longRad < -M_PI) {
		longRad += 2.0 * M_PI;
	}
	if (longRad > M_PI) {
		longRad -= 2.0 * M_PI;
	}

	double minLat, maxLat, minLong, maxLong, minRad, maxRad;
	minLat = 0.0;
	maxLat = M_PI_2;
	minRad = 0.0;
	maxRad = maxRadius;

	// Determine which otcant the point is in
	int octCode = 0;
	if (latRad < 0.0) {
		octCode += 4;
	}
	if (longRad < 0.0) {
		octCode += 2;
	}
	if (abs(longRad) > M_PI_2) {
		octCode += 1;
		minLong = M_PI_2;
		maxLong = M_PI;
	}
	else {
		minLong = 0.0;
		maxLong = M_PI_2;
	}
	latRad = abs(latRad);
	longRad = abs(longRad);

	code += std::to_string(octCode);

	// Loop for desired number of levels and determine
	// which child point is in for each itteration
	SdogCellType curType = SdogCellType::SG;
	for (unsigned int i = 0; i < level; i++) {

		int childCode = 0;
		double midLat = 0.5 * minLat + 0.5 * maxLat;
		double midLong = 0.5 * minLong + 0.5 * maxLong;
		double midRad = 0.5 * minRad + 0.5 * maxRad;

		if (curType == SdogCellType::NG) {

			if (radius > midRad) {
				minRad = midRad;
			}
			else {
				childCode += 4;
				maxRad = midRad;
			}
			if (latRad < midLat) {
				maxLat = midLat;
			}
			else {
				childCode += 2;
				minLat = midLat;
			}
			if (longRad < midLong) {
				maxLong = midLong;
			}
			else {
				childCode += 1;
				minLong = midLong;
			}
			// type doesn't change
		}
		else if (curType == SdogCellType::LG) {

			if (radius > midRad) {
				minRad = midRad;
			}
			else {
				maxRad = midRad;
				childCode += 3;
			}
			if (latRad < midLat) {
				maxLat = midLat;
				curType = SdogCellType::NG;

				if (longRad < midLong) {
					maxLong = midLong;
				}
				else {
					childCode += 1;
					minLong = midLong;
				}
			}
			else {
				childCode += 2;
				minLat = midLat;
				// type doesn't change
			}
		}
		else {// curType == SdogCellType::SG

			if (radius > midRad) {

				minRad = midRad;

				if (latRad < midLat) {
					maxLat = midLat;
					curType = SdogCellType::NG;

					if (longRad < midLong) {
						childCode = 0;
						maxLong = midLong;
					}
					else {
						childCode = 1;
						minLong = midLong;
					}
				}
				else {
					childCode = 2;
					minLat = midLat;
					curType = SdogCellType::LG;
				}
			}
			else {
				childCode = 3;
				maxRad = midRad;
				// type doesn't change
			}
		}
		code += std::to_string(childCode);
	}
	return code;
}


// Gets the codes of the neighbours that share a face with the provided cell
//
// code - code for a cell to find neighbours of
// out - output vector that stores the neighbours - treats as empty
// return - false on invalid code, true otherwise
bool SdogDB::neighbours(const std::string& code, std::vector<std::string>& out) {

	// Get cell info and make sure it's valid
	SdogCell c(code, maxRadius);
	if (c.getType() == SdogCellType::INVALID) {
		return false;
	}
	unsigned int level = (unsigned int) code.length() - 1;

	double midLat = 0.5 * c.getMinLat() + 0.5 * c.getMaxLat();
	double midLong = 0.5 * c.getMinLong() + 0.5 * c.getMaxLong();
	double midRad = 0.5 * c.getMinRad() + 0.5 * c.getMaxRad();

	double latDist = c.getMaxLat() - c.getMinLat();
	double longDist = c.getMaxLong() - c.getMinLong();
	double radDist = c.getMaxRad() - c.getMinRad();

	// Get codes for location of all posible neighbours
	out.push_back(codeForPos(midLat + latDist, midLong, midRad, level));
	out.push_back(codeForPos(midLat, midLong + longDist, midRad, level));
	out.push_back(codeForPos(midLat, midLong - longDist, midRad, level));
	out.push_back(codeForPos(midLat, midLong, midRad - radDist, level));

	out.push_back(codeForPos(midLat - latDist, midLong + 0.01 * longDist, midRad, level));
	out.push_back(codeForPos(midLat - latDist, midLong - 0.01 * longDist, midRad, level));

	out.push_back(codeForPos(midLat + 0.01 * latDist, midLong + 0.01 * longDist, midRad + radDist, level));
	out.push_back(codeForPos(midLat + 0.01 * latDist, midLong - 0.01 * longDist, midRad + radDist, level));
	out.push_back(codeForPos(midLat - 0.01 * latDist, midLong + 0.01 * longDist, midRad + radDist, level));
	out.push_back(codeForPos(midLat - 0.01 * latDist, midLong - 0.01 * longDist, midRad + radDist, level));

	// Remove duplicates and self
	std::sort(out.begin(), out.end());
	out.erase(std::unique(out.begin(), out.end()), out.end());
	out.erase(std::remove(out.begin(), out.end(), code), out.end());

	return true;
}


// Returns if the provided SDOG code is a valid cell or not
bool SdogDB::codeIsValid(std::string code) {

	if (code.length() < 1) {
		return false;
	}

	// Ensure octant code is valid
	char o = code[0];
	if (!(o == '0' || o == '1' || o == '2' || o == '3' || o == '4' || o == '5' || o == '6' || o == '7')) {
		return false;
	}

	// Loop through code to ensure each character is valid given the cell type of the previous element
	SdogCellType prevType = SdogCellType::SG;
	for (unsigned int i = 1; i < code.length(); i++) {

		char c = code[1];
		if (prevType == SdogCellType::NG) {
			if (!(c == '0' || c == '1' || c == '2' || c == '3' || c == '4' || c == '5' || c == '6' || c == '7')) {
				return false;
			}
		}
		else if (prevType == SdogCellType::LG) {
			if (c == '0' || c == '1' || c == '3' || c == '4') {
				prevType = SdogCellType::NG;
			}
			else if (c == '2' || c == '5') {
				prevType = SdogCellType::LG;
			}
			else {
				return false;
			}
		}
		else {// prevType == SdogCellType::SG
			if (c == '0' || c == '1') {
				prevType = SdogCellType::NG;
			}
			else if (c == '2') {
				prevType = SdogCellType::LG;
			}
			else if (c == '3') {
				prevType = SdogCellType::SG;
			}
			else {
				return false;
			}
		}
	}
	return true;
}
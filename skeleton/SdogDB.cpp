#include "SdogDB.h"

#include "SdogCell.h"

#include <algorithm>
#include <cmath>
#include <unordered_map>

// Creates a connection to the provided database. Does not populate with any data
//
// path - path to SQLite3 DB file
SdogDB::SdogDB(const std::string& path) {
	sqlite3_open(path.c_str(), &db);
}


// Creates connection to the provided database and creates the needed tables.
// Inserts the eight SDOG octants into the data base
//
// path - path to SQLite3 DB file
SdogDB::SdogDB(const std::string& path, double radius) : radius(radius) {

	sqlite3_open(path.c_str(), &db);

	// Create tables in the database
	char* tab0 = "CREATE TABLE info(lock INTEGER, gridRadius REAL,"
	             "PRIMARY KEY(Lock), CONSTRAINT info_locked CHECK(lock = 0) )";

	char* tab1 = "CREATE TABLE cells (cellID INTEGER PRIMARY KEY AUTOINCREMENT, code TEXT NOT NULL UNIQUE, windU REAL, windV REAL);";

	char* tab2 = "CREATE TABLE airSigmets (airSigmetID INTEGER PRIMARY KEY AUTOINCREMENT, validFrom TEXT, validUntil TEXT,"
	             "minAltKM REAL, maxAltKM REAL, dirDeg INTEGER, speedKT INTEGER, hazard INTEGER, severity INTEGER, type INTEGER)";

	char* tab3 = "CREATE TABLE airSigmetBounds (airSigmetID INTEGER, number INTEGER, latRad REAL, longRad REAL,"
	             "PRIMARY KEY(airSigmetID, number),"
	             "FOREIGN KEY(airSigmetID) REFERENCES airSigmet(airSigmetID) )";

	char* tab4 = "CREATE TABLE cellHasAirSigmet (cellID INTEGER, airSigmetID INTEGER, boundary INTEGER,"
	             "PRIMARY KEY(cellID, airSigmetID),"
	             "FOREIGN KEY(airSigmetID) REFERENCES airSigmet(airSigmetID),"
	             "FOREIGN KEY(cellID) REFERENCES cells(cellID) )";

	sqlite3_exec(db, tab0, NULL, NULL, NULL);
	sqlite3_exec(db, tab1, NULL, NULL, NULL);
	sqlite3_exec(db, tab2, NULL, NULL, NULL);
	sqlite3_exec(db, tab3, NULL, NULL, NULL);
	sqlite3_exec(db, tab4, NULL, NULL, NULL);

	char* config = "INSERT OR IGNORE INTO info(lock, gridRadius) VALUES (0, @RA)";
	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(db, config, -1, &stmt, NULL);

	sqlite3_bind_double(stmt, 1, radius);
	sqlite3_step(stmt);
	sqlite3_finalize(stmt);
}


// Ends connection with the database
SdogDB::~SdogDB() {
	sqlite3_close(db);
}


// Inserts a new AIRMET/SIGMET into the data base with the provided interior and boundary cells
//
// interior - list of cells that are completely inside the SIGMET/AIRMET
// boundary - list of cells that are on the boundary of the SIGMET/AIRMET
// airSigmet - airSigmet that is associated with the provided cells
void SdogDB::insertAirSigmet(const std::vector<std::string>& interior, const std::vector<std::string>& boundary, const AirSigmet& airSigmet) {

	sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, NULL);
	sqlite3_stmt *insert0Stmt, *insert1Stmt, *insert2Stmt, *selectStmt;

	// Insert AirSigmet data into DB
	char* sqlInsert0 = "INSERT INTO airSigmets(validFrom, validUntil, minAltKM, maxAltKM, dirDeg, speedKT, hazard, severity, type)"
	                   "VALUES (@VF, @VU, @MIN, @MAX, @DIR, @SPD, @HAZ, @SEV, @TYP)";

	sqlite3_prepare_v2(db, sqlInsert0, -1, &insert0Stmt, NULL);
	sqlite3_bind_text(insert0Stmt, 1, airSigmet.validFrom.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(insert0Stmt, 2, airSigmet.validUntil.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_double(insert0Stmt, 3, airSigmet.minAltKM);
	sqlite3_bind_double(insert0Stmt, 4, airSigmet.maxAltKM);
	sqlite3_bind_int(insert0Stmt, 5, airSigmet.dirDeg);
	sqlite3_bind_int(insert0Stmt, 6, airSigmet.speedKT);
	sqlite3_bind_int(insert0Stmt, 7, (int)airSigmet.hazard);
	sqlite3_bind_int(insert0Stmt, 8, (int)airSigmet.severity);
	sqlite3_bind_int(insert0Stmt, 9, (int)airSigmet.type);
	sqlite3_step(insert0Stmt);
	sqlite3_finalize(insert0Stmt);

	int airSigmetID = sqlite3_last_insert_rowid(db);

	// Insert the geometry of the AirSigmet into the DB
	char* sqlInsert1 = "INSERT INTO airSigmetBounds(airSigmetID, number, latRad, longRad) VALUES (@ID, @NUM, @LAT, @LNG)";
	sqlite3_prepare_v2(db, sqlInsert1, -1, &insert1Stmt, NULL);

	int i = 0;
	for (const SphCoord& c : airSigmet.polygon) {
		sqlite3_bind_int(insert1Stmt, 1, airSigmetID);
		sqlite3_bind_int(insert1Stmt, 2, i);
		sqlite3_bind_double(insert1Stmt, 3, c.latitude);
		sqlite3_bind_double(insert1Stmt, 4, c.longitude);
		i++;

		sqlite3_step(insert1Stmt);
		sqlite3_clear_bindings(insert1Stmt);
		sqlite3_reset(insert1Stmt);
	}

	char* sqlInsert2 = "INSERT INTO cellHasAirSigmet(cellID, airSigmetID, boundary) VALUES (@CO, @AS, @BO)";
	char* sqlSelect = "SELECT cellID FROM cells WHERE code = @CO;";

	sqlite3_prepare_v2(db, sqlInsert2, -1, &insert2Stmt, NULL);
	sqlite3_prepare_v2(db, sqlSelect, -1, &selectStmt, NULL);

	// Insert interior cells into DB
	for (const std::string& code : interior) {

		// Insert cell into DB if not already exists and get its ID
		int cellID;
		sqlite3_bind_text(selectStmt, 1, code.c_str(), -1, SQLITE_STATIC);

		if (sqlite3_step(selectStmt) == SQLITE_DONE) {
			insertCell(code);
			cellID = sqlite3_last_insert_rowid(db);
		}
		else {
			cellID = sqlite3_column_int(selectStmt, 0);
		}

		// Insert cell-data relation into DB
		sqlite3_bind_int(insert2Stmt, 1, cellID);
		sqlite3_bind_int(insert2Stmt, 2, airSigmetID);
		sqlite3_bind_int(insert2Stmt, 3, false);
		sqlite3_step(insert2Stmt);

		// Clear bindings and reset SQL statements
		sqlite3_clear_bindings(selectStmt);
		sqlite3_clear_bindings(insert2Stmt);
		sqlite3_reset(selectStmt);
		sqlite3_reset(insert2Stmt);
	}

	// Insert boundary cells into DB
	for (const std::string& code : boundary) {

		// Insert cell into DB if not already exists and get its ID
		int cellID;
		sqlite3_bind_text(selectStmt, 1, code.c_str(), -1, SQLITE_STATIC);

		if (sqlite3_step(selectStmt) == SQLITE_DONE) {
			insertCell(code);
			cellID = sqlite3_last_insert_rowid(db);
		}
		else {
			cellID = sqlite3_column_int(selectStmt, 0);
		}

		// Insert cell-data relation into DB
		sqlite3_bind_int(insert2Stmt, 1, cellID);
		sqlite3_bind_int(insert2Stmt, 2, airSigmetID);
		sqlite3_bind_int(insert2Stmt, 3, true);
		sqlite3_step(insert2Stmt);

		// Clear bindings and reset SQL statements
		sqlite3_clear_bindings(selectStmt);
		sqlite3_clear_bindings(insert2Stmt);
		sqlite3_reset(selectStmt);
		sqlite3_reset(insert2Stmt);
	}

	sqlite3_finalize(insert2Stmt);
	sqlite3_finalize(selectStmt);
	sqlite3_exec(db, "END TRANSACTION", NULL, NULL, NULL);
}

// Inserts wind information for the provided cells into the DB
//
// list - list of cells and their associated wind data
void SdogDB::insertWindData(const std::vector<std::pair<std::string, glm::vec2>>& list) {

	sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, NULL);
	sqlite3_stmt *insertStmt, *updateStmt;

	char* sqlInsert = "INSERT INTO cells(code, windU, windV) VALUES (@CO, @WU, @WV)";
	char* sqlUpdate = "UPDATE cells SET windU = @WU, windV = @WV WHERE code = @CO";

	sqlite3_prepare_v2(db, sqlInsert, -1, &insertStmt, NULL);
	sqlite3_prepare_v2(db, sqlUpdate, -1, &updateStmt, NULL);

	for (const std::pair<std::string, glm::vec2>& p : list) {

		// Try to update wind data for cell
		sqlite3_bind_double(updateStmt, 1, p.second.x);
		sqlite3_bind_double(updateStmt, 2, p.second.y);
		sqlite3_bind_text(updateStmt, 3, p.first.c_str(), -1, SQLITE_STATIC);
		sqlite3_step(updateStmt);

		// If no rows update then cell not in DB, insert instead
		int rows = sqlite3_changes(db);
		if (rows == 0) {
			sqlite3_bind_text(insertStmt, 1, p.first.c_str(), -1, SQLITE_STATIC);
			sqlite3_bind_double(insertStmt, 2, p.second.x);
			sqlite3_bind_double(insertStmt, 3, p.second.y);
			sqlite3_step(insertStmt);
		}

		// Clear bindings and reset SQL statements
		sqlite3_clear_bindings(insertStmt);
		sqlite3_clear_bindings(updateStmt);
		sqlite3_reset(insertStmt);
		sqlite3_reset(updateStmt);
	}

	sqlite3_finalize(insertStmt);
	sqlite3_finalize(updateStmt);
	sqlite3_exec(db, "END TRANSACTION", NULL, NULL, NULL);
}


// Gets all AirSigmets from the DB along with their interior and boundary cells
//
// out - output list of AirSigmets and their associated interior and boundary cells - treats as empty
void SdogDB::getAirSigmetCells(std::vector<AirSigmetCells>& out) {

	// Get all cells from DB that are associate with an AirSigmet
	char* sqlCells = "SELECT c.code, r.boundary, r.airSigmetID FROM cells as c, cellHasAirSigmet as r WHERE c.cellID = r.cellID";
	sqlite3_stmt* cellsStmt;
	sqlite3_prepare_v2(db, sqlCells, -1, &cellsStmt, NULL);

	// Store each cell in list for boundary or interior cells appropriately
	std::vector<std::pair<std::string, int>> boundary, interior;
	while (sqlite3_step(cellsStmt) != SQLITE_DONE) {

		std::string code = std::string(reinterpret_cast<const char*>(sqlite3_column_text(cellsStmt, 0)));
		bool bound = sqlite3_column_int(cellsStmt, 1);

		if (bound) {
			boundary.push_back(std::pair<std::string, int>(code, sqlite3_column_int(cellsStmt, 2)));
		}
		else {
			interior.push_back(std::pair<std::string, int>(code, sqlite3_column_int(cellsStmt, 2)));
		}
	}
	sqlite3_finalize(cellsStmt);

	// Get all AirSigmets from DB
	char* sqlAirSigs = "SELECT airSigmetID, validFrom, validUntil, minAltKM, maxAltKM, dirDeg, speedKT, hazard, severity, type FROM airSigmets";
	sqlite3_stmt* airSigsStmt;
	sqlite3_prepare_v2(db, sqlAirSigs, -1, &airSigsStmt, NULL);

	// Store each AirSigmet in a hash map with its ID as a key
	std::unordered_map<int, AirSigmetCells> airSigs;
	while (sqlite3_step(airSigsStmt) != SQLITE_DONE) {

		AirSigmetCells a;
		int airSigID = sqlite3_column_int(airSigsStmt, 0);

		a.airSigmet.validFrom = std::string(reinterpret_cast<const char*>(sqlite3_column_text(airSigsStmt, 1)));
		a.airSigmet.validUntil = std::string(reinterpret_cast<const char*>(sqlite3_column_text(airSigsStmt, 2)));
		a.airSigmet.minAltKM = sqlite3_column_double(airSigsStmt, 3);
		a.airSigmet.maxAltKM = sqlite3_column_double(airSigsStmt, 4);
		a.airSigmet.dirDeg = sqlite3_column_int(airSigsStmt, 5);
		a.airSigmet.speedKT = sqlite3_column_int(airSigsStmt, 6);
		a.airSigmet.hazard = (HazardType)sqlite3_column_int(airSigsStmt, 7);
		a.airSigmet.severity = (Severity)sqlite3_column_int(airSigsStmt, 8);
		a.airSigmet.type = (AirSigmetType)sqlite3_column_int(airSigsStmt, 9);

		char* sqlBounds = "SELECT b.latRad, b.longRad FROM airSigmets as a, airSigmetBounds as b WHERE b.airSigmetID = @ID";
		sqlite3_stmt* boundsStmt;
		sqlite3_prepare_v2(db, sqlBounds, -1, &boundsStmt, NULL);
		sqlite3_bind_int(boundsStmt, 1, airSigID);

		// Get bounds of the AirSigmet polygon
		while (sqlite3_step(boundsStmt) != SQLITE_DONE) {

			SphCoord c(sqlite3_column_double(boundsStmt, 0), sqlite3_column_double(boundsStmt, 1));
			a.airSigmet.polygon.push_back(c);
		}
		airSigs[airSigID] = a;
		sqlite3_finalize(boundsStmt);
	}
	sqlite3_finalize(airSigsStmt);

	// Associate each boudary and interior cell with the appropriate AirSigmet
	for (const std::pair<std::string, int>& pair : boundary) {
		airSigs[pair.second].boundary.push_back(pair.first);
	}
	for (const std::pair<std::string, int>& pair : interior) {
		airSigs[pair.second].interior.push_back(pair.first);
	}

	// Add all AirSigmets to the output list
	for (const std::pair<int, AirSigmetCells>& pair : airSigs) {
		out.push_back(pair.second);
	}
}


// Gets all cells with wind information from the DB
//
// out - output list of cells and their associated wind data - treats as empty
void SdogDB::getWindCells(std::vector<std::pair<std::string, glm::vec2>>& out) {

	char* sql = "SELECT code, windU, windV FROM cells WHERE windU IS NOT NULL";
	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

	while (sqlite3_step(stmt) != SQLITE_DONE) {

		std::string code = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
		glm::vec3 vec;
		vec.x = sqlite3_column_double(stmt, 1);
		vec.y = sqlite3_column_double(stmt, 2);

		out.push_back(std::pair<std::string, glm::vec2>(code, vec));
	}
	sqlite3_finalize(stmt);
}


// Inserts the given code into the DB
//
// code - code of the cell to insert
void SdogDB::insertCell(const std::string& code) {

	char* sql = "INSERT OR IGNORE INTO cells(code) VALUES (@CO)";
	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

	sqlite3_bind_text(stmt, 1, code.c_str(), -1, SQLITE_STATIC);
	sqlite3_step(stmt);
	sqlite3_finalize(stmt);
}


// Inserts a list of codes into the DB
//
// codes - vector of codes for cells to insert
void SdogDB::insertCells(const std::vector<std::string>& codes) {

	sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, NULL);
	char* sql = "INSERT OR IGNORE INTO cells(code) VALUES (@CO)";
	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

	for (const std::string& code : codes) {

		sqlite3_bind_text(stmt, 1, code.c_str(), -1, SQLITE_STATIC);
		sqlite3_step(stmt);

		sqlite3_clear_bindings(stmt);
		sqlite3_reset(stmt);
	}
	sqlite3_finalize(stmt);
	sqlite3_exec(db, "END TRANSACTION", NULL, NULL, NULL);
}

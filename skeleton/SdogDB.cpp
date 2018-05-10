#define _USE_MATH_DEFINES
#include "SdogDB.h"

#include <cmath>
#include <algorithm>

#include "SdogCell.h"


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
SdogDB::SdogDB(const std::string& path, double radius) {

	sqlite3_open(path.c_str(), &db);

	char* tab0 = "CREATE TABLE info(lock INTEGER, gridRadius REAL,"
		         "PRIMARY KEY(Lock), CONSTRAINT info_locked CHECK(lock = 0) )";
	char* tab1 = "CREATE TABLE cells (cellID INTEGER PRIMARY KEY AUTOINCREMENT, code TEXT NOT NULL UNIQUE);";
	char* tab2 = "CREATE TABLE airSigmet (airSigmetID INTEGER PRIMARY KEY AUTOINCREMENT)";
	char* tab3 = "CREATE TABLE cell_airSigmet (cellID INTEGER, airSigmetID INTEGER, boundary INTEGER,"
		         "PRIMARY KEY(cellID, airSigmetID),"
		         "FOREIGN KEY(airSigmetID) REFERENCES airSigmet(airSigmetID),"
		         "FOREIGN KEY(cellID) REFERENCES cells(cellID) )";

	sqlite3_exec(db, tab0, NULL, NULL, NULL);
	sqlite3_exec(db, tab1, NULL, NULL, NULL);
	sqlite3_exec(db, tab2, NULL, NULL, NULL);
	sqlite3_exec(db, tab3, NULL, NULL, NULL);

	char* config = "INSERT OR IGNORE INTO info(lock, gridRadius) VALUES (0, @RA)";
	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(db, config, -1, &stmt, NULL);

	sqlite3_bind_double(stmt, 1, radius);
	sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	insertCell("0");
	insertCell("1");
	insertCell("2");
	insertCell("3");
	insertCell("4");
	insertCell("5");
	insertCell("6");
	insertCell("7");


	std::vector<std::string> test1;
	test1.push_back("i1");
	test1.push_back("i2");
	test1.push_back("i3");
	test1.push_back("i4");
	test1.push_back("i5");

	std::vector<std::string> test2;
	test2.push_back("b1");
	test2.push_back("b2");
	test2.push_back("b3");
	test2.push_back("b4");
	test2.push_back("b5");
	insertAirSigmet(test1, test2);
}


// Ends connection with the database
SdogDB::~SdogDB() {
	sqlite3_close(db);
}


// Inserts a new AIRMET/SIGMET into the data base with the provided interior and boundary cells
//
// interior - list of cells that are completely inside the SIGMET/AIRMET
// boundary - list of cells that are on the boundary of the SIGMET/AIRMET
void SdogDB::insertAirSigmet(const std::vector<std::string>& interior, const std::vector<std::string>& boundary) {

	sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, NULL);
	sqlite3_exec(db, "INSERT INTO airSigmet DEFAULT VALUES", NULL, NULL, NULL);
	int airSigmetID = sqlite3_last_insert_rowid(db);

	char* sqlInsert = "INSERT INTO cell_airSigmet(cellID, airSigmetID, boundary) VALUES (@CO, @AS, @BO)";
	char* sqlSelect = "SELECT cellID FROM cells WHERE code = @CO;";

	sqlite3_stmt *insertStmt, *selectStmt;
	sqlite3_prepare_v2(db, sqlInsert, -1, &insertStmt, NULL);
	sqlite3_prepare_v2(db, sqlSelect, -1, &selectStmt, NULL);

	// Interior cells
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

		// Insert cell data relation into DB
		sqlite3_bind_int(insertStmt, 1, cellID);
		sqlite3_bind_int(insertStmt, 2, airSigmetID);
		sqlite3_bind_int(insertStmt, 3, false);
		sqlite3_step(insertStmt);

		// Clear bindings and reset SQL statements
		sqlite3_clear_bindings(selectStmt);
		sqlite3_clear_bindings(insertStmt);
		sqlite3_reset(selectStmt);
		sqlite3_reset(insertStmt);
	}

	// Boundary cells
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

		// Insert cell data relation into DB
		sqlite3_bind_int(insertStmt, 1, cellID);
		sqlite3_bind_int(insertStmt, 2, airSigmetID);
		sqlite3_bind_int(insertStmt, 3, true);
		sqlite3_step(insertStmt);

		// Clear bindings and reset SQL statements
		sqlite3_clear_bindings(selectStmt);
		sqlite3_clear_bindings(insertStmt);
		sqlite3_reset(selectStmt);
		sqlite3_reset(insertStmt);
	}

	sqlite3_finalize(insertStmt);
	sqlite3_finalize(selectStmt);

	sqlite3_exec(db, "END TRANSACTION", NULL, NULL, NULL);
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


// Helper for inserting AIRMET/SIGMET data. 
void insertAirSigmetHelper(const std::vector<std::string>& cells, bool boundary) {

}
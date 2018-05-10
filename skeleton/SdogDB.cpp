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
		         "PRIMARY KEY(Lock), CONSTRAINT info_locked CHECK(lock = 0))";
	char* tab1 = "CREATE TABLE cells (cellID INTEGER PRIMARY KEY AUTOINCREMENT, code TEXT NOT NULL UNIQUE);";
	char* tab2 = "CREATE TABLE airSigmet (airSigmetID INTEGER PRIMARY KEY AUTOINCREMENT)";
	char* tab3 = "CREATE TABLE cell_airSigmet (cellID INTEGER, airSigmetID INTEGER,"
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
}


// Ends connection with the database
SdogDB::~SdogDB() {
	sqlite3_close(db);
}


// Inserts a new AIRMET/SIGMET into the data base with the provided interior and boundary cells
//
// interior - list of cells that are completely inside the SIGMET/AIRMET
// boundary - list of cells that are on the boundary of the SIGMET/AIRMET
void SdogDB::insertAirSigment(const std::vector<std::string>& interior, const std::vector<std::string>& boundary) {

}


// Inserts the given code into the DB
//
// code - code of the cell to insert
void SdogDB::insertCell(const std::string& code) {

	char* sqlEmpty = "INSERT OR IGNORE INTO cells(code) VALUES (@CO)";
	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(db, sqlEmpty, -1, &stmt, NULL);

	sqlite3_bind_text(stmt, 1, code.c_str(), -1, SQLITE_STATIC);
	sqlite3_step(stmt);
	sqlite3_finalize(stmt);
}


// Inserts a list of codes into the DB
//
// codes - vector of codes for cells to insert
void SdogDB::insertCells(const std::vector<std::string>& codes) {

	sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, NULL);
	char* sqlEmpty = "INSERT OR IGNORE INTO cells(code) VALUES (@CO)";
	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(db, sqlEmpty, -1, &stmt, NULL);

	for (const std::string& code : codes) {
		
		sqlite3_bind_text(stmt, 1, code.c_str(), -1, SQLITE_STATIC);
		sqlite3_step(stmt);

		sqlite3_clear_bindings(stmt);
		sqlite3_reset(stmt);
	}
	sqlite3_finalize(stmt);
	sqlite3_exec(db, "END TRANSACTION", NULL, NULL, NULL);
}
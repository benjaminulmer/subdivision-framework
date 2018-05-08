#define _USE_MATH_DEFINES
#include "SdogDB.h"

#include <cmath>
#include <algorithm>

#include "SdogCell.h"


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

	char* sql = "CREATE TABLE Cells (CellID INTEGER PRIMARY KEY AUTOINCREMENT, Code TEXT NOT NULL UNIQUE, State INTEGER DEFAULT 0);";
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
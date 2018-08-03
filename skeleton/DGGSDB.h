#pragma once

#include <string>
#include <SQLite\sqlite3.h>
#include <vector>

#include "Renderable.h"

class DGGS;

class DGGSDB {

public:
	DGGSDB(const std::string& path, DGGS* myDGGS, std::vector<double>* data, unsigned int ncols, unsigned int nrows, double xllcorner, double yllcorner, double cellsize, Renderable & r);
	~DGGSDB();

private:
	sqlite3 * db;
};


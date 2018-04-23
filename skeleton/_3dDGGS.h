#pragma once

#include <sqlite/sqlite3.h>

#include <string>
#include <vector>

class _3dGridCell {

public:

private:

};

class _3dDGGS {

public:
	virtual std::string codeForPos(double latRad, double longRad, double radius, int level) const = 0;
	virtual bool neighbours(const std::string& code, std::vector<std::string>& out) const  = 0;

protected:
	sqlite3* db;
};


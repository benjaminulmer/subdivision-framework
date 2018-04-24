#include "SDOG.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>

#include "Constants.h"
#include "Geometry.h"

SphCell::SphCell() :
	minLat(0.0), maxLat(0.0), minLong(0.0), maxLong(0.0), minRad(0.0), maxRad(0.0) {}

SphCell::SphCell(double minLat, double maxLat, double minLong, double maxLong, double minRad, double maxRad) :
	minLat(minLat), maxLat(maxLat), minLong(minLong), maxLong(maxLong), minRad(minRad), maxRad(maxRad) {}

CellType SphCell::type() const {
	if (minRad == 0.0) {
		return CellType::SG;
	}
	else if (maxLat == M_PI_2 || maxLat == -M_PI_2) {
		return CellType::LG;
	}
	else {
		return CellType::NG;
	}
}

void SphGrid::insertCell(const std::string& code, double minLat, double maxLat, double minLong, double maxLong, double minRad, double maxRad) {

	char* sqlEmpty = "insert or ignore into Cells (Code, MinLat, MaxLat, MinLong, MaxLong, MinRad, MaxRad) "
		             "values ('%s', %.17g, %.17g, %.17g, %.17g, %.17g, %.17g);";

	char* sqlFilled = sqlite3_mprintf(sqlEmpty, code.c_str(), minLat, maxLat, minLong, maxLong, minRad, maxRad);

	sqlite3_exec(db, sqlFilled, NULL, NULL, NULL);
	sqlite3_free(sqlFilled);


	//sqlite3_stmt* stmt;
	//sqlite3_prepare_v2(db, sqlFilled, -1, &stmt, NULL);
	//sqlite3_step(stmt);
	//sqlite3_finalize(stmt);
	
}

void SphGrid::insertCells(std::vector<std::pair<std::string, SphCell*>>& toAdd) {

	sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);

	char* sqlEmpty = "insert or ignore into Cells (Code, MinLat, MaxLat, MinLong, MaxLong, MinRad, MaxRad)\n"
		"values ('%s', %.17g, %.17g, %.17g, %.17g, %.17g, %.17g);\n";

	for (std::pair<std::string, SphCell*>& p : toAdd) {

		if (p.first == "5322") {
			std::cout << "br" << std::endl;
		}

		char* line = sqlite3_mprintf(sqlEmpty, p.first.c_str(), p.second->minLat, p.second->maxLat, p.second->minLong, p.second->maxLong, p.second->minRad, p.second->maxRad);
		sqlite3_exec(db, line, NULL, NULL, NULL);
		sqlite3_free(line);
	}
	sqlite3_exec(db, "END TRANSACTION;", NULL, NULL, NULL);
	 

	//sqlite3_stmt* stmt;
	//std::cout << sqlite3_prepare_v2(db, sqlFull.c_str(), -1, &stmt, NULL) << " ";
	//std::cout << sqlite3_step(stmt) << std::endl;
	//sqlite3_finalize(stmt);
}

SphGrid::SphGrid(double radius) : maxDepth(0), maxRadius(radius) {

	sqlite3_open("test1.db", &db);

	insertCell("0", 0.0, M_PI_2, 0.0, M_PI_2, 0.0, radius);
	insertCell("1", 0.0, M_PI_2, M_PI_2, M_PI, 0.0, radius);
	insertCell("2", 0.0, M_PI_2, 0.0, -M_PI_2, 0.0, radius);
	insertCell("3", 0.0, M_PI_2, -M_PI_2, -M_PI, 0.0, radius);

	insertCell("4", 0.0, -M_PI_2, 0.0, M_PI_2, 0.0, radius);
	insertCell("5", 0.0, -M_PI_2, M_PI_2, M_PI, 0.0, radius);
	insertCell("6", 0.0, -M_PI_2, 0.0, -M_PI_2, 0.0, radius);
	insertCell("7", 0.0, -M_PI_2, -M_PI_2, -M_PI, 0.0, radius);

	map["0"] = new SphCell(0.0, M_PI_2, 0.0, M_PI_2, 0.0, radius);
	map["1"] = new SphCell(0.0, M_PI_2, M_PI_2, M_PI, 0.0, radius);
	map["2"] = new SphCell(0.0, M_PI_2, 0.0, -M_PI_2, 0.0, radius);
	map["3"] = new SphCell(0.0, M_PI_2, -M_PI_2, -M_PI, 0.0, radius);

	map["4"] = new SphCell(0.0, -M_PI_2, 0.0, M_PI_2, 0.0, radius);
	map["5"] = new SphCell(0.0, -M_PI_2, M_PI_2, M_PI, 0.0, radius);
	map["6"] = new SphCell(0.0, -M_PI_2, 0.0, -M_PI_2, 0.0, radius);
	map["7"] = new SphCell(0.0, -M_PI_2, -M_PI_2, -M_PI, 0.0, radius);
}

void SphGrid::subdivideTo(int level) {

	while (maxDepth < level) {
		subdivide();
	}
}

void SphGrid::subdivide() {

	std::vector<std::pair<std::string, SphCell*>> toAdd;

	char* sqlEmpty = "select Code, MinLat, MaxLat, MinLong, MaxLong, MinRad, MaxRad from cells where length(Code) = %i;";
	char* sqlFilled = sqlite3_mprintf(sqlEmpty, maxDepth + 1);

	sqlite3_stmt* stmt;
	sqlite3_prepare_v2(db, sqlFilled, -1, &stmt, NULL);

	//while (sqlite3_step(stmt) != SQLITE_DONE) {

	//	SphCell i;
	//	i.minLat = sqlite3_column_double(stmt, 1);
	//	i.maxLat = sqlite3_column_double(stmt, 2);
	//	i.minLong = sqlite3_column_double(stmt, 3);
	//	i.maxLong = sqlite3_column_double(stmt, 4);
	//	i.minRad = sqlite3_column_double(stmt, 5);
	//	i.maxRad = sqlite3_column_double(stmt, 6);

	//	std::string code = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));

	//	subdivideCell(code, &i, toAdd);
	//}

	for (const std::pair<std::string, SphCell*>& p : map) {

		if (p.first.size() == maxDepth + 1 /*&& p.second->dataSets.size() != 0*/) {

			subdivideCell(p.first, p.second, toAdd);
		}
	}

	for (const std::pair<std::string, SphCell*>& p : toAdd) {
		map[p.first] = p.second;
		//insertCell(p.first, p.second->minLat, p.second->maxLat, p.second->minLong, p.second->maxLong, p.second->minRad, p.second->maxRad);
	}
	insertCells(toAdd);
	maxDepth++;


	//sqlite3_finalize(stmt);
	//sqlite3_free(sqlFilled);


	//for (const std::pair<std::string, SphCell*>& p : map) {

	//	if (p.first.size() == maxDepth + 1 /*&& p.second->dataSets.size() != 0*/) {

	//		subdivideCell(p.first, p.second, toAdd);
	//	}
	//}
	//for (const std::pair<std::string, SphCell*>& p : toAdd) {
	//	map[p.first] = p.second;
	//}
	//maxDepth++;



}

void SphGrid::subdivideCell(const std::string& code, const SphCell* cell, std::vector<std::pair<std::string, SphCell*>>& toAdd) {

	double midLat = 0.5 * cell->minLat + 0.5 * cell->maxLat;
	double midLong = 0.5 * cell->minLong + 0.5 * cell->maxLong;
	double midRad = 0.5 * cell->minRad + 0.5 * cell->maxRad;

	if (cell->type() == CellType::NG) {

		toAdd.push_back(std::pair<std::string, SphCell*>(code + "0", new SphCell(cell->minLat, midLat, cell->minLong, midLong, midRad, cell->maxRad)));
		toAdd.push_back(std::pair<std::string, SphCell*>(code + "1", new SphCell(cell->minLat, midLat, midLong, cell->maxLong, midRad, cell->maxRad)));
		toAdd.push_back(std::pair<std::string, SphCell*>(code + "2", new SphCell(midLat, cell->maxLat, cell->minLong, midLong, midRad, cell->maxRad)));
		toAdd.push_back(std::pair<std::string, SphCell*>(code + "3", new SphCell(midLat, cell->maxLat, midLong, cell->maxLong, midRad, cell->maxRad)));

		toAdd.push_back(std::pair<std::string, SphCell*>(code + "4", new SphCell(cell->minLat, midLat, cell->minLong, midLong, cell->minRad, midRad)));
		toAdd.push_back(std::pair<std::string, SphCell*>(code + "5", new SphCell(cell->minLat, midLat, midLong, cell->maxLong, cell->minRad, midRad)));
		toAdd.push_back(std::pair<std::string, SphCell*>(code + "6", new SphCell(midLat, cell->maxLat, cell->minLong, midLong, cell->minRad, midRad)));
		toAdd.push_back(std::pair<std::string, SphCell*>(code + "7", new SphCell(midLat, cell->maxLat, midLong, cell->maxLong, cell->minRad, midRad)));
	}
	else if (cell->type() == CellType::LG) {

		toAdd.push_back(std::pair<std::string, SphCell*>(code + "0", new SphCell(cell->minLat, midLat, cell->minLong, midLong, midRad, cell->maxRad)));
		toAdd.push_back(std::pair<std::string, SphCell*>(code + "1", new SphCell(cell->minLat, midLat, midLong, cell->maxLong, midRad, cell->maxRad)));
		toAdd.push_back(std::pair<std::string, SphCell*>(code + "2", new SphCell(midLat, cell->maxLat, cell->minLong, cell->maxLong, midRad, cell->maxRad)));

		toAdd.push_back(std::pair<std::string, SphCell*>(code + "3", new SphCell(cell->minLat, midLat, cell->minLong, midLong, cell->minRad, midRad)));
		toAdd.push_back(std::pair<std::string, SphCell*>(code + "4", new SphCell(cell->minLat, midLat, midLong, cell->maxLong, cell->minRad, midRad)));
		toAdd.push_back(std::pair<std::string, SphCell*>(code + "5", new SphCell(midLat, cell->maxLat, cell->minLong, cell->maxLong, cell->minRad, midRad)));
	}
	else {// cell.type() == CellType::SG

		toAdd.push_back(std::pair<std::string, SphCell*>(code + "0", new SphCell(cell->minLat, midLat, cell->minLong, midLong, midRad, cell->maxRad)));
		toAdd.push_back(std::pair<std::string, SphCell*>(code + "1", new SphCell(cell->minLat, midLat, midLong, cell->maxLong, midRad, cell->maxRad)));
		toAdd.push_back(std::pair<std::string, SphCell*>(code + "2", new SphCell(midLat, cell->maxLat, cell->minLong, cell->maxLong, midRad, cell->maxRad)));

		toAdd.push_back(std::pair<std::string, SphCell*>(code + "3", new SphCell(cell->minLat, cell->maxLat, cell->minLong, cell->maxLong, cell->minRad, midRad)));
	}
}

void SphGrid::fillData(const SphericalData& data) {

	//const std::vector<SphericalDatum>& dataPoints = data.getData();
	//const DataSetInfo& info = data.getInfo();

	//for (const SphericalDatum& d : dataPoints) {
	//	
	//	std::string code = codeForPos(d.latitude, d.longitude, d.radius, maxDepth);
	//	SphCell* c = map[code];

	//	// Find index in list of data sets for the point
	//	int index = -1;
	//	int i = 0;
	//	for (DataPoints& ds : c->dataSets) {

	//		if (ds.info.id == info.id) {
	//			index = i;
	//			break;
	//		}
	//		i++;
	//	}

	//	// If data point belongs to new data set add a new data set
	//	if (index == -1) {
	//		c->dataSets.push_back(DataPoints(info));
	//		index = c->dataSets.size() - 1;
	//	}
	//	c->dataSets[index].data.push_back(d);
	//}
}

void SphGrid::createRenderable(Renderable& r, int level) {

	//for (std::pair<std::string, SphCell*> p : map) {

	//	if (p.first.size() == level + 1 && p.second->dataSets.size() != 0) {

	//		double minLong = p.second->minLong; double maxLong = p.second->maxLong;
	//		double minLat = p.second->minLat; double maxLat = p.second->maxLat;
	//		double minRad = p.second->minRad; double maxRad = p.second->maxRad;

	//		r.drawMode = GL_LINES;
	//		glm::vec3 origin(0.f, 0.f, 0.f);

	//		// Outer points
	//		glm::vec3 o1 = glm::vec3(sin(minLong)*cos(minLat), sin(minLat), cos(minLong)*cos(minLat)) * (float)maxRad;
	//		glm::vec3 o2 = glm::vec3(sin(maxLong)*cos(minLat), sin(minLat), cos(maxLong)*cos(minLat)) * (float)maxRad;
	//		glm::vec3 o3 = glm::vec3(sin(minLong)*cos(maxLat), sin(maxLat), cos(minLong)*cos(maxLat)) * (float)maxRad;
	//		glm::vec3 o4 = glm::vec3(sin(maxLong)*cos(maxLat), sin(maxLat), cos(maxLong)*cos(maxLat)) * (float)maxRad;

	//		// Inner points
	//		glm::vec3 i1 = glm::vec3(sin(minLong)*cos(minLat), sin(minLat), cos(minLong)*cos(minLat)) * (float)minRad;
	//		glm::vec3 i2 = glm::vec3(sin(maxLong)*cos(minLat), sin(minLat), cos(maxLong)*cos(minLat)) * (float)minRad;
	//		glm::vec3 i3 = glm::vec3(sin(minLong)*cos(maxLat), sin(maxLat), cos(minLong)*cos(maxLat)) * (float)minRad;
	//		glm::vec3 i4 = glm::vec3(sin(maxLong)*cos(maxLat), sin(maxLat), cos(maxLong)*cos(maxLat)) * (float)minRad;

	//		// Straight lines connect each inner point to coresponding outer point
	//		Geometry::createLineR(i1, o1, r);
	//		Geometry::createLineR(i2, o2, r);
	//		Geometry::createLineR(i3, o3, r);
	//		Geometry::createLineR(i4, o4, r);

	//		// Great circle arcs connect points on same longtitude line
	//		Geometry::createArcR(o1, o3, origin, r);
	//		Geometry::createArcR(o2, o4, origin, r);
	//		Geometry::createArcR(i1, i3, origin, r);
	//		Geometry::createArcR(i2, i4, origin, r);

	//		// Small circle arcs connect points on same latitude line
	//		Geometry::createArcR(o1, o2, glm::vec3(0.f, sin(minLat), 0.f) * (float)maxRad, r);
	//		Geometry::createArcR(o3, o4, glm::vec3(0.f, sin(maxLat), 0.f) * (float)maxRad, r);
	//		Geometry::createArcR(i1, i2, glm::vec3(0.f, sin(minLat), 0.f) * (float)minRad, r);
	//		Geometry::createArcR(i3, i4, glm::vec3(0.f, sin(maxLat), 0.f) * (float)minRad, r);
	//	}
	//}
}

void SphGrid::createRenderable(Renderable& r, std::vector<std::string>& codes) {

	for (const std::string& code : codes) {

		SphCellInfo i;
		cellInfoFromCode(code, i);

		SphCell* ii = map[code];

		char* sqlEmpty = "select MinLat, MaxLat, MinLong, MaxLong, MinRad, MaxRad from cells where Code = %s;";
		char* sqlFilled = sqlite3_mprintf(sqlEmpty, code);

		sqlite3_stmt* stmt;
		sqlite3_prepare_v2(db, sqlFilled, -1, &stmt, NULL);
		sqlite3_step(stmt);

		double minLat = sqlite3_column_double(stmt, 0);
		double maxLat = sqlite3_column_double(stmt, 1);
		double minLong = sqlite3_column_double(stmt, 2);
		double maxLong = sqlite3_column_double(stmt, 3);
		double minRad = sqlite3_column_double(stmt, 4);
		double maxRad = sqlite3_column_double(stmt, 5);

		double mminLat = i.minLat; double mmaxLat = i.maxLat;
		double mminLong = i.minLong; double mmaxLong = i.maxLong;
		double mminRad = i.minRad; double mmaxRad = i.maxRad;

		std::cout << "\n\n" << code << std::endl;
		std::cout << minLat << " : " << mminLat << " : " << ii->minLat << std::endl;
		std::cout << maxLat << " : " << mmaxLat << " : " << ii->maxLat << std::endl;
		std::cout << minLong << " : " << mminLong << " : " << ii->minLong << std::endl;
		std::cout << maxLong << " : " << mmaxLong << " : " << ii->maxLong << std::endl;
		std::cout << minRad << " : " << mminRad << " : " << ii->minRad << std::endl;
		std::cout << maxRad << " : " << mmaxRad << " : " << ii->maxRad << std::endl;

		sqlite3_finalize(stmt);
		//SphCell* cell = map[code];
		//double minLong = cell->minLong; double maxLong = cell->maxLong;
		//double minLat = cell->minLat; double maxLat = cell->maxLat;
		//double minRad = cell->minRad; double maxRad = cell->maxRad;

		r.drawMode = GL_LINES;
		glm::vec3 origin(0.f, 0.f, 0.f);

		// Outer points
		glm::vec3 o1 = glm::vec3(sin(minLong)*cos(minLat), sin(minLat), cos(minLong)*cos(minLat)) * (float)maxRad;
		glm::vec3 o2 = glm::vec3(sin(maxLong)*cos(minLat), sin(minLat), cos(maxLong)*cos(minLat)) * (float)maxRad;
		glm::vec3 o3 = glm::vec3(sin(minLong)*cos(maxLat), sin(maxLat), cos(minLong)*cos(maxLat)) * (float)maxRad;
		glm::vec3 o4 = glm::vec3(sin(maxLong)*cos(maxLat), sin(maxLat), cos(maxLong)*cos(maxLat)) * (float)maxRad;

		// Inner points
		glm::vec3 i1 = glm::vec3(sin(minLong)*cos(minLat), sin(minLat), cos(minLong)*cos(minLat)) * (float)minRad;
		glm::vec3 i2 = glm::vec3(sin(maxLong)*cos(minLat), sin(minLat), cos(maxLong)*cos(minLat)) * (float)minRad;
		glm::vec3 i3 = glm::vec3(sin(minLong)*cos(maxLat), sin(maxLat), cos(minLong)*cos(maxLat)) * (float)minRad;
		glm::vec3 i4 = glm::vec3(sin(maxLong)*cos(maxLat), sin(maxLat), cos(maxLong)*cos(maxLat)) * (float)minRad;

		// Straight lines connect each inner point to coresponding outer point
		Geometry::createLineR(i1, o1, r);
		Geometry::createLineR(i2, o2, r);
		Geometry::createLineR(i3, o3, r);
		Geometry::createLineR(i4, o4, r);

		// Great circle arcs connect points on same longtitude line
		Geometry::createArcR(o1, o3, origin, r);
		Geometry::createArcR(o2, o4, origin, r);
		Geometry::createArcR(i1, i3, origin, r);
		Geometry::createArcR(i2, i4, origin, r);

		// Small circle arcs connect points on same latitude line
		Geometry::createArcR(o1, o2, glm::vec3(0.f, sin(minLat), 0.f) * (float)maxRad, r);
		Geometry::createArcR(o3, o4, glm::vec3(0.f, sin(maxLat), 0.f) * (float)maxRad, r);
		Geometry::createArcR(i1, i2, glm::vec3(0.f, sin(minLat), 0.f) * (float)minRad, r);
		Geometry::createArcR(i3, i4, glm::vec3(0.f, sin(maxLat), 0.f) * (float)minRad, r);
	}

}

std::string SphGrid::codeForPos(double latRad, double longRad, double radius, int level) {

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
	CellType curType = CellType::SG;
	for (int i = 0; i < level; i++) {

		int childCode = 0;
		double midLat = 0.5 * minLat + 0.5 * maxLat;
		double midLong = 0.5 * minLong + 0.5 * maxLong;
		double midRad = 0.5 * minRad + 0.5 * maxRad;

		if (curType == CellType::NG) {

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
		else if (curType == CellType::LG) {

			if (radius > midRad) {
				minRad = midRad;
			}
			else {
				maxRad = midRad;
				childCode += 3;
			}
			if (latRad < midLat) {
				maxLat = midLat;
				curType = CellType::NG;

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
		else {// curType == CellType::SG

			if (radius > midRad) {

				minRad = midRad;

				if (latRad < midLat) {
					maxLat = midLat;
					curType = CellType::NG;

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
					curType = CellType::LG;
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

bool SphGrid::cellInfoFromCode(const std::string& code, SphCellInfo& out) {

	out.minRad = 0.0;
	out.maxRad = maxRadius;

	// Set initial values based on which octanct cell is in
	if (code[0] == '0') {
		out.minLat = 0.0;
		out.maxLat = M_PI_2;
		out.minLong = 0.0;
		out.maxLong = M_PI_2;
	}
	else if (code[0] == '1') {
		out.minLat= 0.0;
		out.maxLat = M_PI_2;
		out.minLong = M_PI_2;
		out.maxLong = M_PI;
	}
	else if (code[0] == '2') {
		out.minLat = 0.0;
		out.maxLat = M_PI_2;
		out.minLong = 0.0;
		out.maxLong = -M_PI_2;
	}
	else if (code[0] == '3') {
		out.minLat = 0.0;
		out.maxLat = M_PI_2;
		out.minLong = -M_PI_2;
		out.maxLong = -M_PI;
	}
	else if (code[0] == '4') {
		out.minLat = 0.0;
		out.maxLat = -M_PI_2;
		out.minLong = 0.0;
		out.maxLong = M_PI_2;
	}
	else if (code[0] == '5') {
		out.minLat = 0.0;
		out.maxLat = -M_PI_2;
		out.minLong = M_PI_2;
		out.maxLong = M_PI;
	}
	else if (code[0] == '6') {
		out.minLat = 0.0;
		out.maxLat = -M_PI_2;
		out.minLong = 0.0;
		out.maxLong = -M_PI_2;
	}
	else if (code[0] == '7') {
		out.minLat = 0.0;
		out.maxLat = -M_PI_2;
		out.minLong = -M_PI_2;
		out.maxLong = -M_PI;
	}
	else {
		return false;
	}

	// Loop for each char in code and determine properties based on code
	out.type = CellType::SG;
	for (int i = 1; i < code.length(); i++) {

		double midLat = 0.5 * out.minLat + 0.5 * out.maxLat;
		double midLong = 0.5 * out.minLong + 0.5 * out.maxLong;
		double midRad = 0.5 * out.minRad + 0.5 * out.maxRad;

		if (out.type == CellType::NG) {

			if (code[i] == '0') {
				out.minRad = midRad;
				out.maxLat = midLat;
				out.maxLong = midLong;
			}
			else if (code[i] == '1') {
				out.minRad = midRad;
				out.maxLat = midLat;
				out.minLong = midLong;
			}
			else if (code[i] == '2') {
				out.minRad = midRad;
				out.minLat = midLat;
				out.maxLong = midLong;
			}
			else if (code[i] == '3') {
				out.minRad = midRad;
				out.minLat = midLat;
				out.minLong = midLong;
			}
			else if (code[i] == '4') {
				out.maxRad = midRad;
				out.maxLat = midLat;
				out.maxLong = midLong;
			}
			else if (code[i] == '5') {
				out.maxRad = midRad;
				out.maxLat = midLat;
				out.minLong = midLong;
			}
			else if (code[i] == '6') {
				out.maxRad = midRad;
				out.minLat = midLat;
				out.maxLong = midLong;
			}
			else if (code[i] == '7') {
				out.maxRad = midRad;
				out.minLat = midLat;
				out.minLong = midLong;
			}
			else {
				return false;
			}
			// type doesn't change
		}
		else if (out.type == CellType::LG) {

			if (code[i] == '0') {
				out.minRad = midRad;
				out.maxLat = midLat;
				out.maxLong = midLong;
				out.type = CellType::NG;
			}
			else if (code[i] == '1') {
				out.minRad = midRad;
				out.maxLat = midLat;
				out.minLong = midLong;
				out.type = CellType::NG;
			}
			else if (code[i] == '2') {
				out.minRad = midRad;
				out.minLat = midLat;
				// type doesn't change
			}
			else if (code[i] == '3') {
				out.maxRad = midRad;
				out.maxLat = midLat;
				out.maxLong = midLong;
				out.type = CellType::NG;
			}
			else if (code[i] == '4') {
				out.maxRad = midRad;
				out.maxLat = midLat;
				out.minLong = midLong;
				out.type = CellType::NG;
			}
			else if (code[i] == '5') {
				out.maxRad = midRad;
				out.minLat = midLat;
				// type doesn't change
			}
			else {
				return false;
			}
		}
		else {// out.type == CellType::SG

			if (code[i] == '0') {
				out.minRad = midRad;
				out.maxLat = midLat;
				out.maxLong = midLong;
				out.type = CellType::NG;
			}
			else if (code[i] == '1') {
				out.minRad = midRad;
				out.maxLat = midLat;
				out.minLong = midLong;
				out.type = CellType::NG;
			}
			else if (code[i] == '2') {
				out.minRad = midRad;
				out.minLat = midLat;
				out.type = CellType::LG;
			}
			else if (code[i] == '3') {
				out.maxRad = midRad;
				// type doesn't change
			}
			else {
				return false;
			}
		}
	}
	return true;
}

bool SphGrid::neighbours(const std::string& code, std::vector<std::string>& out) {

	SphCellInfo i;
	if (!cellInfoFromCode(code, i)) {
		return false;
	}
	int level = code.length() - 1;

	double midLat = 0.5 * i.minLat + 0.5 * i.maxLat;
	double midLong = 0.5 * i.minLong + 0.5 * i.maxLong;
	double midRad = 0.5 * i.minRad + 0.5 * i.maxRad;

	double latDist = i.maxLat - i.minLat;
	double longDist = i.maxLong - i.minLong;
	double radDist = i.maxRad - i.minRad;

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

	std::sort(out.begin(), out.end());
	out.erase(std::unique(out.begin(), out.end()), out.end());
	out.erase(std::remove(out.begin(), out.end(), code), out.end());


	return true;
}

const SphCell* SphGrid::getCell(const std::string& code) {
	//if (map.count(code) == 1) {
	//	return map[code];
	//}
	//else {
	//	return nullptr;
	//}
	return nullptr;
}
#include "DGGSDB.h"

#include <iostream>

#define _USE_MATH_DEFINES
#include <math.h>

#include "DGGS.h"
#include "d128Vec3.h"

DGGSDB::DGGSDB(const std::string& path, DGGS* myDGGS, std::vector<double>* data, unsigned int ncols, unsigned int nrows, double xllcorner, double yllcorner, double cellsize, Renderable & r) {
	
	sqlite3_open(path.c_str(), &db);

	char* myTable = "CREATE TABLE DGGS(Code INTEGER PRIMARY KEY, Data REAL)";

	int error = sqlite3_exec(db, myTable, NULL, NULL, NULL);

	std::cout << error << std::endl;

	unsigned int resolution = 7;
	unsigned int numCells = 120 * std::pow(4, resolution - 1) - 1;

	char* insert = "INSERT INTO DGGS (Code, Data) VALUES (@cod, @dat)";
	sqlite3_stmt* statement;
	sqlite3_prepare_v2(db, insert, -1, &statement, NULL);

	double minTheta = xllcorner * (M_PI / 180.0);
	double maxTheta = xllcorner * (M_PI / 180.0) + ((cellsize * (M_PI / 180.0)) * ncols);

	double minPhi = yllcorner * (M_PI / 180.0);
	double maxPhi = yllcorner * (M_PI / 180.0) + ((cellsize * (M_PI / 180.0)) * nrows);

	char* transaction = "BEGIN TRANSACTION";

	error = sqlite3_exec(db, transaction, NULL, NULL, NULL);

	std::cout << error << std::endl;

	for (unsigned int i = 0; i < numCells; i++) {
		d128Vec3 v1;
		d128Vec3 v2;
		d128Vec3 v3;

		myDGGS->cellToVerts(i, resolution, &v1, &v2, &v3);

		d128Vec3 centroid = v1 * (1.0 / 3.0) + v2 * (1.0 / 3.0) + v3 * (1.0 / 3.0);

		centroid.normalize();

		cgiDouble128NS::double128 theta = cgiDouble128NS::atan2(centroid.y, centroid.x);
		cgiDouble128NS::double128 phi = (M_PI / 2.0) - cgiDouble128NS::acos(centroid.z);


		//Check if cell is within the current data
		if (theta >= minTheta && theta <= maxTheta && phi >= minPhi && phi <= maxPhi) {
			double dataCol = cgiDouble128NS::floor((theta - minTheta) / (cellsize * (M_PI / 180.0)));
			double colDelta = (theta - minTheta) / (cellsize * (M_PI / 180.0)) - dataCol;
			double dataRow = cgiDouble128NS::floor((double)nrows - (phi - minPhi) / (cellsize * (M_PI / 180.0)));
			double rowDelta = (double)nrows - (phi - minPhi) / (cellsize * (M_PI / 180.0)) - dataRow;

			double data1 = (*data)[((int)dataRow * ncols + (int)dataCol) % data->size()];;
			double data2 = (*data)[((int)dataRow * ncols + (int)dataCol + 1) % data->size()];
			double data3 = (*data)[(((int)dataRow + 1) * ncols + (int)dataCol) % data->size()];
			double data4 = (*data)[(((int)dataRow + 1) * ncols + (int)dataCol + 1) % data->size()];

			double cellData = (1.0 - colDelta) * (1.0 - rowDelta) * data4 + (1.0 - colDelta) * rowDelta * data3 + colDelta * (1.0 - rowDelta) * data2 + colDelta * rowDelta * data1;

			//Insert data into grid
			sqlite3_bind_int(statement, 1, i);
			sqlite3_bind_double(statement, 2, cellData);

			sqlite3_step(statement);

			sqlite3_clear_bindings(statement);
			sqlite3_reset(statement);

			if (cellData < 0.0001) {
				cellData = 0.0;
			} else if (cellData < 1.0) {
				cellData = 0.1;
			} else if (cellData < 5.0) {
				cellData = 0.3;
			} else if (cellData < 25.0) {
				cellData = 0.5;
			} else if (cellData < 250.0) {
				cellData = 0.7;
			} else if (cellData < 1000.0) {
				cellData = 0.9;
			} else {
				cellData = 1.0;
			}

			r.normals.push_back(glm::vec3(cellData, 0.0, 0.0));
			r.normals.push_back(glm::vec3(cellData, 0.0, 0.0));
			r.normals.push_back(glm::vec3(cellData, 0.0, 0.0));

		} else {

			double cellData = 0.0;

			sqlite3_bind_int(statement, 1, i);
			sqlite3_bind_double(statement, 2, cellData);

			sqlite3_step(statement);

			sqlite3_clear_bindings(statement);
			sqlite3_reset(statement);

			r.normals.push_back(glm::vec3(0.0, 0.0, 0.0));
			r.normals.push_back(glm::vec3(0.0, 0.0, 0.0));
			r.normals.push_back(glm::vec3(0.0, 0.0, 0.0));
		}
	}

	sqlite3_finalize(statement);

	char* commit = "COMMIT TRANSACTION";

	error = sqlite3_exec(db, commit, NULL, NULL, NULL);

	std::cout << error << std::endl;
}

DGGSDB::~DGGSDB() {
	sqlite3_close(db);
}

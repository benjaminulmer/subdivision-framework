#include "ContentReadWrite.h"

// Load obj file from path into indexed list data structure
bool ContentReadWrite::loadOBJ(const char* path, Renderable& r) {
	printf("Loading OBJ file %s...\n", path);

	FILE * file; 
	file = fopen(path, "r");
	if( file == NULL ){
		printf("Cannot open file. Check path.");
		getchar();
		return false;
	}

	std::vector<unsigned int> vertIndices;
	std::vector<unsigned int> normalIndices;

	std::vector<glm::vec3> verts;
	std::vector<glm::vec3> normals;

	while(true){

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader
		
		if ( strcmp( lineHeader, "v" ) == 0 ){
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
			verts.push_back(vertex);
		}else if ( strcmp( lineHeader, "vt" ) == 0 ){
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y );
			uv.y = -uv.y; 
			//uvs.push_back(uv);
		}else if ( strcmp( lineHeader, "vn" ) == 0 ){
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
			normals.push_back(normal);
		}else if ( strcmp( lineHeader, "f" ) == 0 ){
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );

			vertIndices.push_back(vertexIndex[0]-1);
			vertIndices.push_back(vertexIndex[1]-1);
			vertIndices.push_back(vertexIndex[2]-1);
			normalIndices.push_back(normalIndex[0]-1);
			normalIndices.push_back(normalIndex[1]-1);
			normalIndices.push_back(normalIndex[2]-1);

		}else{
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}
	
	for( unsigned int i=0; i<vertIndices.size(); i++ ){

		// Get the indices of its attributes
		unsigned int vertexIndex = vertIndices[i];
		unsigned int normalIndex = normalIndices[i];

		// Get the attributes thanks to the index
		glm::vec3 vertex = verts[ vertexIndex ];
		glm::vec3 normal = normals[ normalIndex ];

		// Put the attributes in buffers
		r.verts.push_back(vertex);
		r.normals.push_back(normal);
	}
	r.drawMode = GL_TRIANGLES;
	fclose(file);
	return true;
}

std::vector<double>* ContentReadWrite::loadPopulationData(const char * path, unsigned int* ncols, unsigned int* nrows, double* xllcorner, double* yllcorner, double* cellsize) {

	//Parse the data file
	int NODATA_value = 0;

	FILE * file;
	file = fopen(path, "r");
	if (file == NULL) {
		printf("Cannot open file. Check path.");
		getchar();
		return false;
	}

	char lineHeader[128];

	// read ncols
	int res = fscanf(file, "%s", lineHeader);
	if (res == EOF) {
		std::cout << "Missing ncols, invalid ESRI ASCII format." << std::endl;
		exit(1); // EOF = End Of File. Invalid format.
	}
	// else : parse lineHeader

	if (strcmp(lineHeader, "ncols") == 0) {
		fscanf(file, "%d\n", ncols);
	}

	// read nrows
	res = fscanf(file, "%s", lineHeader);
	if (res == EOF) {
		std::cout << "Missing nrows, invalid ESRI ASCII format." << std::endl;
		exit(1); // EOF = End Of File. Invalid format.
	}
	// else : parse lineHeader

	if (strcmp(lineHeader, "nrows") == 0) {
		fscanf(file, "%d\n", nrows);
	}

	// read xllcorner
	res = fscanf(file, "%s", lineHeader);
	if (res == EOF) {
		std::cout << "Missing xllcorner, invalid ESRI ASCII format." << std::endl;
		exit(1); // EOF = End Of File. Invalid format.
	}
	// else : parse lineHeader

	if (strcmp(lineHeader, "xllcorner") == 0) {
		fscanf(file, "%lg\n", xllcorner);
	}

	// read yllcorner
	res = fscanf(file, "%s", lineHeader);
	if (res == EOF) {
		std::cout << "Missing yllcorner, invalid ESRI ASCII format." << std::endl;
		exit(1); // EOF = End Of File. Invalid format.
	}
	// else : parse lineHeader

	if (strcmp(lineHeader, "yllcorner") == 0) {
		fscanf(file, "%lg\n", yllcorner);
	}

	// read cellsize
	res = fscanf(file, "%s", lineHeader);
	if (res == EOF) {
		std::cout << "Missing cellsize, invalid ESRI ASCII format." << std::endl;
		exit(1); // EOF = End Of File. Invalid format.
	}
	// else : parse lineHeader

	if (strcmp(lineHeader, "cellsize") == 0) {
		fscanf(file, "%lg\n", cellsize);
	}

	// read NODATA_VALUE
	res = fscanf(file, "%s", lineHeader);
	if (res == EOF) {
		std::cout << "Missing NODATA_value, invalid ESRI ASCII format." << std::endl;
		exit(1); // EOF = End Of File. Invalid format.
	}
	// else : parse lineHeader

	if (strcmp(lineHeader, "NODATA_value") == 0) {
		fscanf(file, "%d\n", &NODATA_value);
	}

	std::vector<double>* data = new std::vector<double>((*ncols)*(*nrows));
	double tempDouble = 0.0;
	unsigned int counter = 0;
	while (true) {
		if (fscanf(file, "%lg", &tempDouble) != EOF) {
			if (round(tempDouble) == NODATA_value) {
				(*data)[counter] = 0.0;
				counter++;
			} else {
				(*data)[counter] = tempDouble;
				counter++;
			}
			
		} else {
			break; //Invalid file format?
		}
	}

	return data;
}

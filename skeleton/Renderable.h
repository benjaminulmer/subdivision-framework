#pragma once

#define _USE_MATH_DEFINES

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <vector>

#include <iostream>
#include <fstream>
#include <string>

struct IndexedLists {
	std::vector<GLushort> vertIndices;
	std::vector<GLushort> normalIndices;
	std::vector<GLushort> uvIndices;

	std::vector<glm::vec3> verts;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
};

class Renderable {

public:
	Renderable();
	~Renderable();

	void outputFaceAreas();

	std::vector<glm::vec3> verts;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;

	GLuint vao;
	GLuint vertexBuffer;
	GLuint normalBuffer;
	GLuint uvBuffer;
	glm::vec3 colour;

	GLuint drawMode;
};


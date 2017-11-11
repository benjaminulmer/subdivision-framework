#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <vector>

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
	Renderable() : vao(0), vertexBuffer(0), colourBuffer(0), fade(false), drawMode(0) {}

	std::vector<glm::vec3> verts;
	std::vector<glm::vec3> colours;

	glm::mat4 model;
	glm::vec3 lineColour;

	GLuint vao;
	GLuint vertexBuffer;
	GLuint colourBuffer;

	bool fade;

	GLuint drawMode;
};


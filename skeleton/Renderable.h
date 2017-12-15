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
	Renderable() : lineColour(glm::vec3(1.f, 1.f, 1.f)), vao(0), vertexBuffer(0), colourBuffer(0), fade(false), drawMode(0) {}

	glm::mat4 model() const {
		return trans * rot * scale;
	}

	std::vector<glm::vec3> verts;
	std::vector<glm::vec3> colours;
	std::vector<glm::vec2> uvs;

	glm::mat4 scale;
	glm::mat4 rot;
	glm::mat4 trans;

	glm::vec3 lineColour;

	GLuint vao;
	GLuint vertexBuffer;
	GLuint colourBuffer;
	GLuint uvBuffer;
	GLuint textureID;

	bool fade;

	GLuint drawMode;
};


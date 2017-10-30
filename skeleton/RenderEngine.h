#pragma once

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "Renderable.h"
#include "ShaderTools.h"
#include "Camera.h"

class RenderEngine {

public:
	RenderEngine(SDL_Window* window, Camera* camera);

	void render(const std::vector<Renderable*>& objects, glm::mat4 view);

	static void assignBuffers(Renderable& renderable);
	static void setBufferData(Renderable& renderable);
	static void deleteBufferData(Renderable& renderable);

	void setWindowSize(int newWidth, int newHeight);
	void updateLightPos(glm::vec3 add);

private:
	SDL_Window* window;
	int width, height;

	GLuint oneColourProgram;
	GLuint manyColoursProgram;

	Camera* camera;
	glm::mat4 view;
	glm::mat4 projection;
	glm::vec3 lightPos;
};


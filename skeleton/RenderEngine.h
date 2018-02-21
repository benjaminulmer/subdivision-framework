#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>

#include <vector>

#include "Renderable.h"
#include "Camera.h"

class RenderEngine {

public:
	RenderEngine(SDL_Window* window, Camera* camera);

	void render(const std::vector<Renderable*>& objects, const glm::mat4& view, float max, float min);

	static void assignBuffers(Renderable& renderable, bool texture);
	static void setBufferData(Renderable& renderable, bool texture);
	static void deleteBufferData(Renderable& renderable, bool texture);
	static GLuint loadTexture(const std::string& filename);

	void setWindowSize(int newWidth, int newHeight);
	void toggleFade() { fade = !fade; }

private:
	SDL_Window* window;
	int width, height;

	GLuint mainProgram;
	bool fade;

	Camera* camera;
	glm::mat4 view;
	glm::mat4 projection;
};


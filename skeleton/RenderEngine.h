#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>

#include <vector>

#include "Renderable.h"
#include "Camera.h"

class RenderEngine {

public:
	RenderEngine(SDL_Window* window);

	void render(const std::vector<const Renderable*>& objects, const glm::mat4& view, float max, float min);

	static void assignBuffers(Renderable& renderable, bool texture);
	static void setBufferData(Renderable& renderable, bool texture);
	static void deleteBufferData(Renderable& renderable, bool texture);
	static GLuint loadTexture(const std::string& filename);

	void setWindowSize(int newWidth, int newHeight);
	void toggleFade() { fade = !fade; }

	float getFovY() { return fovYRad; }
	float getAspectRatio() { return (float)width/height; }
	float getNear() { return near; }
	float getFar() { return far; }
	glm::mat4 getProjection() { return projection; }

private:
	SDL_Window* window;
	int width, height;

	const float fovYRad = 60.f * ((float)M_PI / 180.f);
	const float near = 1.f;
	const float far = 1000.f;

	GLuint mainProgram;
	bool fade;

	glm::mat4 projection;
};


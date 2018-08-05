#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>

#include <vector>

#include "Camera.h"
#include "lodepng.h"
#include "RayTracer.h"
#include "Renderable.h"

class RenderEngine {

public:
	RenderEngine(SDL_Window* window);

	void render(const std::vector<const Renderable*>& objects, const glm::mat4& view, float max, float min);
	void renderSkybox(const glm::mat4& view);

	static void assignBuffers(Renderable& renderable, bool texture);
	static void setBufferData(Renderable& renderable, bool texture);
	static void deleteBufferData(Renderable& renderable, bool texture);
	static GLuint loadTexture(const std::string& filename);

	void setWindowSize(int newWidth, int newHeight);
	void toggleFade() { fade = !fade; }

	float getFovY() { return fovYRad; }
	float getAspectRatio() { return (float)width/height; }
	float getNear() { return fovNear; }
	float getFar() { return fovFar; }
	glm::mat4 getProjection() { return projection; }

	void setScale(float s) { scale = s; }

private:
	void createSkybox();

	SDL_Window* window;
	int width, height;

	const float fovYRad = 60.f * ((float)M_PI / 180.f);
	const float fovNear = 1.f;
	const float fovFar = 1000.f;

	GLuint mainProgram;
	GLuint skyboxProgram;
	bool fade;

	Renderable skybox;

	glm::mat4 projection;

	float scale;
};


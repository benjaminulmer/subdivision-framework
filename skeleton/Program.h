#pragma once

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#undef main
#include <iostream>
#include <string>
#include <sstream>
#include <cfloat>

#include "RenderEngine.h"
#include "InputHandler.h"
#include "ContentReadWrite.h"
#include "Sdog.h"

enum class BoundParam {
	MAX_RADIUS,
	MIN_RADIUS,
	MAX_LAT,
	MIN_LAT,
	MAX_LONG,
	MIN_LONG
};

class Program {

public:
	Program();

	void start();
	void setScheme(Scheme scheme);
	void updateSubdivisionLevel(int add);
	void updateBounds(BoundParam param, int inc);

private:
	SDL_Window* window;
	int width, height;

	RenderEngine* renderEngine;
	Camera* camera;

	Sdog* sdog;
	Renderable referenceOctant;
	Renderable cells;
	int level;
	std::vector<Renderable*> objects;

	void setupWindow();
	void mainLoop();
};

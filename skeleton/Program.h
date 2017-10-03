#pragma once

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#undef main
#include <iostream>
#include <string>
#include <sstream>

#include "RenderEngine.h"
#include "InputHandler.h"
#include "ContentReadWrite.h"
#include "Geometry.h"

class Program {

public:
	Program();

	void start();

private:
	SDL_Window* window;
	int width, height;

	RenderEngine* renderEngine;
	Camera* camera;

	int mouseX, mouseY;

	void setupWindow();
	void mainLoop();
};

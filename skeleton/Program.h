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
#include "Thompson.h"
#include "EqualAreaTriangle.h"
#include "EqualAreaTriangleSubdivision.h"
#include "EqualAreaOctahedron.h"
#include "EqualAreaDodecahedron.h"
#include "EqualAreaTetrahedron.h"
#include "double128/double128.h"

#include "DGGSDB.h"
#include "DGGS.h"

class Program {

public:
	Program();

	void start();

	void setMousePos(int x, int y);

	void _3DPick();

private:
	SDL_Window* window;
	int width, height;

	RenderEngine* renderEngine;
	Camera* camera;

	std::vector<Renderable> objects;

	int mouseX, mouseY;

	DGGS myDGGS;

	void setupWindow();
	void mainLoop();
};

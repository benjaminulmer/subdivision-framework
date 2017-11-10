#pragma once

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#undef main
#include <iostream>
#include <string>
#include <sstream>
#include <cfloat>
#include <algorithm>

#include "RenderEngine.h"
#include "InputHandler.h"
#include "ContentReadWrite.h"
#include "VolumetricSphericalHierarchy.h"

enum class DisplayMode {
	DATA,
	VOLUMES,
	LINES
};

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
	void updateGrid(int levelInc);
	void updateBounds(BoundParam param, int inc);

	void toggleRefSize();
	void toggleRefShape();
	void toggleRef();

	void setBoundsDrawing(bool state);
	void toggleCull();

private:
	const int MAX_LEVEL;

	SDL_Window* window;
	int width, height;

	RenderEngine* renderEngine;
	Camera* camera;

	GridInfo info;
	VolumetricSphericalHierarchy* root;

	Renderable referenceOct;
	Renderable referenceSphere;
	Renderable currRef;
	Renderable grids;
	Renderable cullBounds;

	int subdivLevel;
	DisplayMode dispMode;
	bool refOn;
	bool fullSphereRef;
	bool fullSizeRef;

	std::vector<Renderable*> objects;

	void setupWindow();
	void mainLoop();
	void updateReference();
};

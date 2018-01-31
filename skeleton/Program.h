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

	void createGrid(Scheme scheme);
	void updateGrid(int levelInc);
	void updateBounds(BoundParam param, int inc);

	void toggleRefSize();
	void toggleRefShape();
	void toggleRef();

	void setBoundsDrawing(bool state);
	void toggleCull();
	void toggleRotation();
	void toggleSurfaceLocation();

	void setSubdivisionMode(SubdivisionMode mode);
	void setDisplayMode(DisplayMode mode);

	void toggleMakingSelection();

private:
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
	Renderable coastLines;

	std::vector<Renderable*> objects;

	int maxSubdivLevel;
	int subdivLevel;

	bool makingSelection;

	bool rotation;
	DisplayMode dispMode;
	bool refOn;
	bool fullSphereRef;
	bool fullSizeRef;

	void setupWindow();
	void mainLoop();

	void updateReference();
	void calculateVolumes(int level);
};

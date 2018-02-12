#pragma once

#include <SDL2/SDL.h>
#undef main

#include "Camera.h"
#include "RenderEngine.h"
#include "SphericalGrid.h"

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
	SphericalData eqData1;
	SphericalData eqData2;
	SphericalGrid* root;

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

	void calculateVolumes(int level);
};

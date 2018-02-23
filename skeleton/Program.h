#pragma once

#include <SDL2/SDL.h>
#undef main

#include "Camera.h"
#include "RenderEngine.h"
#include "SphericalGrid.h"

enum class RadialBound {
	MAX,
	MIN,
	BOTH
};


class Program {

public:
	Program();

	void start();

	void refreshGrid() { createGrid(info.scheme); }
	void createGrid(Scheme scheme);
	void updateGrid(int levelInc);

	void updateRadialBounds(RadialBound b, int dir);
	void toggleSurfaceLocation();

	void setDisplayMode(DisplayMode mode);

private:
	SDL_Window* window;
	int width, height;

	RenderEngine* renderEngine;
	Camera* camera;

	GridInfo info;
	SphericalData eqData;
	SphericalData pathsData;
	SphericalGrid* root;

	Renderable cells;
	Renderable coastLines;

	std::vector<Renderable*> objects;

	int maxTreeDepth;

	DisplayMode dispMode;

	void setupWindow();
	void mainLoop();
};

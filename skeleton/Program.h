#pragma once

#include <SDL2/SDL.h>
#undef main

#include "Camera.h"
#include "RenderEngine.h"

#include "test.h"

enum class RadialBound {
	MAX,
	MIN,
	BOTH
};

class Program {

public:
	Program();

	void start();

	void refreshGrid() { createGrid(); }
	void createGrid();
	void updateGrid();

	void updateRotation(int oldX, int newX, int oldY, int newY, bool skew);
	void updateScale(float inc);
	void updateRadialBounds(RadialBound b, int dir);
	void toggleSurfaceLocation();

	void setDisplayMode();
	void updateViewLevel(int inc);

private:
	SDL_Window* window;
	int width, height;

	RenderEngine* renderEngine;
	Camera* camera;

	SphericalData eqData;
	SphericalData pathsData;
	SphericalData sampleData;
	SphericalData sampleData2;
	SphGrid* root;

	Renderable cells;
	Renderable coastLines;

	std::string targetCode;
	std::vector<std::string> neighbourCodes;
	Renderable target;
	Renderable neighbours;

	std::vector<Renderable*> objects;

	int maxTreeDepth;
	int viewLevel;

	float scale;
	float radius;

	float latRot;
	float longRot;

	void setupWindow();
	void mainLoop();
};

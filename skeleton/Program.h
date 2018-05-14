#pragma once

#include <SDL2/SDL.h>
#undef main

#include "Camera.h"
#include "RenderEngine.h"

#include "SdogDB.h"

enum class RadialBound {
	MAX,
	MIN,
	BOTH
};

class Program {

public:
	Program();

	void start();

	void createGrid();
	void updateGrid();

	void updateRotation(int oldX, int newX, int oldY, int newY, bool skew);
	void updateScale(int inc);
	void toggleSurfaceLocation();

	void setDisplayMode();
	void updateViewLevel(int inc);

private:
	SDL_Window* window;
	int width, height;

	RenderEngine* renderEngine;
	Camera* camera;

	SdogDB* dataBase;

	Renderable cells;
	Renderable coastLines;

	std::vector<const Renderable*> objects;

	int maxTreeDepth;
	int viewLevel;

	float radius;

	float scale;
	float latRot;
	float longRot;

	void setupWindow();
	void mainLoop();
};

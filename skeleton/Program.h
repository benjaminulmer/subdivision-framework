#pragma once

#include <iostream>

#include <SDL2/SDL.h>
#undef main

#include "Camera.h"
#include "RayTracer.h"
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

	void updateRotation(int oldX, int newX, int oldY, int newY, bool skew);
	void updateScale(int inc);

	void setArgs(int c, char** v) { argc = c; argv = v; }

	void rayTrace();

private:
	SDL_Window* window;
	int width, height;

	RenderEngine* renderEngine;
	Camera* camera;
	RayTracer* rayTracer;

	SdogDB* dataBase;

	Renderable polys;
	Renderable stormPolys;
	Renderable cells;
	//Renderable bound;
	Renderable wind;
	Renderable coastLines;
	std::vector<Renderable*> bounds;

	std::vector<const Renderable*> objects;

	float radius;

	float scale;
	float latRot;
	float longRot;

	void setupWindow();
	void insertAirSigmets();
	void insertWind();

	void airSigRender1();
	void windRender1();

	void mainLoop();

	bool traceRays;

	int argc;
	char** argv;
};

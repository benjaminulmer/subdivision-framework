#define _USE_MATH_DEFINES
#include "Program.h"

#include <GL/glew.h>
#include <glm/gtx/intersect.hpp>
#include <SDL2/SDL_opengl.h>

#include <cmath>
#include <limits>
#include <iostream>

#include "Constants.h"
#include "ContentReadWrite.h"
#include "InputHandler.h"
#include "AirSigmet.h"
#include "SdogCell.h"
#include "SphCoord.h"

Program::Program() {

	window = nullptr;
	renderEngine = nullptr;
	camera = nullptr;
	root = nullptr;

	maxTreeDepth = 0;
	viewLevel = 7;
	longRot = 0;
	latRot = 0;

	width = height = 800;
}

// Called to start the program. Conducts set up then enters the main loop
void Program::start() {	

	setupWindow();
	GLenum err = glewInit();
	if (glewInit() != GLEW_OK) {
		std::cerr << glewGetErrorString(err) << std::endl;
		system("pause");
		exit(EXIT_FAILURE);
	}

	camera = new Camera();
	renderEngine = new RenderEngine(window);
	InputHandler::setUp(camera, renderEngine, this);

	// Assign buffers
	RenderEngine::assignBuffers(cells, false);
	cells.fade = true;


	RenderEngine::assignBuffers(cells2, false);
	cells2.fade = true;
	RenderEngine::assignBuffers(poly, false);
	poly.drawMode = GL_LINES;


	// Set starting radius
	scale = 1.f;
	radius = RADIUS_EARTH_MODEL * 4.f / 3.f;

	// Load coatline data set
	rapidjson::Document cl = ContentReadWrite::readJSON("data/coastlines.json");
	coastLines = Renderable(cl);
	RenderEngine::assignBuffers(coastLines, false);
	coastLines.fade = true;
	RenderEngine::setBufferData(coastLines, false);

	float s = 1.f + std::numeric_limits<float>::epsilon();
	coastLines.scale = glm::scale(glm::vec3(s * scale, s * scale, s * scale));

	// Create grid
	createGrid();

	// Objects to draw initially
	objects.push_back(&coastLines);
	//objects.push_back(&cells);
	objects.push_back(&cells2);
	objects.push_back(&poly);



	// SIGMET insert prototype

	// Set up dummy data
	AirSigmet airSig;
	airSig.minRad = radius * 0.750001f;
	airSig.maxRad = radius * 0.76f;

	airSig.polygon.push_back(SphCoord(43.7314, -124.6716, false));
	airSig.polygon.push_back(SphCoord(42.7314, -123.7461, false));
	airSig.polygon.push_back(SphCoord(41.0248, -123.7585, false));
	airSig.polygon.push_back(SphCoord(39.7477, -122.7337, false));
	airSig.polygon.push_back(SphCoord(39.8974, -124.6467, false));
	airSig.polygon.push_back(SphCoord(41.1384, -124.8541, false));

	// Create renderable for polygon
	poly.lineColour = glm::vec3(0.f, 1.f, 0.f);
	for (int i = 0; i < airSig.polygon.size(); i++) {
		glm::vec3 v1 = airSig.polygon[i].toCartesian(airSig.maxRad);
		glm::vec3 v2 = airSig.polygon[(i + 1) % airSig.polygon.size()].toCartesian(airSig.maxRad);
		Geometry::createArcR(v1, v2, glm::vec3(), poly);
	}
	RenderEngine::setBufferData(poly, false);

	airSig.gridInsertion(radius, 12, interior, boundary);
	root->insertAirSigmet(interior, boundary);
	// end SIGMET insert prototype


	updateGrid();
	mainLoop();
}

// Creates SDL window for the program and sets callbacks for input
void Program::setupWindow() {

	if (SDL_Init(SDL_INIT_VIDEO) != 0){
		std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
		system("pause");
		exit(EXIT_FAILURE);
	}

	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	window = SDL_CreateWindow("sudivision framework", 10, 30, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (window == nullptr) {
		//TODO: cleanup methods upon exit
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		exit(EXIT_FAILURE);
	}

	SDL_GLContext context = SDL_GL_CreateContext(window);
	if (context == NULL) {
		std::cout << "OpenGL context could not be created! SDL Error: " << SDL_GetError() << std::endl;
	}
	SDL_GL_SetSwapInterval(1); // Vsync on
}

// Main loop
void Program::mainLoop() {

	while (true) {

		// Process all SDL events
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			InputHandler::pollEvent(e);
		}

		float far = glm::length(camera->getPosition() - glm::vec3(0.f, 0.f, 0.f));
		//info.frust = Frustum(*camera, renderEngine->getFovY(), renderEngine->getAspectRatio(), renderEngine->getNear(), far);

		// Find min and max distance from camera to cell renderable - used for fading effect
		glm::vec3 cameraPos = camera->getPosition();
		float max = glm::length(cameraPos) + RADIUS_EARTH_MODEL;
		float min = glm::length(cameraPos) - RADIUS_EARTH_MODEL;

		cells.rot = glm::rotate(latRot, glm::vec3(-1.f, 0.f, 0.f)) * glm::rotate(longRot, glm::vec3(0.f, 1.f, 0.f));
		cells2.rot = glm::rotate(latRot, glm::vec3(-1.f, 0.f, 0.f)) * glm::rotate(longRot, glm::vec3(0.f, 1.f, 0.f));
		poly.rot = glm::rotate(latRot, glm::vec3(-1.f, 0.f, 0.f)) * glm::rotate(longRot, glm::vec3(0.f, 1.f, 0.f));
		coastLines.rot = glm::rotate(latRot, glm::vec3(-1.f, 0.f, 0.f)) * glm::rotate(longRot, glm::vec3(0.f, 1.f, 0.f));

		renderEngine->render(objects, camera->getLookAt(), max, min);
		SDL_GL_SwapWindow(window);
	}

	delete root;
}

// Sets the scheme that will be used for subdivision
void Program::createGrid() {

	delete root;
	root = new SdogDB("test.db", radius);

	// Set max number of grids depending on subdivision scheme
	// These might need to be tweaked
	//int max = 4000000;

	//// Determine max number of subdivision levels that can be reasonably supported
	//int level = 0;
	//while (true) {

	//	//int numGrids = root->size();
	//	int numGrids = 1;
	//	std::cout << level << " : " << numGrids << std::endl;

	//	if (numGrids < max) {
	//		level++;
	//		root->subdivide();
	//	}
	//	else {
	//		maxTreeDepth = level;
	//		break;
	//	}
	//	if (level >= 5) {
	//		//system("pause");
	//		maxTreeDepth = level;
	//		break;
	//	}
	//}
	updateGrid();
}

// Updates the level of subdivision being shown
void Program::updateGrid() {

	cells.verts.clear();
	cells.colours.clear();

	cells2.verts.clear();
	cells2.colours.clear();

	cells2.lineColour = glm::vec3(1.f, 0.f, 0.f);

	//root->createRenderable(cells, viewLevel);
	SdogDB::createRenderable(cells, boundary, radius);
	RenderEngine::setBufferData(cells, false);

//	std::vector<std::string> cell;
//	cell.push_back("3036775555555");

	SdogDB::createRenderable(cells2, interior, radius);
	RenderEngine::setBufferData(cells2, false);
}

// Updates camera rotation
// Locations are in pixel coordinates
void Program::updateRotation(int oldX, int newX, int oldY, int newY, bool skew) {

	glm::mat4 projView = renderEngine->getProjection() * camera->getLookAt();
	glm::mat4 invProjView = glm::inverse(projView);

	float oldXN = (2.f * oldX) / (width) - 1.f; 
	float oldYN = (2.f * oldY) / (height) - 1.f;
	oldYN *= -1.0;

	float newXN = (2.f * newX) / (width) - 1.f; 
	float newYN = (2.f * newY) / (height) - 1.f;
	newYN *= -1.f;

	glm::vec4 worldOld(oldXN, oldYN, -1.f, 1.f);
	glm::vec4 worldNew(newXN, newYN, -1.f, 1.f);

	worldOld = invProjView * worldOld; 

	worldOld.x /= worldOld.w;
	worldOld.y /= worldOld.w;
	worldOld.z /= worldOld.w;

	worldNew = invProjView * worldNew;

	worldNew.x /= worldNew.w;
	worldNew.y /= worldNew.w;
	worldNew.z /= worldNew.w;

	glm::vec3 rayO = camera->getPosition();
	glm::vec3 rayDOld = glm::normalize(glm::vec3(worldOld) - rayO);
	glm::vec3 rayDNew = glm::normalize(glm::vec3(worldNew) - rayO);
	float sphereRad = RADIUS_EARTH_MODEL * scale;
	glm::vec3 sphereO = glm::vec3(0.f, 0.f, 0.f);

	glm::vec3 iPosOld, iPosNew, iNorm;

	if (glm::intersectRaySphere(rayO, rayDOld, sphereO, sphereRad, iPosOld, iNorm) && 
			glm::intersectRaySphere(rayO, rayDNew, sphereO, sphereRad, iPosNew, iNorm)) {

		float longOld = atan(iPosOld.x / iPosOld.z);
		float latOld = (float)( M_PI / 2.f - acos(iPosOld.y / sphereRad) );

		float longNew = atan(iPosNew.x / iPosNew.z);
		float latNew = (float)( M_PI / 2.f - acos(iPosNew.y / sphereRad) );

		if (skew) {
			camera->updateLatitudeRotation(latNew - latOld);
		}
		else {
			latRot += latNew - latOld;
			longRot += longNew - longOld;
		}
	}
}

// Changes scale of model
void Program::updateScale(int inc) {

	if (inc < 0) {
		scale /= 1.4f;
	}
	else {
		scale *= 1.4f;
	}
	camera->setScale(scale);

	float s = 1.f + std::numeric_limits<float>::epsilon();
	cells.scale = glm::scale(glm::vec3(scale, scale, scale));
	cells2.scale = glm::scale(glm::vec3(scale, scale, scale));
	poly.scale = glm::scale(glm::vec3(scale, scale, scale));
	coastLines.scale = glm::scale(glm::vec3(s * scale, s * scale, s * scale));
}

// Toggles location of surface between 0.5 and 0.75
void Program::toggleSurfaceLocation() {

	if (radius == RADIUS_EARTH_MODEL * 2.f) {
		radius = RADIUS_EARTH_MODEL * 4.f / 3.f;
	}
	else {
		radius = RADIUS_EARTH_MODEL * 2.f;
	}
	createGrid();
}

// Sets display mode for rendering
void Program::setDisplayMode() {
	//dispMode = mode;
	//updateGrid();
}

// Update subdivision level of rendering
void Program::updateViewLevel(int inc) {
	viewLevel += inc;
	if (viewLevel < 0) viewLevel = 0;
	if (viewLevel > maxTreeDepth) viewLevel = maxTreeDepth;

	std::cout << "Current subdivision level for view: " << viewLevel << std::endl;
	updateGrid();
}
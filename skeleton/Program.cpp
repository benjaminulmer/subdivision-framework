#define _USE_MATH_DEFINES
#include "Program.h"

#include <GL/glew.h>
#include <glm/gtx/intersect.hpp>
#include <SDL2/SDL_opengl.h>

#include <cmath>
#include <limits>
#include <iostream>

#include "Constants.h"
#include "WindGrid.h"
#include "ContentReadWrite.h"
#include "InputHandler.h"
#include "AirSigmet.h"
#include "SdogCell.h"
#include "SphCoord.h"
#include "Geometry.h"

Program::Program() {

	window = nullptr;
	renderEngine = nullptr;
	camera = nullptr;
	dataBase = nullptr;

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
	RenderEngine::assignBuffers(polys, false);
	RenderEngine::assignBuffers(bound, false);
	cells.fade = true;
	polys.fade = true;
	bound.fade = true;

	// Set starting radius
	scale = 1.f;
	radius = RADIUS_EARTH_KM * 4.f / 3.f;

	// Load coatline data set
	rapidjson::Document cl = ContentReadWrite::readJSON("data/coastlines.json");
	coastLines = Renderable(cl);
	RenderEngine::assignBuffers(coastLines, false);
	coastLines.fade = true;
	RenderEngine::setBufferData(coastLines, false);

	float s = 1.f + std::numeric_limits<float>::epsilon();
	coastLines.model = glm::scale(glm::vec3(s * scale, s * scale, s * scale));

	// Create grid database connection
	dataBase = new SdogDB("test.db", radius);


	// Wind data testing
	rapidjson::Document wind = ContentReadWrite::readJSON("data/wind-25.json");
	std::vector<WindGrid> grids;
	WindGrid::readFromJson(wind, grids);

	std::vector<std::pair<std::string, glm::vec2>> codes;
	WindGrid::gridInsertion(radius, 9, grids, codes);

	// Load and insert sigmet data
	rapidjson::Document sig = ContentReadWrite::readJSON("data/sigmet.json");
	std::vector<AirSigmet> airSigmets;
	AirSigmet::readFromJson(sig, airSigmets);

	int c = 0;
	//for (const AirSigmet& a : airSigmets) {
	//	std::cout << c << std::endl;
	//	std::vector<std::string> interior, boundary;
	//	a.gridInsertion(radius, 10, interior, boundary);
	//	dataBase->insertAirSigmet(interior, boundary, a);
	//	c++;
	//}

	// Objects to draw initially
	objects.push_back(&coastLines);
	objects.push_back(&cells);
	objects.push_back(&polys);
	objects.push_back(&bound);

	// Draw stuff
	std::vector<AirSigmetCells> data;
	dataBase->getAirSigmetCells(data);

	cells.lineColour = glm::vec3(0.8, 0.6f, 0.f);
	cells.drawMode = GL_TRIANGLES;

	bound.lineColour = glm::vec3(0.8, 0.6f, 0.f);
	bound.drawMode = GL_TRIANGLES;

	float max = -1.f;
	float min = 9999999.f;
	double flr = 9999999.f;

	for (const std::pair<std::string, glm::vec2>& p : codes) {

		SdogCell cell(p.first, radius);
		if (cell.getMinRad() < flr) flr = cell.getMinRad();
	}


	for (const std::pair<std::string, glm::vec2>& p : codes) {

		SdogCell cell(p.first, radius);
		if (cell.getMinRad() != flr) continue;

		float mag = glm::length(p.second);
		if (mag > max) max = mag;
		if (mag < min) min = mag;
	}

	for (const std::pair<std::string, glm::vec2>& p : codes) {

		SdogCell cell(p.first, radius);
		if (cell.getMinRad() != flr) continue;

		float mag = glm::length(p.second);
		float norm = (mag - min) / (max - min);

		if (norm < 0.3f) continue;

		cell.addToRenderable(cells, glm::vec3(norm, 0.f, 0.f));
	}

	//int i = -1;
	//for (const AirSigmetCells& datum : data) {
	//	i++;
	//	if (i != 11) continue;


	//	polys.lineColour = glm::vec3(0.f, 1.f, 0.f);
	//	polys.drawMode = GL_LINES;
	//	for (int i = 0; i < datum.airSigmet.polygon.size(); i++) {
	//		glm::vec3 v1 = datum.airSigmet.polygon[i].toCartesian(datum.airSigmet.maxAltKM * 2.2 + RADIUS_EARTH_KM);
	//		glm::vec3 v2 = datum.airSigmet.polygon[(i + 1) % datum.airSigmet.polygon.size()].toCartesian(datum.airSigmet.maxAltKM * 2.2 + RADIUS_EARTH_KM);
	//		Geometry::createArcR(v1, v2, glm::vec3(), polys);
	//	}
	//	RenderEngine::setBufferData(polys, false);


	//	for (const std::string& code : datum.interior) {
	//		
	//		if (code.length() < 11) continue;

	//		SdogCell cell(code, radius);
	//		cell.addToRenderable(cells, glm::vec3(1.f, 1.f, 0.5f));
	//	}

	//	for (const std::string& code : datum.boundary) {

	//		if (code.length() < 11) continue;

	//		SdogCell cell(code, radius);
	//		cell.addToRenderable(bound, glm::vec3(0.2f, 0.2f, 0.5f));
	//	}

	//}
	RenderEngine::setBufferData(cells, false);
	RenderEngine::setBufferData(bound, false);

	//updateGrid();
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
		float max = glm::length(cameraPos) + RADIUS_EARTH_VIEW;
		float min = glm::length(cameraPos) - RADIUS_EARTH_VIEW;

		glm::mat4 worldModel(1.f);
		float s = scale * (1.f / RADIUS_EARTH_KM) * RADIUS_EARTH_VIEW;
		worldModel = glm::scale(worldModel, glm::vec3(s, s, s));
		worldModel = glm::rotate(worldModel, latRot, glm::vec3(-1.f, 0.f, 0.f));
		worldModel = glm::rotate(worldModel, longRot, glm::vec3(0.f, 1.f, 0.f));

		renderEngine->render(objects, camera->getLookAt() * worldModel, max, min);
		SDL_GL_SwapWindow(window);
	}

	delete dataBase;
}

// Sets the scheme that will be used for subdivision
void Program::createGrid() {

	delete dataBase;
	
	updateGrid();
}

// Updates the level of subdivision being shown
void Program::updateGrid() {

	cells.verts.clear();
	cells.colours.clear();

	cells.lineColour = glm::vec3(0.9, 0.f, 0.f);

	//root->createRenderable(cells, viewLevel);
	//SdogDB::createRenderable(cells, interior, radius, 13);
	RenderEngine::setBufferData(cells, false);
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
	float sphereRad = RADIUS_EARTH_VIEW * scale;
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
}

// Toggles location of surface between 0.5 and 0.75
void Program::toggleSurfaceLocation() {

	if (radius == RADIUS_EARTH_VIEW * 2.f) {
		radius = RADIUS_EARTH_VIEW * 4.f / 3.f;
	}
	else {
		radius = RADIUS_EARTH_VIEW * 2.f;
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
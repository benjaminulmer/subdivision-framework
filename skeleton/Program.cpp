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

#include "SdogDB.h"

#include <sqlite/sqlite3.h>

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

	//info.radius = RADIUS_EARTH_MODEL * 4.0 / 3.0;
	//root = new SphGrid(info.radius);

	//SdogDB db("test2.db", 7);
	//system("pause");
	//exit(0);

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

	RenderEngine::assignBuffers(target, false);
	RenderEngine::assignBuffers(neighbours, false);


	RenderEngine::assignBuffers(cells, false);
	cells.fade = true;

	// Set starting radius
	scale = 1.f;
	radius = RADIUS_EARTH_MODEL * 4.f / 3.f;
	//info.cullMaxRadius = 0.75 * info.radius + (20.0 / RADIUS_EARTH_KM) * RADIUS_EARTH_MODEL;
	//info.cullMinRadius = 0.75 * info.radius - (10.0 / RADIUS_EARTH_KM) * RADIUS_EARTH_MODEL;

	// Load earthquake data set
	rapidjson::Document d1 = ContentReadWrite::readJSON("data/eq-2017.json");
	rapidjson::Document m1 = ContentReadWrite::readJSON("data/eq-2017m.json");
	eqData = SphericalData(d1, m1);

	rapidjson::Document d2 = ContentReadWrite::readJSON("data/cat5paths.json");
	rapidjson::Document m2 = ContentReadWrite::readJSON("data/cat5pathsm.json");
	pathsData = SphericalData(d2, m2);

	rapidjson::Document d3 = ContentReadWrite::readJSON("data/samplePath.json");
	rapidjson::Document m3 = ContentReadWrite::readJSON("data/samplePathm.json");
	sampleData = SphericalData(d3, m3);

	rapidjson::Document d4 = ContentReadWrite::readJSON("data/samplePath2.json");
	rapidjson::Document m4 = ContentReadWrite::readJSON("data/samplePath2m.json");
	sampleData2 = SphericalData(d4, m4);

	for (int i = 0; i < 1; i++) {
		pathsData.linSub();
		sampleData.linSub();
		sampleData.linSub();
		sampleData2.linSub();
		sampleData2.linSub();
	}

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
	objects.push_back(&target);
	objects.push_back(&neighbours);

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
		coastLines.rot = glm::rotate(latRot, glm::vec3(-1.f, 0.f, 0.f)) * glm::rotate(longRot, glm::vec3(0.f, 1.f, 0.f));

		target.rot = glm::rotate(latRot, glm::vec3(-1.f, 0.f, 0.f)) * glm::rotate(longRot, glm::vec3(0.f, 1.f, 0.f));
		neighbours.rot = glm::rotate(latRot, glm::vec3(-1.f, 0.f, 0.f)) * glm::rotate(longRot, glm::vec3(0.f, 1.f, 0.f));

		renderEngine->render(objects, camera->getLookAt(), max, min);
		SDL_GL_SwapWindow(window);
	}

	delete root;
}

// Sets the scheme that will be used for subdivision
void Program::createGrid() {

	delete root;
	root = new SphGrid(radius);

	// Set max number of grids depending on subdivision scheme
	// These might need to be tweaked
	int max = 4000000;

	//root->fillData(eqData);
	//root->fillData(pathsData);
	//root->fillData(sampleData);
	//root->fillData(sampleData2);

	std::cout << "first fill" << std::endl;

	// Determine max number of subdivision levels that can be reasonably supported
	int level = 0;
	while (true) {

		//int numGrids = root->size();
		int numGrids = 1;
		std::cout << level << " : " << numGrids << std::endl;

		if (numGrids < max) {
			level++;
			root->subdivide();

			//root->fillData(eqData);
			//root->fillData(pathsData);
			//root->fillData(sampleData);
			//root->fillData(sampleData2);
		}
		else {
			maxTreeDepth = level;
			break;
		}
		if (level >= 5) {
			//system("pause");
			maxTreeDepth = level;
			break;
		}
	}

	targetCode = "5321";
	root->neighbours(targetCode, neighbourCodes);

	updateGrid();
}

// Updates the level of subdivision being shown
void Program::updateGrid() {

	cells.verts.clear();
	cells.colours.clear();

	root->createRenderable(cells, viewLevel);
	RenderEngine::setBufferData(cells, false);


	std::vector<std::string> tar;
	tar.push_back(targetCode);

	target.lineColour = glm::vec3(1.0, 0.0, 0.0);
	neighbours.lineColour = glm::vec3(0.0, 1.0, 0.0);

	root->createRenderable(target, tar);
	root->createRenderable(neighbours, neighbourCodes);

	RenderEngine::setBufferData(target, false);
	RenderEngine::setBufferData(neighbours, false);
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
	coastLines.scale = glm::scale(glm::vec3(s * scale, s * scale, s * scale));

	target.scale = glm::scale(glm::vec3(s * scale, s * scale, s * scale));
	neighbours.scale = glm::scale(glm::vec3(s * scale, s * scale, s * scale));
}

// Updates radial bounds for culling
void Program::updateRadialBounds(RadialBound b, int dir) {

	double amount = radius / maxTreeDepth / 2.0;

	//if (b == RadialBound::MAX) {
	//	info.cullMaxRadius += dir * amount;

	//}
	//else if (b == RadialBound::MIN) {
	//	info.cullMinRadius += dir * amount;

	//}
	//else {//b == RadialBound::BOTH
	//	info.cullMinRadius += dir * amount;
	//	info.cullMaxRadius += dir * amount;
	//}

	//// Bounds enforcing
	//if (info.cullMinRadius < 0.0) info.cullMinRadius = 0.0;
	//if (info.cullMaxRadius > info.radius) info.cullMaxRadius = info.radius;
	//if (info.cullMinRadius > info.cullMaxRadius) info.cullMinRadius = info.cullMaxRadius;
	//if (info.cullMaxRadius < info.cullMinRadius) info.cullMaxRadius = info.cullMinRadius;

	//// Temp until visual representation
	//std::cout << info.cullMinRadius << ", " << info.cullMaxRadius << std::endl;
}

// Toggles location of surface between 0.5 and 0.75
void Program::toggleSurfaceLocation() {

	if (radius == RADIUS_EARTH_MODEL * 2.f) {
		radius = RADIUS_EARTH_MODEL * 4.f / 3.f;
	}
	else {
		radius = RADIUS_EARTH_MODEL * 2.f;
	}
	refreshGrid();
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
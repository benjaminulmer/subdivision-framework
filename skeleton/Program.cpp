#include "Program.h"

#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

#include <iostream>

#include "Constants.h"
#include "ContentReadWrite.h"
#include "Frustum.h"
#include "InputHandler.h"

Program::Program() {

	window = nullptr;
	renderEngine = nullptr;
	camera = nullptr;
	root = nullptr;

	maxTreeDepth = 0;
	dispMode = DisplayMode::DATA;

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

	// Set starting radius
	info.radius = MODEL_SCALE * 4.0 / 3.0;
	info.cullMaxRadius = 0.75 * info.radius + (1000.0 / RADIUS_EARTH_KM) * MODEL_SCALE;
	info.cullMinRadius = 0.75 * info.radius - (1000.0 / RADIUS_EARTH_KM) * MODEL_SCALE;

	// Load earthquake data set
	rapidjson::Document d1 = ContentReadWrite::readJSON("data/eq-2017.json");
	rapidjson::Document m1 = ContentReadWrite::readJSON("data/eq-2017m.json");
	eqData = SphericalData(d1, m1);

	rapidjson::Document d2 = ContentReadWrite::readJSON("data/cat5paths.json");
	rapidjson::Document m2 = ContentReadWrite::readJSON("data/cat5pathsm.json");
	pathsData = SphericalData(d2, m2);

	// Load coatline data set
	rapidjson::Document cl = ContentReadWrite::readJSON("data/coastlines.json");
	coastLines = Renderable(cl);
	RenderEngine::assignBuffers(coastLines, false);
	coastLines.fade = true;
	RenderEngine::setBufferData(coastLines, false);

	// Create grid
	createGrid(Scheme::SDOG);

	// Objects to draw initially
	objects.push_back(&coastLines);
	objects.push_back(&cells);

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
		info.frust = Frustum(*camera, renderEngine->getFovY(), renderEngine->getAspectRatio(), renderEngine->getNear(), far);

		// Find min and max distance from camera to cell renderable - used for fading effect
		glm::vec3 cameraPos = camera->getPosition();
		float max = glm::length(cameraPos) + MODEL_SCALE;
		float min = glm::length(cameraPos) - MODEL_SCALE;

		renderEngine->render(objects, camera->getLookAt(), max, min);
		SDL_GL_SwapWindow(window);
	}
}

// Sets the scheme that will be used for subdivision
void Program::createGrid(Scheme scheme) {

	delete root;
	info.scheme = scheme;
	root = new SphericalGrid(info);

	// Set max number of grids depending on subdivision scheme
	// These might need to be tweaked
	int max = 400000;

	// Determine max number of subdivision levels that can be reasonably supported
	int level = 0;
	while (true) {
		int numGrids = root->countLeafs();
		if (numGrids < max) {
			level++;
			root->subdivide();
		}
		else {
			maxTreeDepth = level;
			break;
		}
	}

	root->fillData(eqData);
	root->fillData(pathsData);
	updateGrid(0);
}

// Updates the level of subdivision being shown
void Program::updateGrid(int levelInc) {

	cells.verts.clear();
	cells.colours.clear();

	root->createRenderable(cells, dispMode);
	RenderEngine::setBufferData(cells, false);
}

// Updates radial bounds for culling
void Program::updateRadialBounds(RadialBound b, int dir) {

	double amount = info.radius / maxTreeDepth / 2.0;

	if (b == RadialBound::MAX) {
		info.cullMaxRadius += dir * amount;

	}
	else if (b == RadialBound::MIN) {
		info.cullMinRadius += dir * amount;

	}
	else {//b == RadialBound::BOTH
		info.cullMinRadius += dir * amount;
		info.cullMaxRadius += dir * amount;
	}

	// Bounds enforcing
	if (info.cullMinRadius < 0.0) info.cullMinRadius = 0.0;
	if (info.cullMaxRadius > info.radius) info.cullMaxRadius = info.radius;
	if (info.cullMinRadius > info.cullMaxRadius) info.cullMinRadius = info.cullMaxRadius;
	if (info.cullMaxRadius < info.cullMinRadius) info.cullMaxRadius = info.cullMinRadius;

	// Temp until visual representation
	std::cout << info.cullMinRadius << ", " << info.cullMaxRadius << std::endl;
}

// Toggles location of surface between 0.5 and 0.75
void Program::toggleSurfaceLocation() {

	if (info.radius == MODEL_SCALE * 2.0) {
		info.radius = MODEL_SCALE * 4.0 / 3.0;
	}
	else {
		info.radius = MODEL_SCALE * 2.0;
	}
	refreshGrid();
}

// Sets display mode for rendering
void Program::setDisplayMode(DisplayMode mode) {
	dispMode = mode;
	updateGrid(0);
}
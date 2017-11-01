#include "Program.h"

Program::Program() {
	window = nullptr;
	renderEngine = nullptr;
	camera = nullptr;
	sdog = nullptr;

	level = 1;
	width = height = 512;
}

// Called to start the program. Conducts set up then enters the main loop
void Program::start() {
	setupWindow();
	GLenum err = glewInit();
	if (glewInit() != GLEW_OK) {
		std::cerr << glewGetErrorString(err) << std::endl;
	}

	camera = new Camera();
	renderEngine = new RenderEngine(window, camera);
	InputHandler::setUp(camera, renderEngine, this);

	ContentReadWrite::loadOBJ("models/octant.obj", referenceOctant);
	referenceOctant.colour = glm::vec3(1.f, 1.f, 1.f);
	RenderEngine::assignBuffers(referenceOctant);
	RenderEngine::setBufferData(referenceOctant);

	setScheme(Scheme::SDOG);

	objects.push_back(&referenceOctant);
	objects.push_back(&cells);

	mainLoop();
}

// Creates SDL window for the program and sets callbacks for input
void Program::setupWindow() {
	if (SDL_Init(SDL_INIT_VIDEO) != 0){
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		exit(EXIT_FAILURE);
	}

	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	window = SDL_CreateWindow("sudivision framework", 10, 30, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (window == nullptr){
		//TODO: cleanup methods upon exit
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		exit(EXIT_FAILURE);
	}

	SDL_GLContext context = SDL_GL_CreateContext(window);
	if (context == NULL)
	{
		std::cout << "OpenGL context could not be created! SDL Error: " << SDL_GetError() << std::endl;
	}

	SDL_GL_SetSwapInterval(1); // Vsync on
}

// Main loop
void Program::mainLoop() {

	int count = 0;
	while (true) {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			InputHandler::pollEvent(e);
		}

		float max = -FLT_MAX;
		float min = FLT_MAX;

		glm::vec3 cameraPos = camera->getPosition();
		for (glm::vec3 v : cells.verts) {
			float dist = glm::length(cameraPos - v);

			max = (dist > max) ? dist : max;
			min = (dist < min) ? dist : min;
		}
		renderEngine->render(objects, camera->getLookAt(), max, min);
		SDL_GL_SwapWindow(window);

		count++;
	}
}

// Sets the scheme that will be used for octant subdivision
void Program::setScheme(Scheme scheme) {
	delete sdog;
	sdog = new Sdog(scheme, 4.0);
	updateSubdivisionLevel(0);
}

// Updates the level of subdivision being shown
void Program::updateSubdivisionLevel(int add) {

	if (level + add < 0 || level + add > 6) {
		return;
	}
	level += add;

	cells = Renderable();
	sdog->createRenderable(cells, level);
	cells.colour = glm::vec3(0.f, 0.f, 0.f);
	cells.fade = true;
	RenderEngine::assignBuffers(cells);
	RenderEngine::setBufferData(cells);
}

// Updates bounds of which cells to show
void Program::updateBounds(BoundParam param, int inc) {
	if (param == BoundParam::MAX_RADIUS) {

	}
	else if (param == BoundParam::MIN_RADIUS) {

	}
	else if (param == BoundParam::MAX_LAT) {
		Sdog::maxLat += inc * M_PI / 180;
	}
	else if (param == BoundParam::MIN_LAT) {
		Sdog::minLat += inc * M_PI / 180;
	}
	else if (param == BoundParam::MAX_LONG) {
		Sdog::maxLong += inc * M_PI / 180;
	}
	else if (param == BoundParam::MIN_LONG) {
		Sdog::minLong += inc * M_PI / 180;
	}
	updateSubdivisionLevel(0);
}

// Toggles drawing of reference octant
void Program::toggleReference() {
	if (objects.size() == 1) {
		objects.push_back(&referenceOctant);
	}
	else {
		objects.clear();
		objects.push_back(&cells);
	}
}
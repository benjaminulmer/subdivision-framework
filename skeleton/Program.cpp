#include "Program.h"

Program::Program() {
	window = nullptr;
	renderEngine = nullptr;
	camera = nullptr;
	sdog = nullptr;

	level = 1;
	referenceState = 0;
	width = height = 800;
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

	// Assign buffers
	RenderEngine::assignBuffers(referenceOctant);
	RenderEngine::assignBuffers(cells);
	RenderEngine::assignBuffers(bounds);

	// Create geometry for reference
	ContentReadWrite::loadOBJ("models/octant.obj", referenceOctant);
	RenderEngine::setBufferData(referenceOctant);

	// Object data
	cells.fade = true;

	// Objects to draw
	objects.push_back(&referenceOctant);
	objects.push_back(&cells);

	// Set up GridInfo
	info.radius = 4.0;
	info.maxRadius = 4.0; info.minRadius = 0.0;
	info.maxLat = M_PI / 2; info.minLat = 0.0;
	info.maxLong = 0.0, info.minLong = -M_PI / 2;

	// And renderable for it
	SphericalGrid b(GridType::NG, info, info.maxRadius, info.minRadius, info.maxLat, info.minLat, info.maxLong, info.minLong);
	bounds.verts.clear();
	bounds.colours.clear();
	b.createRenderable(bounds, 0, 0, 0, 0, true);
	RenderEngine::setBufferData(bounds);

	setScheme(Scheme::SDOG);
	info.cull = true;

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
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			InputHandler::pollEvent(e);
		}

		// Find min and max distance for cell renderable
		// Used for fading
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
	}
}

// Sets the scheme that will be used for octant subdivision
void Program::setScheme(Scheme scheme) {
	delete sdog;
	info.scheme = scheme;
	sdog = new VolumetricSphericalHierarchy(info);
	updateSubdivisionLevel(0);
}

// Updates the level of subdivision being shown
void Program::updateSubdivisionLevel(int add) {
	if (level + add < 0 || level + add > 6) {
		return;
	}
	level += add;

	std::vector<float> volumes;
	sdog->getVolumes(volumes, level);

	float max = -FLT_MAX;
	float min = FLT_MAX;
	float avg = 0.f;

	for (float v : volumes) {
		avg += v;
		max = (v > max) ? v : max;
		min = (v < min) ? v : min;
	}
	avg /= volumes.size();


	cells.verts.clear();
	cells.colours.clear();
	sdog->createRenderable(cells, level, max, min, avg);
	RenderEngine::setBufferData(cells);
}

// Updates bounds of which cells to show
void Program::updateBounds(BoundParam param, int inc) {

	// Update proper bound
	if (param == BoundParam::MAX_RADIUS) {
		info.maxRadius -= inc * 0.1;
		if (info.maxRadius >= 4.0) info.maxRadius = 4.0;
		if (info.maxRadius <= info.minRadius) info.maxRadius = info.minRadius;
	}
	else if (param == BoundParam::MIN_RADIUS) {
		info.minRadius += inc * 0.1;
		if (info.minRadius >= info.maxRadius) info.minRadius = info.maxRadius;
		if (info.minRadius <= 0.0) info.minRadius = 0.0;
	}
	else if (param == BoundParam::MAX_LAT) {
		info.maxLat -= inc * M_PI / 180;
		if (info.maxLat >= M_PI / 2) info.maxLat = M_PI / 2;
		if (info.maxLat <= info.minLat) info.maxLat = info.minLat;
	}
	else if (param == BoundParam::MIN_LAT) {
		info.minLat += inc * M_PI / 180;
		if (info.minLat >= info.maxLat) info.minLat = info.maxLat;
		//if (Sdog::minLat <= 0.0) Sdog::minLat = 0.0;
	}
	else if (param == BoundParam::MAX_LONG) {
		info.maxLong -= inc * M_PI / 180;
		//if (Sdog::maxLong >= 0.0) Sdog::maxLong = 0.0;
		if (info.maxLong <= info.minLong) info.maxLong = info.minLong;
	}
	else if (param == BoundParam::MIN_LONG) {
		info.minLong += inc * M_PI / 180;
		if (info.minLong >= info.maxLong) info.minLong = info.maxLong;
		//if (Sdog::minLong <= -M_PI / 2) Sdog::minLong = -M_PI / 2;
	}

	SphericalGrid b(GridType::NG, info, info.maxRadius, info.minRadius, info.maxLat, info.minLat, info.maxLong, info.minLong);
	bounds.verts.clear();
	bounds.colours.clear();
	b.createRenderable(bounds, 0, 0, 0, 0, true);
	RenderEngine::setBufferData(bounds);

	updateSubdivisionLevel(0);
}

// Toggles drawing of reference octant
void Program::toggleReference() {
	if (referenceState == 0) {
		referenceOctant.model = glm::scale(glm::vec3(2.f, 2.f, 2.f));
		referenceState++;
	}
	else if (referenceState == 1) {
		auto pos = std::find(objects.begin(), objects.end(), &referenceOctant);
		objects.erase(pos);
		referenceState++;
	}
	else {
		objects.push_back(&referenceOctant);
		referenceOctant.model = glm::mat4();
		referenceState = 0;
	}
}

// Turn bounds box drawing on or off
void Program::setBoundsDrawing(bool state) {
	auto pos = std::find(objects.begin(), objects.end(), &bounds);

	if (!state && pos != objects.end()) {
		objects.erase(pos);
	}
	else if (state && pos == objects.end()) {
		objects.push_back(&bounds);
	}
}

// Toggles bounds culling
void Program::toggleCull() {
	info.cull = !info.cull;
	updateSubdivisionLevel(0);
}
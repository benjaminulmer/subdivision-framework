#include "Program.h"

Program::Program() {
	window = nullptr;
	renderEngine = nullptr;
	camera = nullptr;
	root = nullptr;

	level = 1;
	referenceState = 2;
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

	// Renderable data
	cells.fade = true;

	// Objects to draw
	//objects.push_back(&referenceOctant);
	objects.push_back(&cells);

	// Set up GridInfo
	info.radius = 4.0;
	info.cullMaxRadius = 4.0; info.cullMinRadius = 0.0;
	info.cullMaxLat = M_PI / 2; info.cullMinLat = 0.0;
	info.cullMaxLong = 0.0, info.cullMinLong = -M_PI / 2;

	info.data = SphericalData(0);

	// Renderable for cull bounds
	SphericalGrid b(GridType::NG, info, info.cullMaxRadius, info.cullMinRadius, info.cullMaxLat, info.cullMinLat, info.cullMaxLong, info.cullMinLong);
	bounds.verts.clear();
	bounds.colours.clear();
	b.createRenderable(bounds, 0, true);
	RenderEngine::setBufferData(bounds);

	info.cull = true;
	setScheme(Scheme::SDOG);

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
	delete root;
	info.scheme = scheme;
	root = new VolumetricSphericalHierarchy(info);
	updateGrid(0);
}

// Updates the level of subdivision being shown
void Program::updateGrid(int levelInc) {
	if (level + levelInc < 0 || level + levelInc > 6) {
		return;
	}
	level += levelInc;

	root->updateInfo(info);

	cells.verts.clear();
	cells.colours.clear();
	root->fillData(level);
	root->createRenderable(cells, level, false, true);
	RenderEngine::setBufferData(cells);
}

// Updates bounds of which cells to show
void Program::updateBounds(BoundParam param, int inc) {

	// Update proper bound
	if (param == BoundParam::MAX_RADIUS) {
		info.cullMaxRadius -= inc * 0.1;
		if (info.cullMaxRadius >= 4.0) info.cullMaxRadius = 4.0;
		if (info.cullMaxRadius <= info.cullMinRadius) info.cullMaxRadius = info.cullMinRadius;
	}
	else if (param == BoundParam::MIN_RADIUS) {
		info.cullMinRadius += inc * 0.1;
		if (info.cullMinRadius >= info.cullMaxRadius) info.cullMinRadius = info.cullMaxRadius;
		if (info.cullMinRadius <= 0.0) info.cullMinRadius = 0.0;
	}
	else if (param == BoundParam::MAX_LAT) {
		info.cullMaxLat -= inc * M_PI / 180;
		if (info.cullMaxLat >= M_PI / 2) info.cullMaxLat = M_PI / 2;
		if (info.cullMaxLat <= info.cullMinLat) info.cullMaxLat = info.cullMinLat;
	}
	else if (param == BoundParam::MIN_LAT) {
		info.cullMinLat += inc * M_PI / 180;
		if (info.cullMinLat >= info.cullMaxLat) info.cullMinLat = info.cullMaxLat;
		//if (Sdog::minLat <= 0.0) Sdog::minLat = 0.0;
	}
	else if (param == BoundParam::MAX_LONG) {
		info.cullMaxLong -= inc * M_PI / 180;
		//if (Sdog::maxLong >= 0.0) Sdog::maxLong = 0.0;
		if (info.cullMaxLong <= info.cullMinLong) info.cullMaxLong = info.cullMinLong;
	}
	else if (param == BoundParam::MIN_LONG) {
		info.cullMinLong += inc * M_PI / 180;
		if (info.cullMinLong >= info.cullMaxLong) info.cullMinLong = info.cullMaxLong;
		//if (Sdog::minLong <= -M_PI / 2) Sdog::minLong = -M_PI / 2;
	}

	SphericalGrid b(GridType::NG, info, info.cullMaxRadius, info.cullMinRadius, info.cullMaxLat, info.cullMinLat, info.cullMaxLong, info.cullMinLong);
	bounds.verts.clear();
	bounds.colours.clear();
	b.createRenderable(bounds, 0, true);
	RenderEngine::setBufferData(bounds);

	updateGrid(0);
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
	updateGrid(0);
}
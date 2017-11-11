#include "Program.h"

Program::Program() : MAX_LEVEL(6) {
	window = nullptr;
	renderEngine = nullptr;
	camera = nullptr;
	root = nullptr;

	subdivLevel = 1;
	dispMode = DisplayMode::DATA;

	refOn = false;
	fullSphereRef = false;
	fullSizeRef = false;

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
	RenderEngine::assignBuffers(referenceOct);
	RenderEngine::assignBuffers(referenceSphere);
	RenderEngine::assignBuffers(grids);
	RenderEngine::assignBuffers(cullBounds);
	cullBounds.lineColour = glm::vec3(0.f, 1.f, 0.f);

	// Create geometry for references
	ContentReadWrite::loadOBJ("models/octant.obj", referenceOct);
	RenderEngine::setBufferData(referenceOct);

	ContentReadWrite::loadOBJ("models/sphere.obj", referenceSphere);
	RenderEngine::setBufferData(referenceSphere);

	// Renderable data
	grids.fade = true;

	// Objects to draw
	//objects.push_back(&referenceOctant);
	objects.push_back(&grids);

	// Set up GridInfo
	info.radius = 4.0;
	info.cullMaxRadius = 4.0; info.cullMinRadius = 0.0;
	info.cullMaxLat = M_PI / 2; info.cullMinLat = 0.0;
	info.cullMaxLong = 0.0, info.cullMinLong = -M_PI / 2;
	info.cull = true;

	info.data = SphericalData(0);

	// Renderable for cull bounds
	SphericalGrid b(GridType::NG, info, info.cullMaxRadius, info.cullMinRadius, info.cullMaxLat, info.cullMinLat, info.cullMaxLong, info.cullMinLong);
	cullBounds.verts.clear();
	cullBounds.colours.clear();
	b.createRenderable(cullBounds, 0, DisplayMode::LINES);
	RenderEngine::setBufferData(cullBounds);

	setScheme(Scheme::SDOG);
	updateGrid(0);

	updateReference();

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
		for (glm::vec3 v : grids.verts) {
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
	root->fillData(MAX_LEVEL);
	updateGrid(0);
}

// Updates the level of subdivision being shown
void Program::updateGrid(int levelInc) {
	if (subdivLevel + levelInc < 0 || subdivLevel + levelInc > MAX_LEVEL) {
		return;
	}
	subdivLevel += levelInc;

	grids.verts.clear();
	grids.colours.clear();
	root->createRenderable(grids, subdivLevel, DisplayMode::LINES);
	RenderEngine::setBufferData(grids);
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
	cullBounds.verts.clear();
	cullBounds.colours.clear();
	b.createRenderable(cullBounds, 0, DisplayMode::LINES);
	RenderEngine::setBufferData(cullBounds);

	updateGrid(0);
}

// Toggles between full size and half (real) size reference
void Program::toggleRefSize() {
	fullSizeRef = !fullSizeRef;
	updateReference();
}

// Toggles between octant and full sphere reference
void Program::toggleRefShape() {
	fullSphereRef = !fullSphereRef;
	updateReference();
}

// Toggles drawing of reference
void Program::toggleRef() {

	if (refOn) {
		auto pos = std::find(objects.begin(), objects.end(), &currRef);
		objects.erase(pos);
	}
	else {
		objects.push_back(&currRef);
	}
	refOn = !refOn;
}

// Updates reference to draw
void Program::updateReference() {

	// Set size
	if (fullSizeRef) {
		referenceOct.model = glm::scale(glm::vec3(2.f, 2.f, 2.f));
		referenceSphere.model = glm::scale(glm::vec3(2.f, 2.f, 2.f));
	}
	else {
		referenceOct.model = glm::mat4();
		referenceSphere.model = glm::mat4();
	}

	// Set shape
	if (fullSphereRef) {
		currRef = referenceSphere;
	}
	else {
		currRef = referenceOct;
	}
}

// Turn bounds box drawing on or off
void Program::setBoundsDrawing(bool state) {
	auto pos = std::find(objects.begin(), objects.end(), &cullBounds);

	if (!state && pos != objects.end()) {
		objects.erase(pos);
	}
	else if (state && pos == objects.end()) {
		objects.push_back(&cullBounds);
	}
}

// Toggles bounds culling
void Program::toggleCull() {
	info.cull = !info.cull;
	updateGrid(0);
}
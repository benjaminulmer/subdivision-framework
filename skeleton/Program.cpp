#include "Program.h"

Program::Program() {
	window = nullptr;
	renderEngine = nullptr;
	camera = nullptr;
	root = nullptr;

	maxSubdivLevel = 6;
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
	RenderEngine::assignBuffers(referenceOct, true);
	RenderEngine::assignBuffers(referenceSphere, true);
	referenceOct.textureID = RenderEngine::loadTexture("textures/oct.png");
	referenceSphere.textureID = RenderEngine::loadTexture("textures/sphere.png");

	RenderEngine::assignBuffers(grids, false);
	RenderEngine::assignBuffers(cullBounds, false);
	cullBounds.lineColour = glm::vec3(0.f, 1.f, 0.f);

	// Create geometry for references
	ContentReadWrite::loadOBJ("models/octTex.obj", referenceOct);
	RenderEngine::setBufferData(referenceOct, true);

	ContentReadWrite::loadOBJ("models/sphereTex.obj", referenceSphere);
	RenderEngine::setBufferData(referenceSphere, true);

	// Renderable data
	grids.fade = true;

	// Objects to draw
	//objects.push_back(&referenceOctant);
	objects.push_back(&grids);

	// Set up GridInfo
	info.radius = 4.0;
	info.mode = SubdivisionMode::OCTANT;
	info.cullMaxRadius = 4.0; info.cullMinRadius = 0.0;
	info.cullMaxLat = M_PI / 2; info.cullMinLat = 0.0;
	info.cullMaxLong = 0.0, info.cullMinLong = -M_PI / 2;
	info.cull = true;

	info.data = SphericalData(0);

	// Renderable for cull bounds
	SphericalGrid b(GridType::NG, -1, info, info.cullMaxRadius, info.cullMinRadius, info.cullMaxLat, info.cullMinLat, info.cullMaxLong, info.cullMinLong);
	cullBounds.verts.clear();
	cullBounds.colours.clear();
	b.createRenderable(cullBounds, 0, DisplayMode::LINES);
	RenderEngine::setBufferData(cullBounds, false);

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

// Sets the scheme that will be used for subdivision
void Program::setScheme(Scheme scheme) {
	delete root;
	info.scheme = scheme;
	root = new VolumetricSphericalHierarchy(info);
	root->fillData(maxSubdivLevel);
	updateGrid(0);
}

// Updates the level of subdivision being shown
void Program::updateGrid(int levelInc) {
	if (subdivLevel + levelInc < 0 || subdivLevel + levelInc > maxSubdivLevel) {
		return;
	}
	subdivLevel += levelInc;

	grids.verts.clear();
	grids.colours.clear();

	if (dispMode == DisplayMode::VOLUMES) {
		calculateVolumes(subdivLevel);
	}

	root->createRenderable(grids, subdivLevel, dispMode);
	RenderEngine::setBufferData(grids, false);
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
		if (info.cullMinLat <= -M_PI / 2) info.cullMinLat = -M_PI / 2;
	}
	else if (param == BoundParam::MAX_LONG) {
		info.cullMaxLong -= inc * M_PI / 180;
		if (info.cullMaxLong >= M_PI) info.cullMaxLong = M_PI;
		if (info.cullMaxLong <= info.cullMinLong) info.cullMaxLong = info.cullMinLong;
	}
	else if (param == BoundParam::MIN_LONG) {
		info.cullMinLong += inc * M_PI / 180;
		if (info.cullMinLong >= info.cullMaxLong) info.cullMinLong = info.cullMaxLong;
		if (info.cullMinLong <= -M_PI) info.cullMinLong = -M_PI;
	}

	SphericalGrid b(GridType::NG, -1, info, info.cullMaxRadius, info.cullMinRadius, info.cullMaxLat, info.cullMinLat, info.cullMaxLong, info.cullMinLong);
	cullBounds.verts.clear();
	cullBounds.colours.clear();
	b.createRenderable(cullBounds, 0, DisplayMode::LINES);
	RenderEngine::setBufferData(cullBounds, false);

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

// Sets subdivision mode to be used by the grids
void Program::setSubdivisionMode(SubdivisionMode mode) {
	info.mode = mode;

	if (mode == SubdivisionMode::REP_SLICE) {
		maxSubdivLevel = 8;
	}
	else if (mode == SubdivisionMode::OCTANT) {
		maxSubdivLevel = 6;
	}
	else {
		maxSubdivLevel = 5;
	}
	if (subdivLevel > maxSubdivLevel) {
		subdivLevel = maxSubdivLevel;
	}
	setScheme(info.scheme);
}

// Sets display mode for rendering
void Program::setDisplayMode(DisplayMode mode) {
	dispMode = mode;
	updateGrid(0);
}

// Calculate volume of grids at current level
void Program::calculateVolumes(int level) {

	SubdivisionMode old = info.mode;

	// Representative slice for speed
	std::vector<float> volumes;
	info.mode = SubdivisionMode::REP_SLICE;
	VolumetricSphericalHierarchy* temp = new VolumetricSphericalHierarchy(info);
	root->getVolumes(volumes, level);

	// Find max, min, and avg
	float max = -FLT_MAX;
	float min = FLT_MAX;
	float avg = 0.f;

	for (float v : volumes) {
		avg += v;

		if (v > max) max = v;
		if (v < min) min = v;
	}
	avg /= volumes.size();

	// Stor results for grids to use
	info.volMax = max;
	info.volMin = min;
	info.volAvg = avg;
	info.mode = old;
	root->updateInfo(info);
}
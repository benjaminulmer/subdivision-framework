#include "Program.h"

Program::Program() {
	window = nullptr;
	renderEngine = nullptr;
	camera = nullptr;
	root = nullptr;

	makingSelection = false;

	maxSubdivLevel = 6;
	subdivLevel = 1;
	rotation = false;
	dispMode = DisplayMode::LINES;

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
		system("pause");
		exit(EXIT_FAILURE);
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
	grids.fade = true;
	RenderEngine::assignBuffers(cullBounds, false);

	// Create geometry for references
	ContentReadWrite::loadOBJ("models/octTex.obj", referenceOct);
	RenderEngine::setBufferData(referenceOct, true);

	ContentReadWrite::loadOBJ("models/sphereTex.obj", referenceSphere);
	RenderEngine::setBufferData(referenceSphere, true);

	// Objects to draw initially
	objects.push_back(&grids);

	// Set up GridInfo
	info.radius = 8.0 / 3.0;
	info.mode = SubdivisionMode::FULL;

	info.cull.maxRadius = 8.0 / 3.0; info.cull.minRadius = 0.0;
	info.cull.maxLat = M_PI / 2;     info.cull.minLat = 0.0;
	info.cull.maxLong = 0.0,         info.cull.minLong = -M_PI / 2;

	info.selection.maxRadius = 8.0 / 3.0; info.selection.minRadius = 0.0;
	info.selection.maxLat = M_PI / 2;     info.selection.minLat = 0.0;
	info.selection.maxLong = 0.0,         info.selection.minLong = -M_PI / 2;

	info.culling = false;
	info.data = SphericalData(0);

	// Renderable for cull bounds
	SphericalGrid b(GridType::NG, info, info.cull.maxRadius, info.cull.minRadius, info.cull.maxLat, info.cull.minLat, info.cull.maxLong, info.cull.minLong);
	b.createRenderable(cullBounds, 0, DisplayMode::LINES);
	RenderEngine::setBufferData(cullBounds, false);

	createGrid(Scheme::SDOG);
	updateReference();

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
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			InputHandler::pollEvent(e);
		}

		// Find min and max distance from camera to cell renderable
		// Used for fading
		float max = -FLT_MAX;
		float min = FLT_MAX;

		glm::vec3 cameraPos = camera->getPosition();
		for (glm::vec3 v : grids.verts) {
			float dist = glm::length(cameraPos - v);

			max = (dist > max) ? dist : max;
			min = (dist < min) ? dist : min;
		}
		if (rotation) {
			grids.rot = glm::rotate(grids.rot, 0.005f, glm::vec3(0.f, 1.f, 0.f));
			referenceOct.rot = glm::rotate(referenceOct.rot, 0.005f, glm::vec3(0.f, 1.f, 0.f));
			referenceSphere.rot = glm::rotate(referenceSphere.rot, 0.005f, glm::vec3(0.f, 1.f, 0.f));
			currRef.rot = glm::rotate(currRef.rot, 0.005f, glm::vec3(0.f, 1.f, 0.f));
			cullBounds.rot = glm::rotate(cullBounds.rot, 0.005f, glm::vec3(0.f, 1.f, 0.f));
		}

		renderEngine->render(objects, camera->getLookAt(), max, min);
		SDL_GL_SwapWindow(window);
	}
}

// Sets the scheme that will be used for subdivision
void Program::createGrid(Scheme scheme) {
	delete root;
	info.scheme = scheme;
	root = new VolumetricSphericalHierarchy(info);
	//root->fillData(maxSubdivLevel);
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
	GridBounds& b = (makingSelection) ? info.selection : info.cull;

	// Update proper bound
	if (param == BoundParam::MAX_RADIUS) {
		b.maxRadius -= inc * 0.1;
		if (b.maxRadius >= info.radius) b.maxRadius = info.radius;
		if (b.maxRadius <= b.minRadius) b.maxRadius = b.minRadius;
	}
	else if (param == BoundParam::MIN_RADIUS) {
		b.minRadius += inc * 0.1;
		if (b.minRadius >= b.maxRadius) b.minRadius = b.maxRadius;
		if (b.minRadius <= 0.0) b.minRadius = 0.0;
	}
	else if (param == BoundParam::MAX_LAT) {
		b.maxLat -= inc * M_PI / 180;
		if (b.maxLat >= M_PI / 2) b.maxLat = M_PI / 2;
		if (b.maxLat <= b.minLat) b.maxLat = b.minLat;
	}
	else if (param == BoundParam::MIN_LAT) {
		b.minLat += inc * M_PI / 180;
		if (b.minLat >= b.maxLat) b.minLat = b.maxLat;
		if (b.minLat <= -M_PI / 2) b.minLat = -M_PI / 2;
	}
	else if (param == BoundParam::MAX_LONG) {
		b.maxLong -= inc * M_PI / 180;
		if (b.maxLong >= M_PI) b.maxLong = M_PI;
		if (b.maxLong <= b.minLong) b.maxLong = b.minLong;
	}
	else if (param == BoundParam::MIN_LONG) {
		b.minLong += inc * M_PI / 180;
		if (b.minLong >= b.maxLong) b.minLong = b.maxLong;
		if (b.minLong <= -M_PI) b.minLong = -M_PI;
	}

	if (makingSelection) {
		cullBounds.lineColour = glm::vec3(0.8f, 0.f, 0.f);
	}
	else if (info.culling) {
		cullBounds.lineColour = glm::vec3(0.05f, 0.05f, 1.0f);
	}
	else {
		cullBounds.lineColour = glm::vec3(0.14, 0.f, 0.48f);
	}

	SphericalGrid r(GridType::NG, info, b.maxRadius, b.minRadius, b.maxLat, b.minLat, b.maxLong, b.minLong);
	cullBounds.verts.clear();
	cullBounds.colours.clear();
	r.createRenderable(cullBounds, 0, DisplayMode::LINES);
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
		if (info.radius == 4.0) {
			referenceOct.scale = glm::scale(glm::vec3(2.f, 2.f, 2.f));
			referenceSphere.scale = glm::scale(glm::vec3(2.f, 2.f, 2.f));
		}
		else {
			float scale = info.radius / 2.0f;
			referenceOct.scale = glm::scale(glm::vec3(scale, scale, scale));
			referenceSphere.scale = glm::scale(glm::vec3(scale, scale, scale));
		}
	}
	else {
		referenceOct.scale = glm::mat4();
		referenceSphere.scale = glm::mat4();
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
	if (makingSelection && !state) {
		return;
	}

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
	info.culling = !info.culling;
	updateBounds(BoundParam::MAX_RADIUS, 0);
	updateGrid(0);
}

// Toggles auto rotation
void Program::toggleRotation() {
	rotation = !rotation;
}

// Toggles location of surface between 0.5 and 0.75
void Program::toggleSurfaceLocation() {
	if (info.radius == 4.0) {
		info.radius = 8.0 / 3.0;
	}
	else {
		info.radius = 4.0;
	}
	createGrid(info.scheme);
	updateReference();
}

// Sets subdivision mode to be used by the grids
void Program::setSubdivisionMode(SubdivisionMode mode) {
	info.mode = mode;

	if (mode == SubdivisionMode::REP_SLICE) {
		maxSubdivLevel = 8;
	}
	else if (mode == SubdivisionMode::SELECTION) {
		maxSubdivLevel = 6;
	}
	else {//(mode == SubdivisionMode::FULL)
		maxSubdivLevel = 5;
	}
	if (subdivLevel > maxSubdivLevel) {
		subdivLevel = maxSubdivLevel;
	}
	createGrid(info.scheme);
}

// Sets display mode for rendering
void Program::setDisplayMode(DisplayMode mode) {
	dispMode = mode;
	updateGrid(0);
}

// Turns selection mode on or off
void Program::toggleMakingSelection() {
	if (!makingSelection) {
		makingSelection = true;
		updateBounds(BoundParam::MAX_RADIUS, 0);
		setBoundsDrawing(true);
	}
	else {
		makingSelection = false;
		updateBounds(BoundParam::MAX_RADIUS, 0);
		setBoundsDrawing(false);
		createGrid(info.scheme);
	}
}


// Calculate volume of grids at current level
void Program::calculateVolumes(int level) {
	SubdivisionMode old = info.mode;

	// Representative slice for speed
	std::vector<float> volumes;
	info.mode = SubdivisionMode::REP_SLICE;
	VolumetricSphericalHierarchy* temp = new VolumetricSphericalHierarchy(info);
	temp->getVolumes(volumes, level);
	delete temp;

	// Find max, min, and avg
	float max = -FLT_MAX;
	float min = FLT_MAX;
	float avg = 0.f;

	std::cout << volumes.size() << std::endl;
	std::cout << VolumetricSphericalHierarchy::numberOfCells(level) << std::endl;

	for (float v : volumes) {
		avg += v;

		if (v > max) max = v;
		if (v < min) min = v;
	}
	avg /= volumes.size();
	std::cout << max / min << std::endl;

	// Store results for grids to use
	info.volMax = max;
	info.volMin = min;
	info.volAvg = avg;
	info.mode = old;
}
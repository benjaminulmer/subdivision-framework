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
	RenderEngine::assignBuffers(referenceOctant);
	RenderEngine::setBufferData(referenceOctant);

	referenceOctant.single = true;
	referenceOctant.colour = glm::vec3(1.f, 1.f, 1.f);
	for (glm::vec3 v : referenceOctant.verts) {
		referenceOctant.colours.push_back(glm::vec4(0.f, 0.f, 0.f, 0.f));
	}

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

	//std::vector<float> volumes;
	//sdog.getVolumes(volumes, 4);

	//float max = -FLT_MAX;
	//float min = FLT_MAX;

	//for (float v : volumes) {
	//	max = (v > max) ? v : max;
	//	min = (v < min) ? v : min;
	//}
	//std::cout << "Max: " << max << " Min: " << min << std::endl;

	int count = 0;
	while (true) {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			InputHandler::pollEvent(e);
		}

		updateSubdivisionLevel(0);
		renderEngine->render(objects, camera->getLookAt());
		SDL_GL_SwapWindow(window);

		count++;
	}

	// Clean up, program needs to exit
	SDL_Quit();
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

	glm::vec3 cameraPos = camera->getPosition();

	float max = -FLT_MAX;
	float min = FLT_MAX;

	for (glm::vec3 v : cells.verts) {
		float dist = glm::length(cameraPos - v);

		max = (dist > max) ? dist : max;
		min = (dist < min) ? dist : min;
	}

	for (glm::vec3 v : cells.verts) {
		float dist = glm::length(cameraPos - v);

		float norm = (dist - min) / (max - min);
		norm = 1.f - 2.5f * (norm - 0.f);

		if (norm < 0.f) norm = 0.f;
		if (norm > 1.f) norm = 1.f;

		cells.colours.push_back(glm::vec4(0.f, 0.f, 0.f, norm));
	}

	cells.single = false;

	RenderEngine::assignBuffers(cells);
	RenderEngine::setBufferData(cells);
}
#include "Program.h"

Program::Program() {
	window = nullptr;
	renderEngine = nullptr;
	camera = nullptr;

	mouseX = mouseY = 0;
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
	Renderable a;
	ContentReadWrite::loadOBJ("models/oct.obj", a);
	RenderEngine::assignBuffers(a);
	RenderEngine::setBufferData(a);
	a.colour = glm::vec3(1.f, 1.f, 1.f);

	Sdog sdog(2.0, 0.5, 0.5);
	Renderable b;
	sdog.createRenderable(b, 3);
	RenderEngine::assignBuffers(b);
	RenderEngine::setBufferData(b);
	b.colour = glm::vec3(0.f, 0.f, 0.f);

	objects.push_back(a);
	objects.push_back(b);

	std::vector<float> volumes;
	sdog.getVolumes(volumes, 1);

	float max = -FLT_MAX;
	float min = FLT_MAX;

	for (float v : volumes) {
		std::cout << v << std::endl;

		max = (v > max) ? v : max;
		min = (v < min) ? v : min;
	}
	std::cout << "Max: " << max << " Min: " << min << std::endl;

	int count = 0;
	while (true) {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			InputHandler::pollEvent(e);
		}

		Sdog sdog2 = Sdog(2.0, 0.5, 0.5);
		sdog2.subdivideTo(5);

		renderEngine->render(objects, camera->getLookAt());
		SDL_GL_SwapWindow(window);

		count++;
	}

	// Clean up, program needs to exit
	SDL_Quit();
}
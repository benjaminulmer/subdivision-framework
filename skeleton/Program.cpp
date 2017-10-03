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

	window = SDL_CreateWindow("CPSC589 Project", 10, 30, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
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
	ContentReadWrite::loadOBJ("models/sphere.obj", a);
	renderEngine->assignBuffers(a);
	renderEngine->setBufferData(a);

	Renderable b;
	Geometry::createArc(glm::vec3(1.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 0.f, 0.f), b);
	renderEngine->assignBuffers(b);
	renderEngine->setBufferData(b);

	Renderable b1;
	Geometry::createArc(glm::vec3(1.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 0.f, 0.f), b1);
	renderEngine->assignBuffers(b1);
	renderEngine->setBufferData(b1);

	Renderable b2;
	Geometry::createArc(glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 0.f, 0.f), b2);
	renderEngine->assignBuffers(b2);
	renderEngine->setBufferData(b2);

	float root2on2 = 0.70710678f;

	Renderable b4;
	glm::vec3 end = Geometry::geomSlerp(glm::vec3(root2on2, 0.f, root2on2), glm::vec3(0.f, 1.f, 0.f), 0.5f);
	Geometry::createArc(glm::vec3(root2on2, 0.f, root2on2), end, glm::vec3(0.f, 0.f, 0.f), b4);
	renderEngine->assignBuffers(b4);
	renderEngine->setBufferData(b4);

	//Renderable b3;
	//Geometry::createArc(glm::vec3(root2on2, root2on2, 0.f), glm::vec3(0.f, root2on2, root2on2), glm::vec3(0.f, root2on2, 0.f), b3);
	//renderEngine->assignBuffers(b3);
	//renderEngine->setBufferData(b3);

	Renderable b3;
	Geometry::createArc(glm::vec3(root2on2, 0.f, -root2on2), end, glm::vec3(0.f, 0.f, 0.f), b3);
	renderEngine->assignBuffers(b3);
	renderEngine->setBufferData(b3);

	Renderable l1;
	Geometry::createLine(glm::vec3(0.f, 0.f, 0.f), glm::vec3(1.f, 0.f, 0.f), l1);
	renderEngine->assignBuffers(l1);
	renderEngine->setBufferData(l1);

	Renderable l2;
	Geometry::createLine(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f), l2);
	renderEngine->assignBuffers(l2);
	renderEngine->setBufferData(l2);

	Renderable l3;
	Geometry::createLine(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 1.f), l3);
	renderEngine->assignBuffers(l3);
	renderEngine->setBufferData(l3);

	//objects.push_back(a);
	objects.push_back(b);
	objects.push_back(b1);
	objects.push_back(b2);
	objects.push_back(l1);
	objects.push_back(l2);
	objects.push_back(l3);

	objects.push_back(b3);
	objects.push_back(b4);

	while (true) {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			InputHandler::pollEvent(e);
		}
		renderEngine->render(objects, camera->getLookAt());

		SDL_GL_SwapWindow(window);
	}

	// Clean up, program needs to exit
	SDL_Quit();
}
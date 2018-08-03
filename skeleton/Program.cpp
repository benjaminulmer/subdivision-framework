#include "Program.h"

Program::Program() {
	window = nullptr;
	renderEngine = nullptr;
	camera = nullptr;

	mouseX = mouseY = 0;
	width = height = 1000;
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

void Program::setMousePos(int x, int y) {
	mouseX = x;
	mouseY = y;
}

void Program::_3DPick() {
	float x = (2.0f * mouseX) / width - 1.0f;
	float y = 1.0f - (2.0f * mouseY) / height;
	float z = 1.0f;
	glm::vec3 ray_nds = glm::vec3(x, y, z);
	glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.0, 1.0);
	glm::vec4 ray_eye = glm::inverse(renderEngine->projection) * ray_clip;
	ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);
	glm::vec3 ray_wor = (glm::inverse(camera->getLookAt()) * ray_eye);
	// don't forget to normalise the vector at some point
	ray_wor = glm::normalize(ray_wor);

	float t0, t1; // solutions for t if the ray intersects  
	glm::vec3 L = camera->getPosition();
	float a = glm::dot(ray_wor, ray_wor);
	float b = 2 * glm::dot(ray_wor, L);
	float c = glm::dot(L, L) - 1.0f;
	float discr = b * b - 4 * a * c;
	if (discr < 0) {
		return;
	} else if (discr == 0) {
		t0 = t1 = -0.5 * b / a;
	} else {
		float q = (b > 0) ?
			-0.5 * (b + sqrt(discr)) :
			-0.5 * (b - sqrt(discr));
		t0 = q / a;
		t1 = c / q;
	}
	if (t0 > t1) std::swap(t0, t1);

	if (t0 < 0) {
		t0 = t1; // if t0 is negative, let's use t1 instead 
		if (t0 < 0) {
			return; // both t0 and t1 are negative
		}
	}

	glm::vec3 hitPos = camera->getPosition() + t0 * ray_wor;
	double lon = atan2(hitPos.y, hitPos.x);
	double lat = (M_PI / 2.0) - acos(hitPos.z);
	myDGGS.select(d128Vec2(lat, lon));
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

	unsigned int ncols;
	unsigned int nrows;
	double xllcorner;
	double yllcorner;
	double cellsize;

	std::vector<double>* data = ContentReadWrite::loadPopulationData("gpw_v4_population_density_rev10_2015_1_deg.asc", &ncols, &nrows, &xllcorner, &yllcorner, &cellsize);

	Renderable equalAreaTrianglePts;
	EqualAreaTetrahedron sphere = EqualAreaTetrahedron(equalAreaTrianglePts, 3);

	myDGGS = DGGS(sphere.vertsRhombicTriacontahedron, sphere.verts, sphere.faces);

	DGGSDB myDB = DGGSDB("DGGS.db", &myDGGS, data, ncols, nrows, xllcorner, yllcorner, cellsize, equalAreaTrianglePts);

	RenderEngine::assignBuffers(equalAreaTrianglePts);
	RenderEngine::setBufferData(equalAreaTrianglePts);
	equalAreaTrianglePts.colour = glm::vec3(1.f, 1.f, 1.f);
	equalAreaTrianglePts.drawMode = GL_TRIANGLES;
	objects.push_back(equalAreaTrianglePts);

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
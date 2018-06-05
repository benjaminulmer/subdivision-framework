#define _USE_MATH_DEFINES
#include "Program.h"

#include <GL/glew.h>
#include <glm/gtx/intersect.hpp>
#include <SDL2/SDL_opengl.h>

#include <algorithm>
#include <cmath>
#include <limits>
#include <iostream>

#include "Constants.h"
#include "WindGrid.h"
#include "ContentReadWrite.h"
#include "InputHandler.h"
#include "AirSigmet.h"
#include "SdogCell.h"
#include "SphCoord.h"
#include "Geometry.h"

Program::Program() {

	window = nullptr;
	renderEngine = nullptr;
	camera = nullptr;
	dataBase = nullptr;

	longRot = 0;
	latRot = 0;

	width = height = 800;
}

#define AirSigmetInsert true
#define WindInsert false

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
	renderEngine = new RenderEngine(window);
	InputHandler::setUp(camera, renderEngine, this);



	//scale = 7379336.7465132149;
	//camera->setScale(scale);
	//updateRotation(365, 369, 363, 365, false);


	// Assign buffers
	RenderEngine::assignBuffers(cells, false);
	RenderEngine::assignBuffers(polys, false);
	RenderEngine::assignBuffers(bound, false);
	RenderEngine::assignBuffers(wind, false);

	// Set starting radius
	scale = 1.f;
	radius = RADIUS_EARTH_KM * 4.f / 3.f;

	// Load coastline vector data
	rapidjson::Document cl = ContentReadWrite::readJSON("data/coastlines.json");
	coastLines = Renderable(cl);
	RenderEngine::assignBuffers(coastLines, false);
	//float s = 1.f + std::numeric_limits<float>::epsilon();
	//coastLines.model = glm::scale(glm::vec3(s * scale, s * scale, s * scale));

	// Create grid database connection and load data sets
	dataBase = new SdogDB("test.db", radius);

	if (AirSigmetInsert) {
		insertAirSigmets();
	}
	if (WindInsert) {
		insertWind();
	}

	// Objects to draw initially
	objects.push_back(&coastLines);
	objects.push_back(&cells);
	objects.push_back(&polys);
	objects.push_back(&bound);
	objects.push_back(&wind);

	// Draw stuff
	cells.drawMode = GL_LINES;
	bound.drawMode = GL_LINES;
	wind.drawMode = GL_POINTS;
	polys.drawMode = GL_LINES;

	//airSigRender1();
	//windRender1();

	cells.doubleToFloats();
	bound.doubleToFloats();
	polys.doubleToFloats();
	wind.doubleToFloats();
	coastLines.doubleToFloats();

	RenderEngine::setBufferData(cells, false);
	RenderEngine::setBufferData(bound, false);
	RenderEngine::setBufferData(polys, false);
	RenderEngine::setBufferData(wind, false);
	RenderEngine::setBufferData(coastLines, false);

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
		std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		exit(EXIT_FAILURE);
	}

	SDL_GLContext context = SDL_GL_CreateContext(window);
	if (context == NULL) {
		std::cout << "OpenGL context could not be created! SDL Error: " << SDL_GetError() << std::endl;
	}
	SDL_GL_SetSwapInterval(1); // Vsync on
}

// Reads AirSigmets from file, inserts into grid, and then inserts into DB
void Program::insertAirSigmets() {

	//rapidjson::Document sig = ContentReadWrite::readJSON("data/sigmet.json");
	//std::vector<AirSigmet> airSigmets;
	//AirSigmet::readFromJson(sig, airSigmets);

	//int c = 0;
	//for (const AirSigmet& a : airSigmets) {
	//	std::cout << "AirSigmet " << c << " of " << airSigmets.size() << std::endl;
	//	std::vector<std::string> interior, boundary;
	//	a.gridInsertion(radius, 10, interior, boundary);
	//	dataBase->insertAirSigmet(interior, boundary, a);
	//	c++;
	//}

	SphCoord wayPoint(51.08, -114.1292294, false);
	wind.verts.push_back(wayPoint.toCartesian(RADIUS_EARTH_KM));
	wind.colours.push_back(glm::vec3(1.f, 0.f, 0.f));

	std::vector<std::string> interior, boundary;
	AirSigmet a;
	a.polygon.push_back(SphCoord(51.0798777, -114.1292294, false));
	a.polygon.push_back(SphCoord(51.0798757, -114.1289109, false));
	a.polygon.push_back(SphCoord(51.0798094, -114.1289093, false));
	a.polygon.push_back(SphCoord(51.0797771, -114.1288856, false));
	a.polygon.push_back(SphCoord(51.0796888, -114.1288963, false));
	a.polygon.push_back(SphCoord(51.0796550, -114.1288985, false));
	a.polygon.push_back(SphCoord(51.0795592, -114.1288985, false));
	a.polygon.push_back(SphCoord(51.0794433, -114.1288941, false));
	a.polygon.push_back(SphCoord(51.0793132, -114.1288856, false));
	a.polygon.push_back(SphCoord(51.0791902, -114.1287488, false));
	a.polygon.push_back(SphCoord(51.0791544, -114.1288303, false));
	a.polygon.push_back(SphCoord(51.0791790, -114.1290326, false));
	a.polygon.push_back(SphCoord(51.0792537, -114.1290988, false));
	a.polygon.push_back(SphCoord(51.0792437, -114.1293121, false));
	a.polygon.push_back(SphCoord(51.0791826, -114.1292773, false));
	a.polygon.push_back(SphCoord(51.0791322, -114.1295018, false));
	a.polygon.push_back(SphCoord(51.0791941, -114.1295727, false));
	a.polygon.push_back(SphCoord(51.0790480, -114.1296606, false));
	a.polygon.push_back(SphCoord(51.0790886, -114.1300041, false));
	a.polygon.push_back(SphCoord(51.0792530, -114.1300216, false));
	a.polygon.push_back(SphCoord(51.0793164, -114.1300339, false));
	a.polygon.push_back(SphCoord(51.0793268, -114.1299840, false));
	a.polygon.push_back(SphCoord(51.0793504, -114.1299812, false));
	a.polygon.push_back(SphCoord(51.0793944, -114.1299808, false));
	a.polygon.push_back(SphCoord(51.0795293, -114.1299796, false));
	a.polygon.push_back(SphCoord(51.0795291, -114.1299226, false));
	a.polygon.push_back(SphCoord(51.0795725, -114.1299245, false));
	a.polygon.push_back(SphCoord(51.0795734, -114.1292100, false));
	a.polygon.push_back(SphCoord(51.0796241, -114.1292161, false));
	a.polygon.push_back(SphCoord(51.0796218, -114.1293551, false));
	a.polygon.push_back(SphCoord(51.0796469, -114.1293566, false));
	a.polygon.push_back(SphCoord(51.0796486, -114.1293960, false));
	a.polygon.push_back(SphCoord(51.0797354, -114.1293984, false));
	a.polygon.push_back(SphCoord(51.0797348, -114.1293601, false));
	a.polygon.push_back(SphCoord(51.0797601, -114.1293612, false));
	a.polygon.push_back(SphCoord(51.0797595, -114.1292704, false));
	a.polygon.push_back(SphCoord(51.0797814, -114.1292706, false));
	a.polygon.push_back(SphCoord(51.0798077, -114.1292708, false));
	a.polygon.push_back(SphCoord(51.0798065, -114.1292260, false));
	a.polygon.push_back(SphCoord(51.0798065, -114.1292260, false));
	a.minAltKM = 0.0; a.maxAltKM = 0.02;

	int depth = 25;
	a.gridInsertion(radius, depth, interior, boundary);
	std::cout << boundary.size() << " : " << interior.size() << std::endl;

	polys.lineColour = glm::vec3(0.f, 1.f, 0.f);
	polys.drawMode = GL_LINES;
	for (int i = 0; i < a.polygon.size(); i++) {
		glm::vec3 v1 = a.polygon[i].toCartesian(RADIUS_EARTH_KM);
		glm::vec3 v2 = a.polygon[(i + 1) % a.polygon.size()].toCartesian(RADIUS_EARTH_KM);
		Geometry::createArcR(v1, v2, glm::dvec3(0.0), polys);
	}

	for (const std::string& code : interior) {

		SdogCell cell(code, radius);
		if (cell.getMinRad() > RADIUS_EARTH_KM + 0.00301) continue;
		//cell.addToRenderable(cells, glm::vec3(1.f, 1.f, 0.5f), false);
	}
	for (const std::string& code : boundary) {

		if (code.length() < depth + 1) continue;

		SdogCell cell(code, radius);
		//if (cell.getMinRad() > RADIUS_EARTH_KM + 0.00301 || cell.getMinRad() < RADIUS_EARTH_KM + 0.0001) continue;
		cell.addToRenderable(bound, glm::vec3(0.2f, 0.2f, 0.5f), false);
	}
}

// Reads wind data from file, inserts into grid, and then inserts into DB
void Program::insertWind() {

	rapidjson::Document wind = ContentReadWrite::readJSON("data/wind-25.json");
	std::vector<WindGrid> grids;
	WindGrid::readFromJson(wind, grids);

	std::vector<std::pair<std::string, glm::vec2>> codes;
	WindGrid::gridInsertion(radius, 9, grids, codes);
	dataBase->insertWindData(codes);
}

void Program::airSigRender1() {

	std::vector<AirSigmetCells> data;
	dataBase->getAirSigmetCells(data);

	int i = -1;
	for (const AirSigmetCells& datum : data) {
		i++;
		if (i != 11) continue;

		polys.lineColour = glm::vec3(0.f, 1.f, 0.f);
		polys.drawMode = GL_LINES;
		for (int i = 0; i < datum.airSigmet.polygon.size(); i++) {
			glm::vec3 v1 = datum.airSigmet.polygon[i].toCartesian(datum.airSigmet.maxAltKM * 2.2 + RADIUS_EARTH_KM);
			glm::vec3 v2 = datum.airSigmet.polygon[(i + 1) % datum.airSigmet.polygon.size()].toCartesian(datum.airSigmet.maxAltKM * 2.2 + RADIUS_EARTH_KM);
			Geometry::createArcR(v1, v2, glm::vec3(), polys);
		}
		RenderEngine::setBufferData(polys, false);


		for (const std::string& code : datum.interior) {

			SdogCell cell(code, radius);
			cell.addToRenderable(cells, glm::vec3(1.f, 1.f, 0.5f), true);
		}

		for (const std::string& code : datum.boundary) {

			if (code.length() < 11) continue;

			SdogCell cell(code, radius);
			cell.addToRenderable(bound, glm::vec3(0.2f, 0.2f, 0.5f), true);
		}
	}
}

void Program::windRender1() {

	std::vector<std::pair<std::string, glm::vec2>> codes;
	dataBase->getWindCells(codes);

	float max = -1.f;
	float min = 9999999.f;

	double minRad = altToAbs(0.0);
	double maxRad = altToAbs(10.0);

	for (const std::pair<std::string, glm::vec2>& p : codes) {

		SdogCell cell(p.first, radius);
		if (cell.getMinRad() < minRad || cell.getMinRad() > maxRad) continue;

		float mag = glm::length(p.second);
		if (mag > max) max = mag;
		if (mag < min) min = mag;
	}

	for (const std::pair<std::string, glm::vec2>& p : codes) {

		SdogCell cell(p.first, radius);
		if (cell.getMinRad() < minRad || cell.getMinRad() > maxRad) continue;

		float mag = glm::length(p.second);
		float norm = (mag - min) / (max - min);

		if (norm < 0.4f) continue;

		float col = (cell.getMinRad() - RADIUS_EARTH_KM) / 120.f;
		cell.addToRenderable(cells, glm::vec3(norm, col, col), true);
	}
}

// Main loop
void Program::mainLoop() {

	while (true) {

		// Process all SDL events
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			InputHandler::pollEvent(e);
		}

		float far = glm::length(camera->getPosition() - glm::dvec3(0.0));
		//info.frust = Frustum(*camera, renderEngine->getFovY(), renderEngine->getAspectRatio(), renderEngine->getNear(), far);

		// Find min and max distance from camera to cell renderable - used for fading effect
		glm::vec3 cameraPos = camera->getPosition();
		float max = glm::length(cameraPos) + RADIUS_EARTH_VIEW;
		float min = glm::length(cameraPos) - RADIUS_EARTH_VIEW;

		glm::dmat4 worldModel(1.f);
		double s = scale * (1.f / RADIUS_EARTH_KM) * RADIUS_EARTH_VIEW;
		worldModel = glm::scale(worldModel, glm::dvec3(s, s, s));
		worldModel = glm::rotate(worldModel, latRot, glm::dvec3(-1.f, 0.f, 0.f));
		worldModel = glm::rotate(worldModel, longRot, glm::dvec3(0.f, 1.f, 0.f));

		renderEngine->render(objects, (glm::dmat4)camera->getLookAt() * worldModel, max, min);
		SDL_GL_SwapWindow(window);
	}

	delete dataBase;
}

// Updates camera rotation
// Locations are in pixel coordinates
void Program::updateRotation(int oldX, int newX, int oldY, int newY, bool skew) {

	glm::dmat4 projView = renderEngine->getProjection() * camera->getLookAt();
	glm::dmat4 invProjView = glm::inverse(projView);

	double oldXN = (2.0 * oldX) / (width) - 1.0; 
	double oldYN = (2.0 * oldY) / (height) - 1.0;
	oldYN *= -1.0;

	double newXN = (2.0 * newX) / (width) - 1.0;
	double newYN = (2.0 * newY) / (height) - 1.0;
	newYN *= -1.0;

	glm::dvec4 worldOld(oldXN, oldYN, -1.0, 1.0);
	glm::dvec4 worldNew(newXN, newYN, -1.0, 1.0);

	worldOld = invProjView * worldOld; 
	worldOld /= worldOld.w;

	worldNew = invProjView * worldNew;
	worldNew /= worldNew.w;

	glm::dvec3 rayO = camera->getPosition();
	glm::dvec3 rayDOld = glm::normalize(glm::dvec3(worldOld) - rayO);
	glm::dvec3 rayDNew = glm::normalize(glm::dvec3(worldNew) - rayO);
	double sphereRad = RADIUS_EARTH_VIEW * scale;
	glm::dvec3 sphereO = glm::dvec3(0.0);

	glm::dvec3 iPosOld, iPosNew, iNorm;

	if (glm::intersectRaySphere(rayO, rayDOld, sphereO, sphereRad, iPosOld, iNorm) && 
			glm::intersectRaySphere(rayO, rayDNew, sphereO, sphereRad, iPosNew, iNorm)) {

		double longOld = atan2(iPosOld.x, iPosOld.z);
		double latOld = M_PI_2 - acos(iPosOld.y / sphereRad);

		double longNew = atan2(iPosNew.x, iPosNew.z);
		double latNew = M_PI_2 - acos(iPosNew.y / sphereRad);


		std::cout << "pixel old: (" << oldXN << ", " << oldYN << ")\t" << "pixel new: (" << newXN << ", " << newYN << ")" << std::endl;
		

		if (skew) {
			camera->updateLatitudeRotation(latNew - latOld);
		}
		else {
			latRot += latNew - latOld;
			longRot += longNew - longOld;
		}
	}
	else {
		std::cout << "no ray intersect" << std::endl;
		glm::intersectRaySphere(rayO, rayDOld, sphereO, sphereRad, iPosOld, iNorm);
	}
}

// Changes scale of model
void Program::updateScale(int inc) {

	if (inc < 0) {
		scale /= 1.4f;
	}
	else {
		scale *= 1.4f;
	}
	camera->setScale(scale);
}
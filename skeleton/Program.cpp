#define _USE_MATH_DEFINES
#include "Program.h"

#include <GL/glew.h>
#include <glm/gtx/intersect.hpp>
#include <SDL2/SDL_opengl.h>

#include <algorithm>
#include <cmath>
#include <limits>
#include <iostream>
#include <map>

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

#define AirSigmetInsert false
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

	// Assign buffers
	RenderEngine::assignBuffers(cells, false);
	RenderEngine::assignBuffers(polys, false);
	RenderEngine::assignBuffers(stormPolys, false);
	RenderEngine::assignBuffers(wind, false);

	// Set starting radius
	scale = 1.f;
	radius = RADIUS_EARTH_KM * 4.f / 3.f;

	// Load coastline vector data
	rapidjson::Document cl = ContentReadWrite::readJSON("data/coastlines.json");
	coastLines = Renderable(cl);
	RenderEngine::assignBuffers(coastLines, false);
	float s = 1.f + std::numeric_limits<float>::epsilon();
	coastLines.model = glm::scale(glm::vec3(s * scale, s * scale, s * scale));

	// Create grid database connection and load data sets
	dataBase = new SdogDB("test.db", radius);

	std::vector<AirSigmetCells> data;
	dataBase->getAirSigmetCells(data);

	if (AirSigmetInsert) {
		insertAirSigmets();
	}
	if (WindInsert) {
		insertWind();
	}

	// Draw stuff
	cells.drawMode = GL_TRIANGLES;
	wind.drawMode = GL_TRIANGLES;
	polys.drawMode = GL_LINES;

	//airSigRender1();
	//windRender1();

	// Objects to draw initially
	objects.push_back(&coastLines);
	objects.push_back(&cells);
	objects.push_back(&polys);
	objects.push_back(&stormPolys);
	objects.push_back(&wind);

	for (Renderable* r : bounds) {
		objects.push_back(r);
	}

	RenderEngine::setBufferData(cells, false);
	RenderEngine::setBufferData(polys, false);
	RenderEngine::setBufferData(stormPolys, false);
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

	rapidjson::Document sig = ContentReadWrite::readJSON("data/testSigmet.json");
	std::vector<AirSigmet> airSigmets;
	AirSigmet::readFromJson(sig, airSigmets);

	int c = 0;
	for (const AirSigmet& a : airSigmets) {
		std::cout << "AirSigmet " << c << " of " << airSigmets.size() << std::endl;
		std::vector<std::string> interior, boundary;
		a.gridInsertion(radius, 10, interior, boundary);
		dataBase->insertAirSigmet(interior, boundary, a);
		c++;
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

	int count = 0;

	for (const AirSigmetCells& datum : data) {

		count++;
		float alpha;

		Renderable* r = new Renderable();
		bounds.push_back(r);
		RenderEngine::assignBuffers(*r, false);

		// set sigmet type
		if (datum.airSigmet.hazard == HazardType::CONVECTIVE) {
			//alpha = (float)datum.airSigmet.severity / 4.f / 2.f;
			//if (alpha == 0.f) alpha = 0.1f;
			alpha = 0.8f;

			float red = ((float)datum.airSigmet.severity / 4.f / 2.f) + 0.5f;
			r->renderColour = glm::vec3(red, 0.2f, 0.2f);
		}
		else {
			alpha = 1.f;
			r->renderColour = glm::vec3(0.2f, 0.2f, 0.5f);
		}

		Renderable* drawPolys;
		if (datum.airSigmet.hazard == HazardType::CONVECTIVE) drawPolys = &stormPolys;
		else drawPolys = &polys;

		polys.renderColour = glm::vec3(0.f, 1.f, 0.f);
		polys.drawMode = GL_LINES;

		stormPolys.renderColour = glm::vec3(1.f, 0.f, 0.f);
		stormPolys.drawMode = GL_LINES;

		glm::vec3 center(0.f, 0.f, 0.f);

		for (int j = 0; j < datum.airSigmet.polygon.size(); j++) {

			center += datum.airSigmet.polygon[j].toCartesian(datum.airSigmet.minAltKM);

			glm::vec3 v1, v2;

			glm::vec3 maxV1 = datum.airSigmet.polygon[j].toCartesian(altToAbs(datum.airSigmet.maxAltKM));
			glm::vec3 maxV2 = datum.airSigmet.polygon[(j + 1) % datum.airSigmet.polygon.size()].toCartesian(altToAbs(datum.airSigmet.maxAltKM));

			Geometry::createArcR(maxV1, maxV2, glm::vec3(), *drawPolys);

			v1 = datum.airSigmet.polygon[j].toCartesian(altToAbs(datum.airSigmet.minAltKM));
			v2 = datum.airSigmet.polygon[(j + 1) % datum.airSigmet.polygon.size()].toCartesian(altToAbs(datum.airSigmet.minAltKM));

			drawPolys->verts.push_back(v1);
			drawPolys->verts.push_back(maxV1);

			drawPolys->verts.push_back(v2);
			drawPolys->verts.push_back(maxV2);

			drawPolys->colours.push_back(drawPolys->renderColour);
			drawPolys->colours.push_back(drawPolys->renderColour);
			drawPolys->colours.push_back(drawPolys->renderColour);
			drawPolys->colours.push_back(drawPolys->renderColour);
			/*}
			else {
				v1 = datum.airSigmet.polygon[j].toCartesian(datum.airSigmet.maxAltKM * 2.2 + RADIUS_EARTH_KM);
				v2 = datum.airSigmet.polygon[(j + 1) % datum.airSigmet.polygon.size()].toCartesian(datum.airSigmet.maxAltKM * 2.2 + RADIUS_EARTH_KM);
			}*/
			Geometry::createArcR(v1, v2, glm::vec3(), *drawPolys);
		}
		for (const std::string& code : datum.boundary) {
			if (code.length() < 11) continue;

			SdogCell cell(code, radius);

			// Get neighbors, check which ones are part of sigmet

			std::vector<std::string> neighbour;

			// up direction
			cell.getUpNeighbours(neighbour);

			bool hasSig = false;

			for (std::string n : neighbour) {
				std::vector<AirSigmet> sigs;
				dataBase->getAirSigmetForCell(n, sigs);
					
				for (AirSigmet a : sigs) {
					if (a.hazard == datum.airSigmet.hazard) {
						hasSig = true;
						break;
					}
				}
			}
			cell.renderNeighbors.top = hasSig;
			neighbour.clear();

			// down direction
			cell.getDownNeighbours(neighbour);

			hasSig = false;

			for (std::string n : neighbour) {
				std::vector<AirSigmet> sigs;
				dataBase->getAirSigmetForCell(n, sigs);
					
				for (AirSigmet a : sigs) {
					if (a.hazard == datum.airSigmet.hazard) {
						hasSig = true;
						break;
					}
				}
			}
			cell.renderNeighbors.bottom = hasSig;
			neighbour.clear();

			// left direction
			cell.getLeftNeighbours(neighbour);

			hasSig = false;

			for (std::string n : neighbour) {
				std::vector<AirSigmet> sigs;
				dataBase->getAirSigmetForCell(n, sigs);
					
				for (AirSigmet a : sigs) {
					if (a.hazard == datum.airSigmet.hazard) {
						hasSig = true;
						break;
					}
				}
			}
			cell.renderNeighbors.left = hasSig;
			neighbour.clear();

			// right direction
			cell.getRightNeighbours(neighbour);

			hasSig = false;

			for (std::string n : neighbour) {
				std::vector<AirSigmet> sigs;
				dataBase->getAirSigmetForCell(n, sigs);

				for (AirSigmet a : sigs) {
					if (a.hazard == datum.airSigmet.hazard) {
						hasSig = true;
						break;
					}
				}
			}
			cell.renderNeighbors.right = hasSig;
			neighbour.clear();

			// inside direction
			cell.getInNeighbours(neighbour);

			hasSig = false;

			for (std::string n : neighbour) {
				std::vector<AirSigmet> sigs;
				dataBase->getAirSigmetForCell(n, sigs);
					
				for (AirSigmet a : sigs) {
					if (a.hazard == datum.airSigmet.hazard) {
						hasSig = true;
						break;
					}
				}
			}
			cell.renderNeighbors.inside = hasSig;
			neighbour.clear();

			// outside direction
			cell.getOutNeighbours(neighbour);

			hasSig = false;
			for (std::string n : neighbour) {
				std::vector<AirSigmet> sigs;
				dataBase->getAirSigmetForCell(n, sigs);
					
				for (AirSigmet a : sigs) {
					if (a.hazard == datum.airSigmet.hazard) {
						hasSig = true;
						break;
					}
				}
			}
			cell.renderNeighbors.outside = hasSig;

			cell.addToSigmetRenderable(*r, r->renderColour, &datum, *drawPolys);
			
		}

		/*
		for (const std::string& code : datum.interior) {
			//if (code.length() < 11) continue;

			SdogCell cell(code, radius);

			cell.addToRenderable(*r, r->renderColour, *drawPolys);
		}
		*/

		r->alpha = alpha;
		r->drawMode = GL_TRIANGLES;

		RenderEngine::setBufferData(*r, false);

		std::cout << "Done sigmet " << count << " of " << data.size() << std::endl;
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
		cell.addToRenderable(cells, glm::vec3(norm, col, col), Renderable());
	}
	std::cout << "done" << std::endl;
}

// Main loop
void Program::mainLoop() {

	while (true) {

		// Process all SDL events
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			InputHandler::pollEvent(e);
		}

		float far = glm::length(camera->getPosition() - glm::vec3(0.f, 0.f, 0.f));
		//info.frust = Frustum(*camera, renderEngine->getFovY(), renderEngine->getAspectRatio(), renderEngine->getNear(), far);

		// Find min and max distance from camera to cell renderable - used for fading effect
		glm::vec3 cameraPos = camera->getPosition();
		float max = glm::length(cameraPos) + RADIUS_EARTH_VIEW;
		float min = glm::length(cameraPos) - RADIUS_EARTH_VIEW;

		glm::mat4 worldModel(1.f);
		float s = scale * (1.f / RADIUS_EARTH_KM) * RADIUS_EARTH_VIEW;
		worldModel = glm::scale(worldModel, glm::vec3(s, s, s));
		worldModel = glm::rotate(worldModel, latRot, glm::vec3(-1.f, 0.f, 0.f));
		worldModel = glm::rotate(worldModel, longRot, glm::vec3(0.f, 1.f, 0.f));

		renderEngine->render(objects, camera->getLookAt() * worldModel, max, min);
		SDL_GL_SwapWindow(window);
	}
	delete dataBase;
}

// Updates camera rotation
// Locations are in pixel coordinates
void Program::updateRotation(int oldX, int newX, int oldY, int newY, bool skew) {

	glm::mat4 projView = renderEngine->getProjection() * camera->getLookAt();
	glm::mat4 invProjView = glm::inverse(projView);

	float oldXN = (2.f * oldX) / (width) - 1.f; 
	float oldYN = (2.f * oldY) / (height) - 1.f;
	oldYN *= -1.0;

	float newXN = (2.f * newX) / (width) - 1.f; 
	float newYN = (2.f * newY) / (height) - 1.f;
	newYN *= -1.f;

	glm::vec4 worldOld(oldXN, oldYN, -1.f, 1.f);
	glm::vec4 worldNew(newXN, newYN, -1.f, 1.f);

	worldOld = invProjView * worldOld; 

	worldOld.x /= worldOld.w;
	worldOld.y /= worldOld.w;
	worldOld.z /= worldOld.w;

	worldNew = invProjView * worldNew;

	worldNew.x /= worldNew.w;
	worldNew.y /= worldNew.w;
	worldNew.z /= worldNew.w;

	glm::vec3 rayO = camera->getPosition();
	glm::vec3 rayDOld = glm::normalize(glm::vec3(worldOld) - rayO);
	glm::vec3 rayDNew = glm::normalize(glm::vec3(worldNew) - rayO);
	float sphereRad = RADIUS_EARTH_VIEW * scale;
	glm::vec3 sphereO = glm::vec3(0.f, 0.f, 0.f);

	glm::vec3 iPosOld, iPosNew, iNorm;

	if (glm::intersectRaySphere(rayO, rayDOld, sphereO, sphereRad, iPosOld, iNorm) && 
			glm::intersectRaySphere(rayO, rayDNew, sphereO, sphereRad, iPosNew, iNorm)) {

		float longOld = atan(iPosOld.x / iPosOld.z);
		float latOld = (float)( M_PI / 2.f - acos(iPosOld.y / sphereRad) );

		float longNew = atan(iPosNew.x / iPosNew.z);
		float latNew = (float)( M_PI / 2.f - acos(iPosNew.y / sphereRad) );

		if (skew) {
			camera->updateLatitudeRotation(latNew - latOld);
		}
		else {
			latRot += latNew - latOld;
			longRot += longNew - longOld;
		}

		glm::mat4 worldModel(1.f);
		float s = scale * (1.f / RADIUS_EARTH_KM) * RADIUS_EARTH_VIEW;
		worldModel = glm::scale(worldModel, glm::vec3(s, s, s));
		worldModel = glm::rotate(worldModel, latRot, glm::vec3(-1.f, 0.f, 0.f));
		worldModel = glm::rotate(worldModel, longRot, glm::vec3(0.f, 1.f, 0.f));
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
	renderEngine->setScale(scale);
	camera->setScale(scale);
}
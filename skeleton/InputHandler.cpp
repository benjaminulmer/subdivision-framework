#include "InputHandler.h"

Camera* InputHandler::camera;
RenderEngine* InputHandler::renderEngine;
Program* InputHandler::program;
int InputHandler::mouseOldX;
int InputHandler::mouseOldY;
bool InputHandler::moved;

// Must be called before processing any SDL2 events
void InputHandler::setUp(Camera* camera, RenderEngine* renderEngine, Program* program) {
	InputHandler::camera = camera;
	InputHandler::renderEngine = renderEngine;
	InputHandler::program = program;
}

void InputHandler::pollEvent(SDL_Event& e) {
	if (e.type == SDL_KEYDOWN) {
		InputHandler::key(e.key);
	}
	else if (e.type == SDL_MOUSEBUTTONDOWN) {
		moved = false;
	}
	else if (e.type == SDL_MOUSEBUTTONUP) {
		InputHandler::mouse(e.button);
	}
	else if (e.type == SDL_MOUSEMOTION) {
		InputHandler::motion(e.motion);
	}
	else if (e.type == SDL_MOUSEWHEEL) {
		InputHandler::scroll(e.wheel);
	}
	else if (e.type == SDL_WINDOWEVENT) {
		InputHandler::reshape(e.window);
	}
	else if (e.type == SDL_QUIT) {
		SDL_Quit();
		exit(0);
	}
}

// Callback for key presses
void InputHandler::key(SDL_KeyboardEvent& e) {
	// Light controls
	switch (e.keysym.sym) {
	case(SDLK_UP) :
		program->updateSubdivisionLevel(1);
		break;
	case(SDLK_DOWN) :
		program->updateSubdivisionLevel(-1);
		break;
	case(SDLK_1) :
		program->setScheme(Scheme::SDOG);
		break;
	case(SDLK_2) :
		program->setScheme(Scheme::SDOG_OPT);
		break;
	case(SDLK_3) :
		program->setScheme(Scheme::NAIVE);
		break;
	case(SDLK_4) :
		program->setScheme(Scheme::VOLUME_SDOG);
		break;
	case(SDLK_5) :
		program->setScheme(Scheme::VOLUME);
		break;
	case(SDLK_c) :
		camera->reset();
		break;
	case(SDLK_f):
		renderEngine->toggleFade();
		break;
	case(SDLK_ESCAPE) :
		SDL_Quit();
		exit(0);
		break;
	}
}

// Callback for mouse button presses
void InputHandler::mouse(SDL_MouseButtonEvent& e) {
	mouseOldX = e.x;
	mouseOldY = e.y;
}

// Callback for mouse motion
void InputHandler::motion(SDL_MouseMotionEvent& e) {
	int dx, dy;
	dx = e.x - mouseOldX;
	dy = e.y - mouseOldY;

	// left mouse button moves camera
	if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
		camera->updateLongitudeRotation(dx * 0.5f);
		camera->updateLatitudeRotation(dy * 0.5f);
	}
	else if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_MIDDLE)) {
		camera->translate(glm::vec3(-dx, dy, 0.f));
	}

	// Update current position of the mouse
	int width, height;
	SDL_Window* window = SDL_GetWindowFromID(e.windowID);
	SDL_GetWindowSize(window, &width, &height);

	int iX = e.x;
	int iY = height - e.y;
	//program->setMousePos(iX, iY);

	mouseOldX = e.x;
	mouseOldY = e.y;
}

// Callback for mouse scroll
void InputHandler::scroll(SDL_MouseWheelEvent& e) {
	int dy;
	dy = e.x - e.y;

	const Uint8 *state = SDL_GetKeyboardState(0);
	if (state[SDL_SCANCODE_U]) {
		
	}
	else if (state[SDL_SCANCODE_J]) {

	}
	else if (state[SDL_SCANCODE_I]) {
		program->updateBounds(BoundParam::MAX_LAT, dy);
	}
	else if (state[SDL_SCANCODE_K]) {
		program->updateBounds(BoundParam::MIN_LAT, dy);
	}
	else if (state[SDL_SCANCODE_O]) {
		program->updateBounds(BoundParam::MAX_LONG, dy);
	}
	else if (state[SDL_SCANCODE_L]) {
		program->updateBounds(BoundParam::MIN_LONG, dy);
	}
	else {
		camera->updateZoom(dy);
	}
}

// Callback for window reshape/resize
void InputHandler::reshape(SDL_WindowEvent& e) {
	if (e.event == SDL_WINDOWEVENT_RESIZED) {
		renderEngine->setWindowSize(e.data1, e.data2);
	}
}

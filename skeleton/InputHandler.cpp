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
	case(SDLK_ESCAPE) :
		SDL_Quit();
		exit(0);
	}
}

// Callback for mouse button presses
void InputHandler::mouse(SDL_MouseButtonEvent& e) {
	mouseOldX = e.x;
	mouseOldY = e.y;

	if (e.button == SDL_BUTTON_LEFT && !moved) {
		//program->_3Dpick(true);
	}
}

// Callback for mouse motion
void InputHandler::motion(SDL_MouseMotionEvent& e) {
	int dx, dy;
	dx = e.x - mouseOldX;
	dy = e.y - mouseOldY;

	// left mouse button moves camera
	if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
		moved = true;
		camera->updateLongitudeRotation(dx * 0.5f);
		camera->updateLatitudeRotation(dy * 0.5f);
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
	if (state[SDL_SCANCODE_LSHIFT]) {
		//program->moveCurrentPart(dy * -0.25f);
	}
	else {
		camera->updatePosition(glm::vec3(0.f, 0.f, dy));
	}
}

// Callback for window reshape/resize
void InputHandler::reshape(SDL_WindowEvent& e) {
	if (e.event == SDL_WINDOWEVENT_RESIZED) {
		//program->setWindowSize(e.data1, e.data2);
	}
}

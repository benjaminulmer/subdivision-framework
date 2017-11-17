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
	if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
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
	
	auto key = e.keysym.sym;

	if (e.state == SDL_PRESSED) {
		if (key == SDLK_UP) {
			program->updateGrid(1);
		}
		else if (key == SDLK_DOWN) {
			program->updateGrid(-1);
		}
		else if (key == SDLK_1) {
			program->setScheme(Scheme::SDOG);
		}
		else if (key == SDLK_2) {
			program->setScheme(Scheme::SDOG_OPT);
		}
		// Do not care about these options right now
		//else if (key == SDLK_3) {
		//	program->setScheme(Scheme::NAIVE);
		//}
		//else if (key == SDLK_4) {
		//	program->setScheme(Scheme::VOLUME_SDOG);
		//}
		//else if (key == SDLK_5) {
		//	program->setScheme(Scheme::VOLUME);
		//}
		else if (key == SDLK_4) {
			program->setSubdivisionMode(SubdivisionMode::OCTANT);
		}
		else if (key == SDLK_5) {
			program->setSubdivisionMode(SubdivisionMode::FULL);
		}
		else if (key == SDLK_6) {
			program->setSubdivisionMode(SubdivisionMode::REP_SLICE);
		}
		else if (key == SDLK_7) {
			program->setDisplayMode(DisplayMode::DATA);
		}
		else if (key == SDLK_8) {
			program->setDisplayMode(DisplayMode::LINES);
		}
		else if (key == SDLK_9) {
			program->setDisplayMode(DisplayMode::VOLUMES);
		}
		else if (key == SDLK_c) {
			program->toggleCull();
		}
		else if (key == SDLK_f) {
			renderEngine->toggleFade();
		}
		else if (key == SDLK_r) {
			program->toggleRef();
		}
		else if (key == SDLK_e) {
			program->toggleRefSize();
		}
		else if (key == SDLK_w) {
			program->toggleRefShape();
		}
		else if (key == SDLK_q) {
			program->toggleRotation();
		}
		else if (key == SDLK_u || key == SDLK_i || key == SDLK_o ||
		         key == SDLK_j || key == SDLK_k || key == SDLK_l) {
			program->setBoundsDrawing(true);
		}
		else if (key == SDLK_ESCAPE) {
			SDL_Quit();
			exit(0);
		}
	}
	else { // (e.state == SDL_RELEASED)
		if (key == SDLK_u || key == SDLK_i || key == SDLK_o ||
		    key == SDLK_j || key == SDLK_k || key == SDLK_l) {
			program->setBoundsDrawing(false);
		}
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
		program->updateBounds(BoundParam::MAX_RADIUS, dy);
	}
	else if (state[SDL_SCANCODE_J]) {
		program->updateBounds(BoundParam::MIN_RADIUS, dy);
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

#include "RenderEngine.h"

RenderEngine::RenderEngine(SDL_Window* window, Camera* camera) :
	window(window), fade(false), camera(camera) {

	SDL_GetWindowSize(window, &width, &height);

	mainProgram = ShaderTools::compileShaders("./shaders/main.vert", "./shaders/main.frag");

	projection = glm::perspective(45.f, (float)width/height, 0.01f, 1000.f);

	// Default openGL state
	// If you change state you must change back to default after
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_MULTISAMPLE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPointSize(30.f);
	glClearColor(0.3f, 0.3f, 0.4f, 1.f);
}

// Called to render the active object. RenderEngine stores all information about how to render
void RenderEngine::render(const std::vector<Renderable*>& objects, glm::mat4 view, float max, float min) {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glUseProgram(mainProgram);

	for (const Renderable* r : objects) {	
		glBindVertexArray(r->vao);

		glm::mat4 modelView = view * r->model;
		glUniformMatrix4fv(glGetUniformLocation(mainProgram, "modelView"), 1, GL_FALSE, glm::value_ptr(modelView));
		glUniformMatrix4fv(glGetUniformLocation(mainProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniform3fv(glGetUniformLocation(mainProgram, "objColour"), 1, glm::value_ptr(r->colour));

		glUniform1i(glGetUniformLocation(mainProgram, "fade"), fade && r->fade);
		glUniform1f(glGetUniformLocation(mainProgram, "maxDist"), max);
		glUniform1f(glGetUniformLocation(mainProgram, "minDist"), min);

		glDrawArrays(r->drawMode, 0, r->verts.size());
		glBindVertexArray(0);
	}
}

// Assigns buffers for a renderable
void RenderEngine::assignBuffers(Renderable& renderable) {
	glGenVertexArrays(1, &renderable.vao);
	glBindVertexArray(renderable.vao);

	// Vertex buffer
	glGenBuffers(1, &renderable.vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, renderable.vertexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	// Normal buffer
	glGenBuffers(1, &renderable.normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, renderable.normalBuffer);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);
}

// Sets buffer data for a renderable
void RenderEngine::setBufferData(Renderable& renderable) {
	// Vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, renderable.vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*renderable.verts.size(), renderable.verts.data(), GL_STATIC_DRAW);

	// Normal buffer
	glBindBuffer(GL_ARRAY_BUFFER, renderable.normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*renderable.normals.size(), renderable.normals.data(), GL_STATIC_DRAW);
}

// Deletes buffers for a renderable
void RenderEngine::deleteBufferData(Renderable & renderable) {
	glDeleteBuffers(1, &renderable.vertexBuffer);
	glDeleteBuffers(1, &renderable.normalBuffer);
	glDeleteVertexArrays(1, &renderable.vao);
}

// Sets projection and viewport for new width and height
void RenderEngine::setWindowSize(int newWidth, int newHeight) {
	width = newWidth;
	height = newHeight;
	projection = glm::perspective(45.0f, (float)width/height, 0.01f, 100.0f);
	glViewport(0, 0, width, height);
}

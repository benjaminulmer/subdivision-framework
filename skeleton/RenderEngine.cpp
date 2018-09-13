#include "RenderEngine.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "lodepng.h"

#include <iostream>

#include "ShaderTools.h"
#include "Texture.h"

RenderEngine::RenderEngine(SDL_Window* window) : window(window), fade(true), scale(1.f) {

	SDL_GetWindowSize(window, &width, &height);

	mainProgram = ShaderTools::compileShaders("./shaders/main.vert", "./shaders/main.frag");
	skyboxProgram = ShaderTools::compileShaders("./shaders/skybox.vert", "./shaders/skybox.frag");

	projection = glm::perspective(fovYRad, (float)width/height, fovNear, fovFar);

	// Default openGL state
	// If you change state you must change back to default after
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_MULTISAMPLE);
	//glEnable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPointSize(30.f);
	glLineWidth(1.5f);
	glClearColor(0.f, .0078f, 0.1686f, 1.f);

	createSkybox();
}

void RenderEngine::createSkybox() {

	skybox = Renderable();
	// Load skybox texture from .png image file
	skybox.textureID = loadTexture("textures/skybox.png");

	skybox.fade = false;

	skybox.verts = {
		glm::vec3(-30000.f, -30000.f, 0.f),
		glm::vec3(30000.f, -30000.f, 0.f),
		glm::vec3(30000.f, 30000.f, 0.f),
		glm::vec3(30000.f, 30000.f, 0.f),
		glm::vec3(-30000.f, 30000.f, 0.f),
		glm::vec3(-30000.f, -30000.f, 0.f)
	};

	skybox.uvs = {
		glm::vec2(0.f, 0.f),
		glm::vec2(1.f, 0.f),
		glm::vec2(1.f, 1.f),
		glm::vec2(1.f, 1.f),
		glm::vec2(0.f, 1.f),
		glm::vec2(0.f, 0.f)
	};

	skybox.colours = {
		glm::vec3(1.f, 1.f, 1.f),
		glm::vec3(1.f, 1.f, 1.f),
		glm::vec3(1.f, 1.f, 1.f),
		glm::vec3(1.f, 1.f, 1.f)
	};

	skybox.renderColour = glm::vec3(1.f, 1.f, 1.f);
	skybox.drawMode = GL_TRIANGLES;

	skybox.model = glm::scale(skybox.model, glm::vec3(1.f, 1.f, 1.f));

	assignBuffers(skybox, true);
	setBufferData(skybox, true);
}

// Called to render the active object. RenderEngine stores all information about how to render
void RenderEngine::render(const std::vector<const Renderable*>& objects, const glm::mat4& view, float max, float min) {
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cout << "Has error" << std::endl;
		std::cout << error << std::endl;
	}

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glUseProgram(mainProgram);
	
	for (const Renderable* r : objects) {	

		//std::cout << r->verts[0].x << " " << r->verts[0].y << " " << r->verts[0].z << std::endl;
		
		glBindVertexArray(r->vao);

		glm::mat4 modelView = view * r->model;
		glUniformMatrix4fv(glGetUniformLocation(mainProgram, "modelView"), 1, GL_FALSE, glm::value_ptr(modelView));
		glUniformMatrix4fv(glGetUniformLocation(mainProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		glUniform1i(glGetUniformLocation(mainProgram, "fade"), fade && r->fade);
		glUniform1f(glGetUniformLocation(mainProgram, "alpha"), r->alpha);
		glUniform1f(glGetUniformLocation(mainProgram, "maxDist"), max);
		glUniform1f(glGetUniformLocation(mainProgram, "minDist"), min);

		if (r->textureID != 0) {
			glUniform1i(glGetUniformLocation(mainProgram, "hasTexture"), true);
			Texture::bind2DTexture(mainProgram, r->textureID, "imTexture");
		}
		else {
			glUniform1i(glGetUniformLocation(mainProgram, "hasTexture"), false);
		}

		glDrawArrays(r->drawMode, 0, (GLsizei)r->verts.size());
		glBindVertexArray(0);
	}
	glUseProgram(0);

	renderSkybox(view);
}

void RenderEngine::renderSkybox(const glm::mat4& view) {
	// Render the skybox
	glUseProgram(skyboxProgram);

	glBindVertexArray(skybox.vao);

	//glm::mat4 modelView = view * skybox.model;
	glm::mat4 modelView = view;
	glUniformMatrix4fv(glGetUniformLocation(skyboxProgram, "modelView"), 1, GL_FALSE, glm::value_ptr(modelView));
	glUniformMatrix4fv(glGetUniformLocation(skyboxProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniform1f(glGetUniformLocation(skyboxProgram, "scale"), scale);

	Texture::bind2DTexture(skyboxProgram, skybox.textureID, "imTexture");

	glDrawArrays(skybox.drawMode, 0, (GLsizei)skybox.verts.size());
	glBindVertexArray(0);

	glUseProgram(0);
}


// Assigns buffers for a renderable
void RenderEngine::assignBuffers(Renderable& renderable, bool texture) {

	glGenVertexArrays(1, &renderable.vao);
	glBindVertexArray(renderable.vao);

	// Vertex buffer
	glGenBuffers(1, &renderable.vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, renderable.vertexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	// Colour buffer
	glGenBuffers(1, &renderable.colourBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, renderable.colourBuffer);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);

	if (texture) {
		// UV buffer
		glGenBuffers(1, &renderable.uvBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, renderable.uvBuffer);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(2);
	}
}

// Sets buffer data for a renderable
void RenderEngine::setBufferData(Renderable& renderable, bool texture) {

	// Vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, renderable.vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*renderable.verts.size(), renderable.verts.data(), GL_STATIC_DRAW);

	// Colour buffer
	glBindBuffer(GL_ARRAY_BUFFER, renderable.colourBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*renderable.colours.size(), renderable.colours.data(), GL_STATIC_DRAW);

	if (texture) {
		// UV buffer
		glBindBuffer(GL_ARRAY_BUFFER, renderable.uvBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2)*renderable.uvs.size(), renderable.uvs.data(), GL_STATIC_DRAW);
	}
}

// Deletes buffers for a renderable
void RenderEngine::deleteBufferData(Renderable & renderable, bool texture) {

	glDeleteBuffers(1, &renderable.vertexBuffer);
	glDeleteBuffers(1, &renderable.colourBuffer);
	if (texture) {
		glDeleteBuffers(1, &renderable.uvBuffer);
	}
	glDeleteVertexArrays(1, &renderable.vao);
}

// Creates a 2D texture
GLuint RenderEngine::loadTexture(const std::string& filename) {

	// Read in file (png only)
	std::vector<unsigned char> _image;
	unsigned int _imageWidth, _imageHeight;

	unsigned int error = lodepng::decode(_image, _imageWidth, _imageHeight, filename.c_str());
	if (error) {
		std::cout << "reading error" << error << ":" << lodepng_error_text(error) << std::endl;
	}

	GLuint id = Texture::create2DTexture(_image, _imageWidth, _imageHeight);
	return id;
}

// Sets projection and viewport for new width and height
void RenderEngine::setWindowSize(int newWidth, int newHeight) {
	width = newWidth;
	height = newHeight;
	projection = glm::perspective(fovYRad, (float)width / height, fovNear, fovFar);
	glViewport(0, 0, width, height);
}

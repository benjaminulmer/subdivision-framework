#include "RenderEngine.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "lodepng.h"

#include <iostream>

#include "Constants.h"
#include "ShaderTools.h"
#include "Texture.h"

RenderEngine::RenderEngine(SDL_Window* window, double cameraDist) : window(window), fade(true) {

	SDL_GetWindowSize(window, &width, &height);

	mainProgram = ShaderTools::compileShaders("./shaders/main.vert", "./shaders/main.frag");

	updatePlanes(cameraDist);
	projection = glm::perspective(fovYRad, (double)width/height, near, far);

	// Default openGL state
	// If you change state you must change back to default after
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_MULTISAMPLE);
	//glEnable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPointSize(30.f);
	glLineWidth(1.f);
	glClearColor(0.4f, 0.4f, 0.4f, 1.f);
}

// Called to render the active object. RenderEngine stores all information about how to render
void RenderEngine::render(const std::vector<const Renderable*>& objects, const glm::dmat4& view, float max, float min) {

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glUseProgram(mainProgram);

	for (const Renderable* r : objects) {	
		glBindVertexArray(r->vao);

		glm::dmat4 modelViewD = view;

		glm::dmat4 inv = glm::inverse(modelViewD);
		glm::dvec3 eyePos = inv[3];

		modelViewD[3] = glm::dvec4(0.0, 0.0, 0.0, 1.0);

		glm::mat4 modelView = modelViewD;

		glm::vec3 eyeHigh = eyePos;
		glm::vec3 eyeLow = eyePos - (glm::dvec3)eyeHigh;

		glUniformMatrix4fv(glGetUniformLocation(mainProgram, "modelView"), 1, GL_FALSE, glm::value_ptr(modelView));
		glUniformMatrix4fv(glGetUniformLocation(mainProgram, "projection"), 1, GL_FALSE, glm::value_ptr((glm::mat4)projection));

		glUniform3fv(glGetUniformLocation(mainProgram, "eyeHigh"), 1, glm::value_ptr(eyeHigh));
		glUniform3fv(glGetUniformLocation(mainProgram, "eyeLow"), 1, glm::value_ptr(eyeLow));

		glUniform1i(glGetUniformLocation(mainProgram, "fade"), fade && r->fade);
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
		Texture::unbind2DTexture();
		glBindVertexArray(0);
	}
}

// Assigns buffers for a renderable
void RenderEngine::assignBuffers(Renderable& renderable, bool texture) {

	glGenVertexArrays(1, &renderable.vao);
	glBindVertexArray(renderable.vao);

	// Vertex buffer
	glGenBuffers(1, &renderable.vertexHighBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, renderable.vertexHighBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	// Vertex buffer
	glGenBuffers(1, &renderable.vertexLowBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, renderable.vertexLowBuffer);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);

	// Colour buffer
	glGenBuffers(1, &renderable.colourBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, renderable.colourBuffer);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(2);

	if (texture) {
		// UV buffer
		glGenBuffers(1, &renderable.uvBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, renderable.uvBuffer);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(3);
	}
}

// Sets buffer data for a renderable
void RenderEngine::setBufferData(Renderable& renderable, bool texture) {

	// Vertex high buffer
	glBindBuffer(GL_ARRAY_BUFFER, renderable.vertexHighBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*renderable.vertsHigh.size(), renderable.vertsHigh.data(), GL_STATIC_DRAW);

	// Vertex low buffer
	glBindBuffer(GL_ARRAY_BUFFER, renderable.vertexLowBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*renderable.vertsLow.size(), renderable.vertsLow.data(), GL_STATIC_DRAW);

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

	glDeleteBuffers(1, &renderable.vertexHighBuffer);
	glDeleteBuffers(1, &renderable.vertexLowBuffer);
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
	projection = glm::perspective(fovYRad, (double)width / height, near, far);
	glViewport(0, 0, width, height);
}


// Updates near and far planes based on camera distance
//
// cameraDist - distance camera is from surface of the Earth
void RenderEngine::updatePlanes(double cameraDist) {

	double cameraPlusRad = RADIUS_EARTH_M + cameraDist;
	double t = sqrt(cameraPlusRad * cameraPlusRad + RADIUS_EARTH_M * RADIUS_EARTH_M);
	double theta = asin(RADIUS_EARTH_M / cameraPlusRad);

	far = cos(theta) * t;
	// TODO maybe a better way to calculate near
	near = cameraDist - RADIUS_EARTH_M - 33000.0;
	//if (near < 0.0) {
	//	near = cameraDist - 33000.0 * scaleFactor;
	//}
	if (near < 0.0) {
		if (far < 6500000.0) {
			near = far / 1000.0;
		}
		else {
			near = far / 100.0;
		}

	}

	projection = glm::perspective(fovYRad, (double)width / height, near, far);
}
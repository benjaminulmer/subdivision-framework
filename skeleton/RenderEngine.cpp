#include "RenderEngine.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

RenderEngine::RenderEngine(SDL_Window* window, Camera* camera) :
	window(window), camera(camera) {

	SDL_GetWindowSize(window, &width, &height);

	mainProgram = ShaderTools::compileShaders("./shaders/mesh.vert", "./shaders/mesh.frag");

	lightPos = glm::vec3(-10.f, 100.f, -10.f);
	projection = glm::perspective(45.f, (float)width/height, 0.01f, 1000.f);

	// Default openGL state
	// If you change state you must change back to default after
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_MULTISAMPLE);
	glPointSize(5.f);
	glClearColor(0.3f, 0.3f, 0.4f, 0.f);


	glUseProgram(mainProgram);
	//Load Blue Marble texture
	glGenTextures(1, &Texture);
	glBindTexture(GL_TEXTURE_2D, Texture);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load and generate the texture
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* imgdata = stbi_load("2k_earth_daymap.jpg", &width, &height, &nrChannels, 0);
	if (imgdata) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imgdata);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(imgdata);

}

// Called to render the active object. RenderEngine stores all information about how to render
void RenderEngine::render(const std::vector<Renderable>& objects, glm::mat4 view) {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glUseProgram(mainProgram);

	GLuint TextureID = glGetUniformLocation(mainProgram, "image");

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture);
	// Set our "myTextureSampler" sampler to use Texture Unit 0
	glUniform1i(TextureID, 0);

	for (const Renderable& r : objects) {
		glBindVertexArray(r.vao);

		glm::mat4 modelView = view;
		glUniformMatrix4fv(glGetUniformLocation(mainProgram, "modelView"), 1, GL_FALSE, glm::value_ptr(modelView));
		glUniformMatrix4fv(glGetUniformLocation(mainProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniform3fv(glGetUniformLocation(mainProgram, "objColour"), 1, glm::value_ptr(r.colour));

		glDrawArrays(r.drawMode, 0, r.verts.size());
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

	// uv buffer
	glGenBuffers(1, &renderable.uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, renderable.uvBuffer);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(2);
}

// Sets buffer data for a renderable
void RenderEngine::setBufferData(Renderable& renderable) {
	// Vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, renderable.vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*renderable.verts.size(), renderable.verts.data(), GL_STATIC_DRAW);

	// Normal buffer
	glBindBuffer(GL_ARRAY_BUFFER, renderable.normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*renderable.normals.size(), renderable.normals.data(), GL_STATIC_DRAW);
	
	// uv buffer
	glBindBuffer(GL_ARRAY_BUFFER, renderable.uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2)*renderable.uvs.size(), renderable.uvs.data(), GL_STATIC_DRAW);
}

// Deletes buffers for a renderable
void RenderEngine::deleteBufferData(Renderable & renderable) {
	glDeleteBuffers(1, &renderable.vertexBuffer);
	glDeleteBuffers(1, &renderable.normalBuffer);
	glDeleteBuffers(1, &renderable.uvBuffer);
	glDeleteVertexArrays(1, &renderable.vao);
}

// Sets projection and viewport for new width and height
void RenderEngine::setWindowSize(int newWidth, int newHeight) {
	width = newWidth;
	height = newHeight;
	projection = glm::perspective(45.0f, (float)width/height, 0.01f, 100.0f);
	glViewport(0, 0, width, height);
}

// Updates lightPos by specified value
void RenderEngine::updateLightPos(glm::vec3 add) {
	lightPos += add;
}

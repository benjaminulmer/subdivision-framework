#pragma once

//#include <gl/glew.h>
#include <glm/glm.hpp>
#include <map>

#include <vector>

// This is the class that links with NVIDIA CUDA to do ray casting on the gpu. 
#include <cudaSamples/helper_gl.h>
//#include <GL/glew.h>
//#include <glm/glm.hpp>
//#include <SDL2/SDL.h>

#include <GL/freeglut.h>

// CUDA Runtime, Interop, and includes
#include <cuda/cuda_runtime.h>
#include <cuda/cuda_gl_interop.h>
#include <cuda/cuda_profiler_api.h>
#include <cuda/vector_types.h>
#include <cuda/vector_functions.h>
#include <cuda/driver_functions.h>

// Helper functions
#include <cudaSamples/helper_cuda.h>
#include <cudaSamples/helper_functions.h>
#include <cudaSamples/helper_timer.h>

#include "Camera.h"
#include "Constants.h"
#include "SdogCell.h"
#include "SdogDB.h"

typedef unsigned char VolumeType;

class RayTracer
{
public:
	RayTracer(Camera* c, int ac, char** v);// {}
	//RayTracer(unsigned int w, unsigned int h);

	~RayTracer() {}

	void display();
	void trace(const std::vector<Renderable*>& objects, Camera* c, SdogDB* database, glm::mat4 projView, glm::mat4 worldModel, float scale);
	glm::vec4 traceHelper(float x, float y, glm::mat4 projView, glm::mat4 worldModel, glm::vec3 camPos, SdogDB* database);
	//void trace(SdogDB* database);
	void resize(unsigned int w, unsigned int h);

	//int iDivUp(int a, int b);

	std::vector<std::vector<glm::vec4>> renderBuffer;

	// Stores colours (with alpha value) associated with a given sigmet
	std::map<std::string, glm::vec4> cache;

private:

	int argc;
	char** argv;

	//temporary:
	//void display(SdogDB* database);
	void runSingleTest(const char *ref_file, const char *exec_path);

	void initPixelBuffer();
	// end temporary

	//int wWidth;
	//int wHeight;

	//SdogDB* database;

	std::vector<std::vector<glm::vec3>> pixels;

	//void traceHelper(const Ray &ray, int depth, SdogDB* database, glm::mat4 worldModel);

	Camera* camera;
	GLuint pbo;     // OpenGL pixel buffer object
	cudaExtent volumeSize;
	StopWatchInterface *timer;
	dim3 gridSize;
	struct cudaGraphicsResource *cuda_pbo_resource; // CUDA Graphics Resource (to transfer PBO)


	/*SDL_Window* window;

	void pollEvent(SDL_Event& e);*/

};


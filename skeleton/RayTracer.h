#pragma once

//#include <gl/glew.h>
#include <glm/glm.hpp>
#include <map>

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

#include "SdogDB.h"

class RayTracer
{
public:
	RayTracer();// {}
	//RayTracer(unsigned int w, unsigned int h);

	~RayTracer() {}

	static void display();
	//void trace();
	//void trace(SdogDB* database);
	void resize(unsigned int w, unsigned int h);

	//int iDivUp(int a, int b);

	std::vector<std::vector<glm::vec4>> renderBuffer;

	// Stores colours (with alpha value) associated with a given sigmet
	std::map<std::string, glm::vec4> cache;

private:

	//temporary:
	//void display(SdogDB* database);
	void runSingleTest(const char *ref_file, const char *exec_path);

	// end temporary

	int wWidth;
	int wHeight;

	SdogDB* database;

	/*SDL_Window* window;

	void pollEvent(SDL_Event& e);*/

};


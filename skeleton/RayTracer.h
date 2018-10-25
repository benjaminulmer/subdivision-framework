#pragma once

#include <glm/glm.hpp>
#include <map>
#include <vector>

// This is the class that links with NVIDIA CUDA to do ray casting on the gpu. 
#include <cudaSamples/helper_gl.h>

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

typedef unsigned int uint;
typedef unsigned char uchar;

#define MAX_EPSILON_ERROR 5.00f
#define THRESHOLD         0.30f

#define _USE_MATH_DEFINES
#include <math.h> 

#include <glm/gtx/intersect.hpp>

typedef struct
{
	float4 m[4];
} float4x4;

extern "C" void initCuda(void *h_volume, cudaExtent volumeSize);
extern "C" void cudaCodeForPos(float trace_x, float trace_y, float trace_z, char* returnCode);

extern "C"
void render_kernel(dim3 gridSize, dim3 blockSize, uint *d_output, uint imageW, uint imageH, float4x4 projView,
	float4x4 worldModel, float3 camPos);

extern "C" void copyDataCache(const char *cacheSource, size_t sizeofCache);

typedef unsigned char VolumeType;

struct Cell {
	int sigmetType;
	char* code;
};

class RayTracer
{
public:
	RayTracer(Camera* c, int ac, char** v);
	~RayTracer() {}

	void trace(const std::vector<Renderable*>& objects, Camera* c, glm::mat4 projView, glm::mat4 worldModel, float scale);
	void resize(unsigned int w, unsigned int h);

	//int iDivUp(int a, int b);

	std::vector<std::vector<glm::vec4>> renderBuffer;

	void addToCache(AirSigmet sigmet, std::string code);
	void copyCacheToKernel();

private:

	//temporary:
	void initPixelBuffer();
	// end temporary

	int wWidth;
	int wHeight;

	glm::vec4 traceHelper(float x, float y, glm::mat4 projView, glm::mat4 worldModel, glm::vec3 camPos);

	void getAirSigmetForCell(std::string code, std::vector<int>& out, int level);

	Camera* camera;
	GLuint pbo;     // OpenGL pixel buffer object
	dim3 gridSize;
	dim3 blockSize;

	struct cudaGraphicsResource *cuda_pbo_resource; // CUDA Graphics Resource (to transfer PBO)

	std::vector<Cell> dataCache;
};


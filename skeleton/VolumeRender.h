#pragma once

// OpenGL Graphics includes
#include <helper_gl.h>
#include <GL/freeglut.h>

// CUDA Runtime, Interop, and includes
#include <cuda_runtime.h>
#include <cuda_gl_interop.h>
#include <cuda_profiler_api.h>
#include <vector_types.h>
#include <vector_functions.h>
#include <driver_functions.h>

// CUDA utilities
#include <helper_cuda.h>

// Helper functions
#include <helper_cuda.h>
#include <helper_functions.h>
#include <helper_timer.h>


class VolumeRender
{
public:
	VolumeRender() {}

	~VolumeRender() {}

	void setup(int argc);

	void loop();
};


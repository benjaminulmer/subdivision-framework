#pragma once
// This is the class that links with NVIDIA CUDA to do ray casting on the gpu. 

#include <helper_gl.h>

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

class RayTracer
{
public:
	RayTracer();
	~RayTracer() {}
};


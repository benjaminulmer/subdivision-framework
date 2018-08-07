#pragma once
// This is the class that links with NVIDIA CUDA to do ray casting on the gpu. 

#include <cudaSamples/helper_gl.h>

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

class RayTracer
{
public:
	RayTracer();
	~RayTracer() {}

	void trace();
};


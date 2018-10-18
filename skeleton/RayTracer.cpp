#include "RayTracer.h"

typedef unsigned int uint;
typedef unsigned char uchar;

#define MAX_EPSILON_ERROR 5.00f
#define THRESHOLD         0.30f

#define _USE_MATH_DEFINES
#include <math.h> 

#include <glm/gtx/intersect.hpp>

dim3 blockSize(16, 16);

// Define the files that are to be save and the reference images for validation
//const char *sOriginal[] =
//{
//	"volume.ppm",
//	NULL
//};
//
//const char *sReference[] =
//{
//	"ref_volume.ppm",
//	NULL
//};

//const char *sSDKsample = "CUDA 3D Volume Render";
//
//const char *volumeFilename = "Bucky.raw";
//cudaExtent volumeSize = make_cudaExtent(32, 32, 32);
//typedef unsigned char VolumeType;
//
////char *volumeFilename = "mrt16_angio.raw";
////cudaExtent volumeSize = make_cudaExtent(416, 512, 112);
////typedef unsigned short VolumeType;
//
//uint width = 512, height = 512;
//dim3 blockSize(16, 16);
//dim3 gridSize;
//
//float3 viewRotation;
//float3 viewTranslation = make_float3(0.0, 0.0, -4.0f);
//float invViewMatrix[12];
//
//float density = 0.05f;
//float brightness = 1.0f;
//float transferOffset = 0.0f;
//float transferScale = 1.0f;
//bool linearFiltering = true;
//
//GLuint pbo = 0;     // OpenGL pixel buffer object
//GLuint tex = 0;     // OpenGL texture object
//struct cudaGraphicsResource *cuda_pbo_resource; // CUDA Graphics Resource (to transfer PBO)
//
//StopWatchInterface *timer = 0;
//
//// Auto-Verification Code
//const int frameCheckNumber = 2;
//int fpsCount = 0;        // FPS count for averaging
//int fpsLimit = 1;        // FPS limit for sampling
//int g_Index = 0;
//unsigned int frameCount = 0;
//
//int *pArgc;
//char **pArgv;

typedef struct
{
	float4 m[4];
} float4x4;

#ifndef MAX
#define MAX(a,b) ((a > b) ? a : b)
#endif

//extern "C" void setTextureFilterMode(bool bLinearFilter);
extern "C" void initCuda(void *h_volume, cudaExtent volumeSize);
//extern "C" void freeCudaBuffers();
//extern "C" void copyInvViewMatrix(float *invViewMatrix, size_t sizeofMatrix);

extern "C" void cudaCodeForPos(double latRad, double longRad, double radius, double gridRadius, unsigned int level, char* returnCode);

extern "C" void getInteger(uint* returnInt);

extern "C"
void render_kernel(dim3 gridSize, dim3 blockSize, uint *d_output, uint imageW, uint imageH, float4x4 projView,
	float4x4 worldModel, float3 camPos, SdogDB* database);

//void initPixelBuffer();

int iDivUp(int a, int b)
{
	return (a % b != 0) ? (a / b + 1) : (a / b);
}

glm::vec4 RayTracer::traceHelper(float x, float y, glm::mat4 projView, glm::mat4 worldModel, glm::vec3 camPos, SdogDB* database)
{
	glm::mat4 invProjView = glm::inverse(projView);

	glm::vec4 worldNew(x, y, -1.f, 1.f);

	worldNew = invProjView * worldNew;

	worldNew.x /= worldNew.w;
	worldNew.y /= worldNew.w;
	worldNew.z /= worldNew.w;

	worldNew = glm::inverse(worldModel) * worldNew;

	glm::vec3 rayO = camPos;

	glm::vec3 rayD = glm::normalize(glm::vec3(worldNew) - rayO);

	glm::vec4 colour(0.f, 0.f, 0.f, 0.f);
	
	int continueCount = 0;
	int count = 0;
	int k = 0;
	std::string prevCode = "";

	while(count < 750) 
	{
		glm::vec3 tracePoint = rayO + ((float)k * glm::normalize(rayD));
		
		k += 50;

		SphCoord coord(tracePoint);

		std::string code = SdogCell::codeForPos(coord.latitude, coord.longitude, coord.radius, (RADIUS_EARTH_KM * 4.f / 3.f), 8);

		// Setup to get code from the GPU
		char* cudaCode;

		checkCudaErrors(cudaMalloc((void **)&cudaCode, 20 * sizeof(char)));

		checkCudaErrors(cudaMemset(cudaCode, 0, 20 * sizeof(char)));

		cudaCodeForPos(coord.latitude, coord.longitude, coord.radius, (RADIUS_EARTH_KM * 4.f / 3.f), 8, cudaCode);

		getLastCudaError("kernel failed");

		char *charCode = (char *)malloc(20 * sizeof(char));
		checkCudaErrors(cudaMemcpy(charCode, cudaCode, 20 * sizeof(char), cudaMemcpyDeviceToHost));

		//std::cout << "Code:     " << code << std::endl;
		//std::cout << "charCode: " << charCode << std::endl;

		std::vector<AirSigmet> sigs;

		if (code.compare(prevCode) == 0) {
			continueCount++;
			if (continueCount > 1000)
			{
				//std::cout << "too many continues" << std::endl;

				break;
			}
			//std::cout << "continuing" << std::endl;
			continue;
		}
		else if (stoi(code) == 0) break;

		continueCount = 0;
		//std::cout << code << std::endl; 

		prevCode = code; 
		count++;

		database->getAirSigmetForCell(code, sigs);

		if (sigs.size() == 0) continue;

		//std::cout << "has sigmet" << std::endl;

		colour = glm::vec4(1.f, 0.f, 0.f, 0.5f);
		break;

	}

	return colour;
}

void RayTracer::trace(const std::vector<Renderable*>& objects, Camera* c, SdogDB* database, glm::mat4 projView, glm::mat4 worldModel, float scale)
{
	std::cout << "about to trace" << std::endl;

	int wWidth = 800;
	int wHeight = 800;

	//std::cout << "w: " << wWidth << " h: " << wHeight << std::endl;

	std::vector<std::vector<glm::vec4>> buffer;

	glm::vec4 cPos = glm::inverse(worldModel) * glm::vec4(c->getPosition(), 1.0);

	std::cout << "camera pos = " << cPos.x << " " << cPos.y << " " << cPos.z << std::endl;

	uint* d_output;

	// clear image
	//checkCudaErrors(cudaMemset(d_output, 0, wWidth*wHeight * 4));

	dim3 blockSize(16, 16);
	dim3 gridSize = dim3(iDivUp(wWidth, blockSize.x), iDivUp(wHeight, blockSize.y));

	glm::mat4 invProjView = glm::inverse(projView);
	glm::mat4 invWorldModel = glm::inverse(worldModel);

	float3 cudaCPos = make_float3(cPos.x, cPos.y, cPos.z);

	//render_kernel(gridSize, blockSize, d_output, wWidth, wHeight, invProjView, invWorldModel, cudaCPos, database);

	//getLastCudaError("kernel failed");

	//checkCudaErrors(cudaGraphicsUnmapResources(1, &cuda_pbo_resource, 0));

	traceHelper(0.f, 0.f, projView, worldModel, cPos, database);


	//for (int i = 0; i < wWidth; i++) {
	//	std::vector<glm::vec4> row;
	//	for (int j = 0; j < wHeight; j++) {

	//		float w = (((float)i / (float)wWidth) * 2.f) - 1.f;
	//		float h = (((float)j / (float)wHeight) * 2.f) - 1.f;

	//		row.push_back(traceHelper(w, h, projView, worldModel, cPos, database));
	//	}

	//	std::cout << "Done row " << i << std::endl;
	//	buffer.push_back(row);
	//}

	glDisable(GL_DEPTH_TEST);
	glPointSize(2.f);

	glEnable(GL_BLEND);
	glBegin(GL_POINTS);

	glColor4f(1.f, 0.f, 0.f, 0.3f);
/*
	for (int i = 0; i < wWidth; i++) {
		for (int j = 0; j < wHeight; j++) {
			glm::vec4 val = buffer[i][j];
			glColor4f(val.x, val.y, val.z, val.w);

			float w = ((float)i / (float)wWidth) * 2.f - 1.f;
			float h = ((float)j / (float)wHeight) * 2.f - 1.f;

			if (val.w > 0.f) glVertex2f(w, h);
		}
	}*/
	glEnd();
}

void RayTracer::resize(unsigned int w, unsigned int h)
{
	//wWidth = w;
	//wHeight = h;
}

/*
* Copyright 1993-2015 NVIDIA Corporation.  All rights reserved.
*
* Please refer to the NVIDIA end user license agreement (EULA) associated
* with this source code for terms and conditions that govern your use of
* this software. Any use, reproduction, disclosure, or distribution of
* this software and related documentation outside the terms of the EULA
* is strictly prohibited.
*
*/

/*
Volume rendering sample

This sample loads a 3D volume from disk and displays it using
ray marching and 3D textures.

Note - this is intended to be an example of using 3D textures
in CUDA, not an optimized volume renderer.

Changes
sgg 22/3/2010
- updated to use texture for display instead of glDrawPixels.
- changed to render from front-to-back rather than back-to-front.
*/

//void computeFPS()
//{
//	frameCount++;
//	fpsCount++;
//
//	if (fpsCount == fpsLimit)
//	{
//		char fps[256];
//		float ifps = 1.f / (sdkGetAverageTimerValue(&timer) / 1000.f);
//		//sprintf(fps, "Volume Render: %3.1f fps", ifps);
//
//		//printf(fps);
//		//printf("\n");
//
//		// glutSetWindowTitle(fps);
//		fpsCount = 0;
//
//		fpsLimit = (int)MAX(1.f, ifps);
//		sdkResetTimer(&timer);
//	}
//}

//void reshape(int w, int h)
//{
//	width = w;
//	height = h;
//	initPixelBuffer();
//
//	// calculate new grid size
//	//gridSize = dim3(iDivUp(width, blockSize.x), iDivUp(height, blockSize.y));
//
//	glViewport(0, 0, w, h);
//
//	glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity();
//
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//	glOrtho(0.0, 1.0, 0.0, 1.0, 0.0, 1.0);
//}
//
//void cleanup()
//{
//	sdkDeleteTimer(&timer);
//
//	freeCudaBuffers();
//
//	if (pbo)
//	{
//		cudaGraphicsUnregisterResource(cuda_pbo_resource);
//		glDeleteBuffers(1, &pbo);
//		glDeleteTextures(1, &tex);
//	}
//	// Calling cudaProfilerStop causes all profile data to be
//	// flushed before the application exits
//	checkCudaErrors(cudaProfilerStop());
//}

void RayTracer::initPixelBuffer()
{
	int width = 800;
	int height = 800;

	if (pbo)
	{
		// unregister this buffer object from CUDA C
		checkCudaErrors(cudaGraphicsUnregisterResource(cuda_pbo_resource));

		// delete old buffer
		glDeleteBuffers(1, &pbo);
	}

	// create pixel buffer object for display
	glGenBuffers(1, &pbo);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, pbo);
	glBufferData(GL_PIXEL_UNPACK_BUFFER_ARB, width*height * sizeof(GLubyte) * 4, 0, GL_STREAM_DRAW_ARB);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

	// register this buffer object with CUDA
	checkCudaErrors(cudaGraphicsGLRegisterBuffer(&cuda_pbo_resource, pbo, cudaGraphicsMapFlagsWriteDiscard)); // This line caused errors(?)
}

RayTracer::RayTracer(Camera* c, int ac, char** v) : argc(ac), argv(v) {
	std::cout << "set camera" << std::endl;

	pbo = 0;

	camera = c;
	char *ref_file = NULL;

	cudaExtent volumeSize = make_cudaExtent(32, 32, 32);
	timer = 0;

	findCudaDevice(argc, (const char **)argv);

	std::cout << "argc: " << argc << std::endl;
	std::cout << "argv: " << argv << std::endl;


	size_t size = volumeSize.width*volumeSize.height*volumeSize.depth * sizeof(VolumeType);
	//void *h_volume = loadRawFile(path, size);

	//initCuda(h_volume, volumeSize);
	//free(h_volume);

	sdkCreateTimer(&timer);

	int width = 800;
	int height = 800;

	// calculate new grid size
	gridSize = dim3(iDivUp(width, blockSize.x), iDivUp(height, blockSize.y));

	initPixelBuffer();

}
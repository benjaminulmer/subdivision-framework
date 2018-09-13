#include "RayTracer.h"

typedef unsigned int uint;
typedef unsigned char uchar;

#define MAX_EPSILON_ERROR 5.00f
#define THRESHOLD         0.30f

#define _USE_MATH_DEFINES
#include <math.h> 

// Define the files that are to be save and the reference images for validation
const char *sOriginal[] =
{
	"volume.ppm",
	NULL
};

const char *sReference[] =
{
	"ref_volume.ppm",
	NULL
};

const char *sSDKsample = "CUDA 3D Volume Render";

const char *volumeFilename = "Bucky.raw";
cudaExtent volumeSize = make_cudaExtent(32, 32, 32);
typedef unsigned char VolumeType;

//char *volumeFilename = "mrt16_angio.raw";
//cudaExtent volumeSize = make_cudaExtent(416, 512, 112);
//typedef unsigned short VolumeType;

uint width = 512, height = 512;
dim3 blockSize(16, 16);
dim3 gridSize;

float3 viewRotation;
float3 viewTranslation = make_float3(0.0, 0.0, -4.0f);
float invViewMatrix[12];

float density = 0.05f;
float brightness = 1.0f;
float transferOffset = 0.0f;
float transferScale = 1.0f;
bool linearFiltering = true;

GLuint pbo = 0;     // OpenGL pixel buffer object
GLuint tex = 0;     // OpenGL texture object
struct cudaGraphicsResource *cuda_pbo_resource; // CUDA Graphics Resource (to transfer PBO)

StopWatchInterface *timer = 0;

// Auto-Verification Code
const int frameCheckNumber = 2;
int fpsCount = 0;        // FPS count for averaging
int fpsLimit = 1;        // FPS limit for sampling
int g_Index = 0;
unsigned int frameCount = 0;

int *pArgc;
char **pArgv;

#ifndef MAX
#define MAX(a,b) ((a > b) ? a : b)
#endif

extern "C" void setTextureFilterMode(bool bLinearFilter);
extern "C" void initCuda(void *h_volume, cudaExtent volumeSize);
extern "C" void freeCudaBuffers();
extern "C" void copyInvViewMatrix(float *invViewMatrix, size_t sizeofMatrix);
//extern "C" void render_kernel(dim3 gridSize, dim3 blockSize, unsigned int* d_output, unsigned int imageW, unsigned int imageH, SdogDB* database);

extern "C"
void render_kernel(dim3 gridSize, dim3 blockSize, uint *d_output, uint imageW, uint imageH,
	float density, float brightness, float transferOffset, float transferScale);

void initPixelBuffer();

int iDivUp(int a, int b)
{
	return (a % b != 0) ? (a / b + 1) : (a / b);
}

//void RayTracer::trace(SdogDB* database) 
//void trace() 
//{
//	copyInvViewMatrix(invViewMatrix, sizeof(float4) * 3);
//
//	// map PBO to get CUDA device pointer
//	uint *d_output;
//	// map PBO to get CUDA device pointer
//	checkCudaErrors(cudaGraphicsMapResources(1, &cuda_pbo_resource, 0));
//	size_t num_bytes;
//	checkCudaErrors(cudaGraphicsResourceGetMappedPointer((void **)&d_output, &num_bytes,
//		cuda_pbo_resource));
//	//printf("CUDA mapped PBO: May access %ld bytes\n", num_bytes);
//
//	// clear image
//	checkCudaErrors(cudaMemset(d_output, 0, width*height * 4));
//
//	dim3 blockSize(16, 16);
//	dim3 gridSize = dim3(iDivUp(width, blockSize.x), iDivUp(height, blockSize.y));
//
//	// call CUDA kernel, writing results to PBO
//	SdogDB* database;
//
//	//render_kernel(gridSize, blockSize, d_output, wWidth, wHeight, database);
//	render_kernel(gridSize, blockSize, d_output, width, height, density, brightness, transferOffset, transferScale);
//
//	getLastCudaError("kernel failed");
//
//	checkCudaErrors(cudaGraphicsUnmapResources(1, &cuda_pbo_resource, 0));
//}

const bool intersect(const glm::vec3 &rayorig, const glm::vec3 &raydir, float &t0, float &t1)
{
	/*glm::vec3 l = center - rayorig;
	float tca = l.dot(raydir);
	if (tca < 0) return false;
	float d2 = l.dot(l) - tca * tca;
	if (d2 > radius2) return false;
	float thc = sqrt(radius2 - d2);
	t0 = tca - thc;
	t1 = tca + thc;

	return true;*/

	// Check cells for sigmets here
}

//glm::vec3 RayTracer::traceHelper(const Ray &ray, int depth)
void RayTracer::traceHelper(const Ray &ray, int depth, SdogDB* database)
{
	bool hasSigmet = false;

	std::string prevCode = "";

	int count = 0;
	int k = 0; 

	glm::vec3 rayPos;

	int continueCount = 0;

	while(count < 15) 
	{
		glm::vec3 tracePoint = ray.origin + (5.f * (float)k * glm::normalize(ray.dir));
		
		k++;

		SphCoord coord(tracePoint);

		//std::string code = SdogCell::codeForPos(coord.latitude, coord.longitude, coord.radius, (RADIUS_EARTH_KM * 4.f / 3.f), 10);
		std::string code = SdogCell::codeForPos(coord.latitude, coord.longitude, coord.radius, (RADIUS_EARTH_KM * 4.f / 3.f), 8);

		std::vector<AirSigmet> sigs;

		if (code.compare(prevCode) == 0) {
			continueCount++;
			if (continueCount > 100) break;
			//std::cout << "continuing" << std::endl;
			continue;
		}
		else if (stoi(code) == 0) break;

		continueCount = 0;
		std::cout << code << std::endl; 

		prevCode = code; 
		count++;

		rayPos = tracePoint;

		database->getAirSigmetForCell(code, sigs);
		if (sigs.size() == 0) continue;

		hasSigmet = true;

	}
	// point is illuminated
	//return object->color * light.brightness;

	//std::cout << "done casting a ray.";
	//std::cout << "Pos: " << rayPos.x << " " << rayPos.y << " " << rayPos.z << std::endl;
	if (hasSigmet)
	{
		std::cout << " Has a sigmet.";
		while (true) {}
	}
	//std::cout << std::endl;
}

void RayTracer::trace(Camera* c, SdogDB* database)
{
	std::cout << c->getPosition().x << " " << c->getPosition().y << " " << c->getPosition().z << std::endl;
	std::cout << c->getLookDir().x << " " << c->getLookDir().y << " " << c->getLookDir().z << std::endl;

	glm::vec3 *image = new glm::vec3[width * height], *pixel = image;


	float invWidth = 1 / float(width);
	float invHeight = 1 / float(height);
	float fov = 30.f;
	float aspectratio = (float)width / float(height);
	float angle = tan(M_PI * 0.5 * fov / 180.);


	std::cout << c->isCreated << std::endl;

	glm::vec3 source = c->getPosition();

	int count = 0;
	// for each pixel of the image
	//for (int j = 0; j < height; ++j) {
	//	for (int i = 0; i < width; ++i) {
			// compute primary ray direction
			/*int x = i - 1;
			int y = j - 1;

			float xx = (2 * ((x + 0.5) * invWidth) - 1) * angle * aspectratio;
			float yy = (1 - 2 * ((y + 0.5) * invHeight)) * angle;
			glm::vec3 raydir(xx, yy, -1);
			raydir = glm::normalize(raydir);*/

			glm::vec3 raydir = glm::vec3(0.f, 0.f, -1.f);
			Ray ray;
			ray.origin = source;
			ray.dir = raydir;

			traceHelper(ray, 0, database);
			count++;
		//}
		//std::cout << "Cast row " << j << " of " << height << std::endl;
	//}
	//std::cout << "cast " << count << " rays." << std::endl;
}

void RayTracer::resize(unsigned int w, unsigned int h)
{
	wWidth = w;
	wHeight = h;
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

void computeFPS()
{
	frameCount++;
	fpsCount++;

	if (fpsCount == fpsLimit)
	{
		char fps[256];
		float ifps = 1.f / (sdkGetAverageTimerValue(&timer) / 1000.f);
		//sprintf(fps, "Volume Render: %3.1f fps", ifps);

		//printf(fps);
		//printf("\n");

		// glutSetWindowTitle(fps);
		fpsCount = 0;

		fpsLimit = (int)MAX(1.f, ifps);
		sdkResetTimer(&timer);
	}
}

//Camera* RayTracer::camera;
//RenderEngine* RayTracer::renderEngine;
//Program* RayTracer::program;
//int RayTracer::mouseOldX;
//int RayTracer::mouseOldY;
//bool RayTracer::moved;
//
//// Must be called before processing any SDL2 events
//void RayTracer::setUp(Camera* camera, RenderEngine* renderEngine, Program* program) {
//	InputHandler::camera = camera;
//	InputHandler::renderEngine = renderEngine;
//	InputHandler::program = program;
//}
//
//void RayTracer::pollEvent(SDL_Event& e) {
//	if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
//		InputHandler::key(e.key);
//	}
//	else if (e.type == SDL_MOUSEBUTTONDOWN) {
//		moved = false;
//	}
//	else if (e.type == SDL_MOUSEBUTTONUP) {
//		InputHandler::mouse(e.button);
//	}
//	else if (e.type == SDL_MOUSEMOTION) {
//		InputHandler::motion(e.motion);
//	}
//	else if (e.type == SDL_MOUSEWHEEL) {
//		InputHandler::scroll(e.wheel);
//	}
//	else if (e.type == SDL_WINDOWEVENT) {
//		InputHandler::reshape(e.window);
//	}
//	else if (e.type == SDL_QUIT) {
//		SDL_Quit();
//		exit(0);
//	}
//}
//
//// Callback for key presses
//void RayTracer::key(SDL_KeyboardEvent& e) {
//
//	auto key = e.keysym.sym;
//
//	if (e.state == SDL_PRESSED) {
//		if (key == SDLK_f) {
//			renderEngine->toggleFade();
//		}
//		else if (key == SDLK_ESCAPE) {
//			SDL_Quit();
//			exit(0);
//		}
//	}
//}
//
//// Callback for mouse button presses
//void RayTracer::mouse(SDL_MouseButtonEvent& e) {
//	mouseOldX = e.x;
//	mouseOldY = e.y;
//}
//
//// Callback for mouse motion
//void RayTracer::motion(SDL_MouseMotionEvent& e) {
//	int dx, dy;
//	dx = e.x - mouseOldX;
//	dy = e.y - mouseOldY;
//
//	// left mouse button moves camera
//	if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
//		program->updateRotation(mouseOldX, e.x, mouseOldY, e.y, false);
//	}
//	else if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_MIDDLE)) {
//		program->updateRotation(mouseOldX, e.x, mouseOldY, e.y, true);
//	}
//
//	// Update current position of the mouse
//	int width, height;
//	SDL_Window* window = SDL_GetWindowFromID(e.windowID);
//	SDL_GetWindowSize(window, &width, &height);
//
//	int iX = e.x;
//	int iY = height - e.y;
//	//program->setMousePos(iX, iY);
//
//	mouseOldX = e.x;
//	mouseOldY = e.y;
//}
//
//// Callback for mouse scroll
//void RayTracer::scroll(SDL_MouseWheelEvent& e) {
//	int dy;
//	dy = e.y;
//
//	const Uint8 *state = SDL_GetKeyboardState(0);
//	if (state[SDL_SCANCODE_U]) {
//		//program->updateRadialBounds(RadialBound::MAX, -dy);
//	}
//	else if (state[SDL_SCANCODE_J]) {
//		//program->updateRadialBounds(RadialBound::MIN, -dy);
//	}
//	else if (state[SDL_SCANCODE_M]) {
//		//program->updateRadialBounds(RadialBound::BOTH, -dy);
//	}
//	else {
//		if (abs(dy) > 0) program->updateScale(dy);
//		//camera->updateZoom(dy);
//	}
//}
//
//// Callback for window reshape/resize
//void RayTracer::reshape(SDL_WindowEvent& e) {
//	if (e.event == SDL_WINDOWEVENT_RESIZED) {
//		renderEngine->setWindowSize(e.data1, e.data2);
//	}
//}


void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
#if defined (__APPLE__) || defined(MACOSX)
		exit(EXIT_SUCCESS);
#else
		glutDestroyWindow(glutGetWindow());
		return;
#endif
		break;

	case 'f':
		linearFiltering = !linearFiltering;
		setTextureFilterMode(linearFiltering);
		break;

	case '+':
		density += 0.01f;
		break;

	case '-':
		density -= 0.01f;
		break;

	case ']':
		brightness += 0.1f;
		break;

	case '[':
		brightness -= 0.1f;
		break;

	case ';':
		transferOffset += 0.01f;
		break;

	case '\'':
		transferOffset -= 0.01f;
		break;

	case '.':
		transferScale += 0.01f;
		break;

	case ',':
		transferScale -= 0.01f;
		break;

	default:
		break;
	}

	printf("density = %.2f, brightness = %.2f, transferOffset = %.2f, transferScale = %.2f\n", density, brightness, transferOffset, transferScale);
	glutPostRedisplay();
}

int ox, oy;
int buttonState = 0;

void mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		buttonState |= 1 << button;
	}
	else if (state == GLUT_UP)
	{
		buttonState = 0;
	}

	ox = x;
	oy = y;
	glutPostRedisplay();
}

void motion(int x, int y)
{
	float dx, dy;
	dx = (float)(x - ox);
	dy = (float)(y - oy);

	if (buttonState == 4)
	{
		// right = zoom
		viewTranslation.z += dy / 100.0f;
	}
	else if (buttonState == 2)
	{
		// middle = translate
		viewTranslation.x += dx / 100.0f;
		viewTranslation.y -= dy / 100.0f;
	}
	else if (buttonState == 1)
	{
		// left = rotate
		viewRotation.x += dy / 5.0f;
		viewRotation.y += dx / 5.0f;
	}

	ox = x;
	oy = y;
	glutPostRedisplay();
}

// display results using OpenGL (called by GLUT)
//void RayTracer::display(SdogDB* database)
void RayTracer::display()
{
	sdkStartTimer(&timer);

	// use OpenGL to build view matrix
	GLfloat modelView[16];
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glRotatef(-viewRotation.x, 1.0, 0.0, 0.0);
	glRotatef(-viewRotation.y, 0.0, 1.0, 0.0);
	glTranslatef(-viewTranslation.x, -viewTranslation.y, -viewTranslation.z);
	glGetFloatv(GL_MODELVIEW_MATRIX, modelView);
	glPopMatrix();

	invViewMatrix[0] = modelView[0];
	invViewMatrix[1] = modelView[4];
	invViewMatrix[2] = modelView[8];
	invViewMatrix[3] = modelView[12];
	invViewMatrix[4] = modelView[1];
	invViewMatrix[5] = modelView[5];
	invViewMatrix[6] = modelView[9];
	invViewMatrix[7] = modelView[13];
	invViewMatrix[8] = modelView[2];
	invViewMatrix[9] = modelView[6];
	invViewMatrix[10] = modelView[10];
	invViewMatrix[11] = modelView[14];

	//trace();
	//trace(database);

	// display results
	glClear(GL_COLOR_BUFFER_BIT);

	// draw image from PBO
	glDisable(GL_DEPTH_TEST);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
#if 0
	// draw using glDrawPixels (slower)
	glRasterPos2i(0, 0);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, pbo);
	glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
#else
	// draw using texture

	// copy from pbo to texture
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, pbo);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

	// draw textured quad
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex2f(0, 0);
	glTexCoord2f(1, 0);
	glVertex2f(1, 0);
	glTexCoord2f(1, 1);
	glVertex2f(1, 1);
	glTexCoord2f(0, 1);
	glVertex2f(0, 1);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
#endif

	glutSwapBuffers();
	glutReportErrors();

	sdkStopTimer(&timer);

	computeFPS();
}

void reshape(int w, int h)
{
	width = w;
	height = h;
	initPixelBuffer();

	// calculate new grid size
	gridSize = dim3(iDivUp(width, blockSize.x), iDivUp(height, blockSize.y));

	glViewport(0, 0, w, h);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 1.0, 0.0, 1.0, 0.0, 1.0);
}

void cleanup()
{
	sdkDeleteTimer(&timer);

	freeCudaBuffers();

	if (pbo)
	{
		cudaGraphicsUnregisterResource(cuda_pbo_resource);
		glDeleteBuffers(1, &pbo);
		glDeleteTextures(1, &tex);
	}
	// Calling cudaProfilerStop causes all profile data to be
	// flushed before the application exits
	checkCudaErrors(cudaProfilerStop());
}

void initGL(int *argc, char **argv)
{
	// initialize GLUT callback functions
	glutInit(argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(width, height);
	glutCreateWindow("CUDA volume rendering");

	if (!isGLVersionSupported(2, 0) ||
		!areGLExtensionsSupported("GL_ARB_pixel_buffer_object"))
	{
		printf("Required OpenGL extensions are missing.");
		exit(EXIT_SUCCESS);
	}
}

void initPixelBuffer()
{
	if (pbo)
	{
		// unregister this buffer object from CUDA C
		checkCudaErrors(cudaGraphicsUnregisterResource(cuda_pbo_resource));

		// delete old buffer
		glDeleteBuffers(1, &pbo);
		glDeleteTextures(1, &tex);
	}

	// create pixel buffer object for display
	glGenBuffers(1, &pbo);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, pbo);
	glBufferData(GL_PIXEL_UNPACK_BUFFER_ARB, width*height * sizeof(GLubyte) * 4, 0, GL_STREAM_DRAW_ARB);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

	// register this buffer object with CUDA
	checkCudaErrors(cudaGraphicsGLRegisterBuffer(&cuda_pbo_resource, pbo, cudaGraphicsMapFlagsWriteDiscard)); // This line caused errors(?)

	// create texture for display
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
}

// Load raw data from disk
void *loadRawFile(char *filename, size_t size)
{
	FILE *fp = fopen(filename, "rb");

	if (!fp)
	{
		fprintf(stderr, "Error opening file '%s'\n", filename);
		return 0;
	}

	void *data = malloc(size);
	size_t read = fread(data, 1, size, fp);
	fclose(fp);

#if defined(_MSC_VER_)
	printf("Read '%s', %Iu bytes\n", filename, read);
#else
	printf("Read '%s', %zu bytes\n", filename, read);
#endif

	return data;
}

void RayTracer::runSingleTest(const char *ref_file, const char *exec_path)
{
	bool bTestResult = true;

	uint *d_output;
	checkCudaErrors(cudaMalloc((void **)&d_output, width*height * sizeof(uint)));
	checkCudaErrors(cudaMemset(d_output, 0, width*height * sizeof(uint)));

	float modelView[16] =
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 4.0f, 1.0f
	};

	invViewMatrix[0] = modelView[0];
	invViewMatrix[1] = modelView[4];
	invViewMatrix[2] = modelView[8];
	invViewMatrix[3] = modelView[12];
	invViewMatrix[4] = modelView[1];
	invViewMatrix[5] = modelView[5];
	invViewMatrix[6] = modelView[9];
	invViewMatrix[7] = modelView[13];
	invViewMatrix[8] = modelView[2];
	invViewMatrix[9] = modelView[6];
	invViewMatrix[10] = modelView[10];
	invViewMatrix[11] = modelView[14];

	// call CUDA kernel, writing results to PBO
	copyInvViewMatrix(invViewMatrix, sizeof(float4) * 3);

	// Start timer 0 and process n loops on the GPU
	int nIter = 10;

	for (int i = -1; i < nIter; i++)
	{
		if (i == 0)
		{
			cudaDeviceSynchronize();
			sdkStartTimer(&timer);
		}

		//render_kernel(gridSize, blockSize, d_output, width, height, database);

		render_kernel(gridSize, blockSize, d_output, width, height, density, brightness, transferOffset, transferScale);

	}

	cudaDeviceSynchronize();
	sdkStopTimer(&timer);
	// Get elapsed time and throughput, then log to sample and master logs
	double dAvgTime = sdkGetTimerValue(&timer) / (nIter * 1000.0);
	printf("volumeRender, Throughput = %.4f MTexels/s, Time = %.5f s, Size = %u Texels, NumDevsUsed = %u, Workgroup = %u\n",
		(1.0e-6 * width * height) / dAvgTime, dAvgTime, (width * height), 1, blockSize.x * blockSize.y);


	getLastCudaError("Error: render_kernel() execution FAILED");
	checkCudaErrors(cudaDeviceSynchronize());

	unsigned char *h_output = (unsigned char *)malloc(width*height * 4);
	checkCudaErrors(cudaMemcpy(h_output, d_output, width*height * 4, cudaMemcpyDeviceToHost));

	sdkSavePPM4ub("volume.ppm", h_output, width, height);
	bTestResult = sdkComparePPM("volume.ppm", sdkFindFilePath(ref_file, exec_path), MAX_EPSILON_ERROR, THRESHOLD, true);

	cudaFree(d_output);
	free(h_output);
	cleanup();

	exit(bTestResult ? EXIT_SUCCESS : EXIT_FAILURE);
}

RayTracer::RayTracer(Camera* c) {
	std::cout << "set camera" << std::endl;

	camera = c;
	//char *ref_file = NULL;

	////start logs
	//printf("%s Starting...\n\n", sSDKsample);

	//int argc = 1;
	//char** argv;

	//// First initialize OpenGL context, so we can properly set the GL for CUDA.
	//// This is necessary in order to achieve optimal performance with OpenGL/CUDA interop.
	//initGL(&argc, argv);

	//findCudaDevice(argc, (const char **)argv);

	//std::cout << "argc: " << argc << std::endl;
	//std::cout << "argv: " << argv << std::endl;

	//// parse arguments
	//char *filename;

	//// load volume data
	//char *path = sdkFindFilePath(volumeFilename, argv[0]);

	//if (path == 0)
	//{
	//	printf("Error finding file '%s'\n", volumeFilename);
	//	exit(EXIT_FAILURE);
	//}

	//size_t size = volumeSize.width*volumeSize.height*volumeSize.depth * sizeof(VolumeType);
	//void *h_volume = loadRawFile(path, size);

	//initCuda(h_volume, volumeSize);
	//free(h_volume);

	//sdkCreateTimer(&timer);

	//printf("Press '+' and '-' to change density (0.01 increments)\n"
	//	"      ']' and '[' to change brightness\n"
	//	"      ';' and ''' to modify transfer function offset\n"
	//	"      '.' and ',' to modify transfer function scale\n\n");

	//// calculate new grid size
	//gridSize = dim3(iDivUp(width, blockSize.x), iDivUp(height, blockSize.y));

	//if (ref_file)
	//{
	//	runSingleTest(ref_file, argv[0]);
	//}
	//else
	//{
	//	// This is the normal rendering path for VolumeRender
	//	glutKeyboardFunc(keyboard);
	//	glutMouseFunc(mouse);
	//	glutMotionFunc(motion);
	//	glutReshapeFunc(reshape);

	//	initPixelBuffer();

	//	glutCloseFunc(cleanup);

	//	//glutMainLoop();
	//}
}
#include "RayTracer.h"

int iDivUp(int a, int b)
{
	return (a % b != 0) ? (a / b + 1) : (a / b);
}

glm::vec4 RayTracer::traceHelper(float x, float y, glm::mat4 invProjView, glm::mat4 worldModel, glm::vec3 camPos, SdogDB* database)
{
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

	char* cudaCode;

	checkCudaErrors(cudaMalloc((void **)&cudaCode, 20 * sizeof(char)));

	while(count < 750) 
	{
		glm::vec3 tracePoint = rayO + ((float)k * glm::normalize(rayD));
		k += 50;

		checkCudaErrors(cudaMemset(cudaCode, 0, 20 * sizeof(char)));

		cudaCodeForPos(tracePoint.x, tracePoint.y, tracePoint.z, cudaCode);

		getLastCudaError("kernel failed");

		char *charCode = (char *)malloc(20 * sizeof(char));
		checkCudaErrors(cudaMemcpy(charCode, cudaCode, 20 * sizeof(char), cudaMemcpyDeviceToHost));

		//std::cout << "Code:     " << code << std::endl;
		//std::cout << "charCode: " << charCode << std::endl;
		//std::cout << "done" << std::endl;
		/*if (code.compare(std::string(charCode)) != 0) {
			std::cout << "codes don't match!" << std::endl;
		}*/

		std::vector<AirSigmet> sigs;

		std::string code(charCode);


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
	//std::cout << "about to trace" << std::endl;

	std::vector<std::vector<glm::vec4>> buffer;

	glm::vec4 cPos = glm::inverse(worldModel) * glm::vec4(c->getPosition(), 1.0);

	//std::cout << "camera pos = " << cPos.x << " " << cPos.y << " " << cPos.z << std::endl;

	//uint* d_output;
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

	glm::vec4 colour = traceHelper(0.f, 0.f, invProjView, worldModel, cPos, database);

	if (colour.w > 0) std::cout << "Hit sigmet" << std::endl;


	//for (int i = 0; i < wWidth; i++) {
	//	std::vector<glm::vec4> row;
	//	for (int j = 0; j < wHeight; j++) {

	//		float w = (((float)i / (float)wWidth) * 2.f) - 1.f;
	//		float h = (((float)j / (float)wHeight) * 2.f) - 1.f;

	//		row.push_back(traceHelper(w, h, invProjView, worldModel, cPos, database));
	//	}

	//	std::cout << "Done row " << i << std::endl;
	//	buffer.push_back(row);
	//}

	glDisable(GL_DEPTH_TEST);
	glPointSize(2.f);

	glEnable(GL_BLEND);
	/*glBegin(GL_POINTS);

	glColor4f(1.f, 0.f, 0.f, 0.3f);

	for (int i = 0; i < wWidth; i++) {
		for (int j = 0; j < wHeight; j++) {
			glm::vec4 val = buffer[i][j];
			glColor4f(val.x, val.y, val.z, val.w);

			float w = ((float)i / (float)wWidth) * 2.f - 1.f;
			float h = ((float)j / (float)wHeight) * 2.f - 1.f;

			if (val.w > 0.f) glVertex2f(w, h);
		}
	}
	glEnd();*/
}

void RayTracer::resize(unsigned int w, unsigned int h)
{
	//wWidth = w;
	//wHeight = h;
}


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
	glBufferData(GL_PIXEL_UNPACK_BUFFER_ARB, wWidth*wHeight * sizeof(GLubyte) * 4, 0, GL_STREAM_DRAW_ARB);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

	// register this buffer object with CUDA
	checkCudaErrors(cudaGraphicsGLRegisterBuffer(&cuda_pbo_resource, pbo, cudaGraphicsMapFlagsWriteDiscard)); // This line caused errors(?)
}

void RayTracer::addToCache(AirSigmet sigmet, std::string code) {
	Cell cell;
	cell.sigmet = sigmet;
	cell.code = code;

	dataCache.push_back(cell);
}

RayTracer::RayTracer(Camera* c, int argc, char** argv) : camera(c) {
	std::cout << "set camera" << std::endl;

	pbo = 0;

	cudaExtent volumeSize = make_cudaExtent(32, 32, 32);

	findCudaDevice(argc, (const char **)argv);

	wWidth = 800;
	wHeight = 800;

	std::cout << "w: " << wWidth << " h: " << wHeight << std::endl;

	// calculate new grid size
	blockSize = dim3(16, 16);
	gridSize = dim3(iDivUp(wWidth, blockSize.x), iDivUp(wHeight, blockSize.y));

	initPixelBuffer();

}
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

// Simple 3D volume renderer

#include <iostream>
#include <stdio.h>

#ifndef _VOLUMERENDER_KERNEL_CU_
#define _VOLUMERENDER_KERNEL_CU_

#include <helper_cuda.h>
#include <helper_math.h>

#include "Constants.h"
#include "Renderable.h"
#include "SdogDB.h"

#include <glm/gtx/intersect.hpp>

typedef unsigned int  uint;
typedef unsigned char uchar;

typedef unsigned char VolumeType;

typedef struct
{
	float4 m[3];
} float3x4;

typedef struct
{
	float4 m[4];
} float4x4;

enum SdogCellType {
	NG,
	LG,
	SG,
	INVALID
};

__constant__ float3x4 c_invViewMatrix;  // inverse view matrix


__device__ struct SphereCoord
{
	SphereCoord(float x, float y, float z) {
		radius = glm::length(glm::vec3(x, y, z));
		latitude = asin(y / radius);
		longitude = atan2(x, z);
	};

	double latitude;
	double longitude;
	double radius;
};

// transform vector by matrix (no translation)
__device__
float3 mul(const float3x4 &M, const float3 &v)
{
	float3 r;
	r.x = dot(v, make_float3(M.m[0]));
	r.y = dot(v, make_float3(M.m[1]));
	r.z = dot(v, make_float3(M.m[2]));
	return r;
}

// transform vector by matrix with translation
__device__
float4 mul(const float3x4 &M, const float4 &v)
{
	float4 r;
	r.x = dot(v, M.m[0]);
	r.y = dot(v, M.m[1]);
	r.z = dot(v, M.m[2]);
	r.w = 1.0f;
	return r;
}

// transform vector by matrix with translation
__device__
float4 mul(const float4x4 &M, const float4 &v)
{
	float4 r;
	r.x = dot(v, M.m[0]);
	r.y = dot(v, M.m[1]);
	r.z = dot(v, M.m[2]);
	r.w = dot(v, M.m[3]);
	return r;
}


__device__ uint rgbaFloatToInt(float4 rgba)
{
	rgba.x = __saturatef(rgba.x);   // clamp to [0.0, 1.0]
	rgba.y = __saturatef(rgba.y);
	rgba.z = __saturatef(rgba.z);
	rgba.w = __saturatef(rgba.w);
	return (uint(rgba.w * 255) << 24) | (uint(rgba.z * 255) << 16) | (uint(rgba.y * 255) << 8) | uint(rgba.x * 255);
}

//__device__ std::string code = SdogCell::codeForPos(coord.latitude, coord.longitude, coord.radius, (RADIUS_EARTH_KM * 4.f / 3.f), 8);

__global__ void
d_render(uint *d_output, uint imageW, uint imageH, float4x4 invProjView, float4x4 invWorldModel, float3 camPos, SdogDB* database)
{

	//uint x = blockIdx.x*blockDim.x + threadIdx.x;
	//uint y = blockIdx.y*blockDim.y + threadIdx.y;

	//if ((x >= imageW) || (y >= imageH)) return;

	//float4 worldNew = make_float4(x, y, -1.f, 1.f);

	//worldNew = mul(invProjView, worldNew);

	//worldNew.x /= worldNew.w;
	//worldNew.y /= worldNew.w;
	//worldNew.z /= worldNew.w;

	//worldNew = mul(invWorldModel, worldNew);

	//float3 rayO = camPos;

	//float3 rayD = normalize(make_float3(worldNew.x, worldNew.y, worldNew.z) - rayO);

	//float4 colour = make_float4(0.f, 0.f, 0.f, 0.f);

	//int continueCount = 0;
	//int count = 0;
	//int k = 0;
	//std::string prevCode = "";

	//while (count < 750)
	//{
	//	float3 tracePoint = rayO + ((float)k * normalize(rayD));

	//	k += 50;

	//	//glm::vec4 t = inverseModel * glm::vec4(tracePoint, 1.0);
	//	//tracePoint = glm::vec3(t.x, t.y, t.z);

	//	SphereCoord coord(tracePoint.x, tracePoint.y, tracePoint.z);

	//	std::string code = SdogCell::codeForPos(coord.latitude, coord.longitude, coord.radius, (RADIUS_EARTH_KM * 4.f / 3.f), 8);

	//	std::vector<AirSigmet> sigs;

	//	if (code.compare(prevCode) == 0) {
	//		continueCount++;
	//		if (continueCount > 1000)
	//		{
	//			//std::cout << "too many continues" << std::endl;

	//			break;
	//		}
	//		//std::cout << "continuing" << std::endl;
	//		continue;
	//	}
	//	else if (strtol(code.c_str(), (char**)NULL, 10) == 0) break;

	//	continueCount = 0;
	//	//std::cout << code << std::endl; 

	//	prevCode = code;
	//	count++;

	//	database->getAirSigmetForCell(code, sigs);

	//	if (sigs.size() == 0) continue;

	//	//std::cout << "has sigmet" << std::endl;

	//	colour = make_float4(1.f, 0.f, 0.f, 0.5f);
	//	break;

	//}

	//// write output color
	//d_output[y*imageW + x] = rgbaFloatToInt(colour);
}

extern "C"
void initCuda(void *h_volume, cudaExtent volumeSize)
{
	// create 3D array
	cudaChannelFormatDesc channelDesc = cudaCreateChannelDesc<VolumeType>();
	//checkCudaErrors(cudaMalloc3DArray(&d_volumeArray, &channelDesc, volumeSize));

	// copy data to 3D array
	cudaMemcpy3DParms copyParams = { 0 };
	copyParams.srcPtr = make_cudaPitchedPtr(h_volume, volumeSize.width * sizeof(VolumeType), volumeSize.width, volumeSize.height);
	//copyParams.dstArray = d_volumeArray;
	copyParams.extent = volumeSize;
	copyParams.kind = cudaMemcpyHostToDevice;
	checkCudaErrors(cudaMemcpy3D(&copyParams));

	// bind array to 3D texture
	//checkCudaErrors(cudaBindTextureToArray(tex, d_volumeArray, channelDesc));

	// create transfer function texture
	float4 transferFunc[] =
	{
		{ 0.0, 0.0, 0.0, 0.0, },
	{ 1.0, 0.0, 0.0, 1.0, },
	{ 1.0, 0.5, 0.0, 1.0, },
	{ 1.0, 1.0, 0.0, 1.0, },
	{ 0.0, 1.0, 0.0, 1.0, },
	{ 0.0, 1.0, 1.0, 1.0, },
	{ 0.0, 0.0, 1.0, 1.0, },
	{ 1.0, 0.0, 1.0, 1.0, },
	{ 0.0, 0.0, 0.0, 0.0, },
	};

	cudaChannelFormatDesc channelDesc2 = cudaCreateChannelDesc<float4>();
	cudaArray *d_transferFuncArray;
	checkCudaErrors(cudaMallocArray(&d_transferFuncArray, &channelDesc2, sizeof(transferFunc) / sizeof(float4), 1));
	checkCudaErrors(cudaMemcpyToArray(d_transferFuncArray, 0, 0, transferFunc, sizeof(transferFunc), cudaMemcpyHostToDevice));
}


extern "C"
void render_kernel(dim3 gridSize, dim3 blockSize, uint *d_output, uint imageW, uint imageH, float4x4 invProjView,
	float4x4 invWorldModel, float3 camPos, SdogDB* database)
{
	d_render << <gridSize, blockSize >> > (d_output, imageW, imageH, invProjView, invWorldModel, camPos, database);
}

// These two functions from https://stackoverflow.com/questions/20201335/add-char-arrays-in-cuda
__device__ char * my_strcpy(char dest[], const char src[]) {
	int i = 0;
	do {
		dest[i] = src[i];
	} while (src[i++] != 0);
	return dest;
}

__device__ char* my_strcat(char dest[], const char src[]) {
	int i = 0;
	while (dest[i] != 0) i++;
	my_strcpy(dest + i, src);
	return dest;
}

// Returns the SDOG cell code for the provided spherical point at a given subdivision level - Converted to CUDA code from Ben's C++ function
__global__ void
cudaCodeForPos_kernel(double latRad, double longRad, double radius, double gridRadius, unsigned int level, char* returnCode) {

	double M_PI = 3.14159265358979323846;
	double M_PI_2 = M_PI / 2.0;

	char code[15];
	if (longRad < -M_PI) {
		longRad += 2.0 * M_PI;
	}
	if (longRad > M_PI) {
		longRad -= 2.0 * M_PI;
	}

	double minLat, maxLat, minLong, maxLong, minRad, maxRad;
	minLat = 0.0;
	maxLat = M_PI_2;
	minRad = 0.0;
	maxRad = gridRadius;

	// Determine which otcant the point is in
	char octCode = '0';
	if (latRad < 0.0) {
		octCode = '4';
		//octCode = 4;
	}
	if (longRad < 0.0) {
		if (octCode == '0') {
			octCode = '2';
		}
		else if (octCode == '4') {
			octCode = '6';
		}
		//octCode += 2;
	}
	if (abs(longRad) > M_PI_2) {
		if (octCode == '0') {
			octCode = '1';
		}
		else if (octCode == '4') {
			octCode = '5';
		}
		else if (octCode == '2') {
			octCode = '3';
		}
		else if (octCode == '6') {
			octCode = '7';
		}

		//octCode += 1;
		minLong = M_PI_2;
		maxLong = M_PI;
	}
	else {
		minLong = 0.0;
		maxLong = M_PI_2;
	}
	latRad = abs(latRad);
	longRad = abs(longRad);

	code[0] = octCode;

	// Loop for desired number of levels and determine
	// which child point is in for each itteration
	SdogCellType curType = SdogCellType::SG;
	for (unsigned int i = 0; i < level; i++) {

		char childCode = '0';
		double midLat = 0.5 * minLat + 0.5 * maxLat;
		double midLong = 0.5 * minLong + 0.5 * maxLong;
		double midRad = 0.5 * minRad + 0.5 * maxRad;

		if (curType == SdogCellType::NG) {

			if (radius > midRad) {
				minRad = midRad;
			}
			else {
				childCode = '4';
				//childCode += 4;
				maxRad = midRad;
			}
			if (latRad < midLat) {
				maxLat = midLat;
			}
			else {
				if (childCode == '0') {
					childCode = '2';
				}
				else childCode = '6';
				//childCode += 2;
				minLat = midLat;
			}
			if (longRad < midLong) {
				maxLong = midLong;
			}
			else {
				if (childCode == '0') {
					childCode = '1';
				}
				else if (childCode == '2') {
					childCode = '3';
				}
				else if (childCode == '4') {
					childCode = '5';
				}
				else if (childCode == '6') {
					childCode = '7';
				}
				//childCode += 1;
				minLong = midLong;
			}
			// type doesn't change
		}
		// childCode = '0'
		else if (curType == SdogCellType::LG) {

			if (radius > midRad) {
				minRad = midRad;
			}
			else {
				maxRad = midRad;
				childCode = '3';
				//childCode += 3;
			}
			if (latRad < midLat) {
				maxLat = midLat;
				curType = SdogCellType::NG;

				if (longRad < midLong) {
					maxLong = midLong;
				}
				else {
					if (childCode == '0') {
						childCode = '1';
					}
					else if (childCode == '3') {
						childCode = '4';
					}
					//childCode += 1;
					minLong = midLong;
				}
			}
			// childCode = 0 or 3
			else {
				if (childCode == '0') {
					childCode = '2';
				}
				else if (childCode == '3') {
					childCode = '5';
				}
				//childCode += 2;
				minLat = midLat;
				// type doesn't change
			}
		}
		// childCode = '0'
		else {// curType == SdogCellType::SG

			if (radius > midRad) {

				minRad = midRad;

				if (latRad < midLat) {
					maxLat = midLat;
					curType = SdogCellType::NG;

					if (longRad < midLong) {
						childCode = '0';
						maxLong = midLong;
					}
					else {
						childCode = '1';
						minLong = midLong;
					}
				}
				else {
					childCode = '2';
					minLat = midLat;
					curType = SdogCellType::LG;
				}
			}
			else {
				childCode = '3';
				maxRad = midRad;
				// type doesn't change
			}
		}
		code[i + 1] = childCode;
	}

	int i = 0;
	do {
		returnCode[i] = code[i];
	} while (code[i++] != 0);
}

// latRad - latitude of point, in radians
// longRad - longitude of point, in radians
// radius - radius of point (0 is centre of the sphere)
// gridRadius - radius of the grid the cell belongs to
// return - code for the cell that contains the point
extern "C"
void cudaCodeForPos(double latRad, double longRad, double radius, double gridRadius, unsigned int level, char* returnCode) {
	cudaCodeForPos_kernel << <1, 1 >> >(latRad, longRad, radius, gridRadius, level, returnCode);
}

__global__ void
getInteger_kernel(uint* returnInt) {
	returnInt[0] = 12;
}

extern "C"
void getInteger(uint* returnInt) {
	getInteger_kernel <<<1,1>>>(returnInt);
}

#endif // #ifndef _VOLUMERENDER_KERNEL_CU_

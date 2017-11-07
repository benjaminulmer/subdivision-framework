#pragma once

#include "SphericalGrid.h"

class VolumetricSphericalHierarchy {

public:
	VolumetricSphericalHierarchy(GridInfo& info);
	virtual ~VolumetricSphericalHierarchy();

	// TODO these at some point for completeness
	//Sdog(const Sdog& other);
	//Sdog& operator= (const Sdog& other);

	//static Scheme scheme;
	//static bool cull;
	//static double maxRadius, minRadius, maxLat, minLat, maxLong, minLong;

	GridInfo info;

	void subdivideTo(int level, bool wholeSphere = false);
	void createRenderable(Renderable& r, int level, float max, float min, float avg, bool lines = false, bool wholeSphere = false);

	void getVolumes(std::vector<float>& volumes, int level, bool wholeSphere = false);

private:
	//	double radius;
	SphericalGrid* octants[8];

	int numLevels;
};
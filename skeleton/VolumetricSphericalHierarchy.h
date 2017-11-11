#pragma once

#include "SphericalGrid.h"

class VolumetricSphericalHierarchy {

public:
	VolumetricSphericalHierarchy(GridInfo& info);
	virtual ~VolumetricSphericalHierarchy();

	// TODO these at some point for completeness
	//Sdog(const Sdog& other);
	//Sdog& operator= (const Sdog& other);

	void updateInfo(GridInfo& info) {
		this->info = info;
	}

	void subdivideTo(int level);
	void createRenderable(Renderable& r, int level, DisplayMode mode);
	void getVolumes(std::vector<float>& volumes, int level);

	void fillData(int level);

private:
	SphericalGrid* octants[8];
	GridInfo info;
	int numLevels;
};
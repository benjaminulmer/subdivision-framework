#pragma once

#include "Geometry.h"
#include "RenderEngine.h"

enum class Scheme {
	SDOG,
	SDOG_OPT,
	NAIVE,
	VOLUME_SDOG,
	VOLUME
};

enum class BigSmall {
	BIG,
	SMALL,
	BOTH
};

enum class GridType {
	NG,
	LG,
	SG,
};

class SdogGrid {
	
public:
	SdogGrid(GridType type, int depth, double maxRadius, double minRadius, 
	         double maxLat, double minLat, double maxLong, double minLong);
	virtual ~SdogGrid();

	void setBigSmall(BigSmall bigRad, BigSmall bigLat) {
		this->bsRad = bigRad;
		this->bsLat = bigLat;
	}

	void subdivideTo(int level);
	void createRenderable(Renderable& r, int level);

	void getVolumes(std::vector<float>& volumes, int level);

private:
	GridType type;

	int depth;
	BigSmall bsRad;
	BigSmall bsLat;

	double maxRadius, minRadius;
	double maxLat, minLat;
	double maxLong, minLong;

	SdogGrid* children[8];
	int numChildren;
	bool leaf;

	void subdivide();
	bool inRange();
	void fillRenderable(Renderable& r);
};

class Sdog {

public:
	Sdog(Scheme scheme, double radius = 2.0);
	Sdog(const Sdog& other);
	Sdog& operator= (const Sdog& other);
	virtual ~Sdog();

	static Scheme scheme;
	static bool cull;
	static double maxRadius, minRadius, maxLat, minLat, maxLong, minLong;

	void subdivideTo(int level, bool wholeSphere = false);
	void createRenderable(Renderable& r, int level, bool wholeSphere = false);
	
	void getVolumes(std::vector<float>& volumes, int level, bool wholeSphere = false);

private:
	double radius;
	SdogGrid* octants[8];

	int numLevels;
};


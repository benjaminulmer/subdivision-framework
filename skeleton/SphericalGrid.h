#pragma once

#include "SphericalData.h"
#include "Geometry.h"
#include "RenderEngine.h"

enum class SubdivisionMode {
	FULL,
	OCTANT,
	REP_SLICE
};

enum class DisplayMode {
	DATA,
	VOLUMES,
	LINES
};

enum class Scheme {
	SDOG,
	SDOG_OPT,
	NAIVE,
	VOLUME_SDOG,
	VOLUME
};

enum class GridType {
	NG,
	LG,
	SG
};

struct GridInfo {
	Scheme scheme;
	SubdivisionMode mode;
	bool cull;
	double radius;
	double cullMaxRadius, cullMinRadius, cullMaxLat, cullMinLat, cullMaxLong, cullMinLong;

	float volMin, volMax, volAvg;

	SphericalData data;
};

struct volInfo {
	float max, min, avg;
};

class SphericalGrid {
	
public:
	SphericalGrid(GridType type, const GridInfo& info, double maxRadius, double minRadius, 
	              double maxLat, double minLat, double maxLong, double minLong);
	virtual ~SphericalGrid();

	static std::vector<volInfo> volInfos;

	void subdivideTo(int level);
	bool contains(const SphericalDatum& d);
	void fillData(const SphericalDatum& d, int level);
	void createRenderable(Renderable& r, int level, DisplayMode mode);
	void getVolumes(std::vector<float>& volumes, int level);

private:
	GridType type;

	double maxRadius, minRadius;
	double maxLat, minLat;
	double maxLong, minLong;

	SphericalGrid* children[8];
	const GridInfo& info;

	int numChildren;
	bool leaf;

	SphericalData data;

	void subdivide();
	bool inRange();
	void fillRenderable(Renderable& r, DisplayMode mode);
};


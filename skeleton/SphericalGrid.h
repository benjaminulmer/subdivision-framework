#pragma once

#include "SphericalData.h"
#include "Geometry.h"
#include "RenderEngine.h"

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
	bool cull;
	double radius;
	double cullMaxRadius, cullMinRadius, cullMaxLat, cullMinLat, cullMaxLong, cullMinLong;

	SphericalData data;
};

class SphericalGrid {
	
public:
	SphericalGrid(GridType type, const GridInfo& info, double maxRadius, double minRadius, 
	              double maxLat, double minLat, double maxLong, double minLong);
	virtual ~SphericalGrid();

	void subdivideTo(int level);
	void fillData(int level);
	void createRenderable(Renderable& r, int level, bool lines);
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

	float dataSum;
	int dataCount;




	void subdivide();
	bool inRange();
	void fillRenderable(Renderable& r, bool lines);
};


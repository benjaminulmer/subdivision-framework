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
	NAIVE,
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

class SphericalGrid {
	
public:
	SphericalGrid(GridType type, const GridInfo& info, double maxRadius, double minRadius, 
	              double maxLat, double minLat, double maxLong, double minLong);
	virtual ~SphericalGrid();

	bool contains(const SphericalDatum& d);
	void fillData(const SphericalDatum& d, int level);
	void subdivideTo(int level);
	void createRenderable(Renderable& r, int level, DisplayMode mode);
	void getVolumes(std::vector<float>& volumes, int level);

private:
	GridType type;

	double maxRadius, minRadius;
	double maxLat, minLat;
	double maxLong, minLong;

	std::vector<SphericalGrid*> children;
	const GridInfo& info;

	SphericalData data;

	void subdivide();
	void repSliceSubdivision(double midRadius, double midLat, double midLong);
	void fullSubdivision(double midRadius, double midLat, double midLong);
	bool inRange();
	void faceRenderable(Renderable& r);
	void lineRenderable(Renderable& r);
	glm::vec3 getDataColour();
	glm::vec3 getVolumeColour();
	void fillRenderable(Renderable& r, DisplayMode mode);
};
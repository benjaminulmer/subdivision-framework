#pragma once

#include "SphericalData.h"
#include "Geometry.h"
#include "RenderEngine.h"

enum class SubdivisionMode {
	FULL,
	SELECTION,
	REP_SLICE
};

enum class DisplayMode {
	DATA,
	VOLUMES,
	LINES
};

enum class Scheme {
	SDOG,
	OPT_SDOG,
	NAIVE,
	VOLUME,
	TERNARY
};

enum class GridType {
	NG,
	LG,
	SG
};

struct GridBounds {
	double maxRadius, minRadius;
	double maxLat, minLat;
	double maxLong, minLong;
};

struct GridInfo {
	Scheme scheme;
	SubdivisionMode mode;
	bool culling;
	double radius;
	GridBounds cull;
	GridBounds selection;

	float volMin, volMax, volAvg, volSD;
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
	int getNumGrids();

private:
	GridType type;

	double maxRadius, minRadius;
	double maxLat, minLat;
	double maxLong, minLong;

	std::vector<SphericalGrid*> children;
	const GridInfo& info;

	SphericalData data;

	void subdivide();
	void binarySubdivide();
	void ternarySubdivide();
	void repSliceSubdivision(double midRadius, double midLat, double midLong);
	void fullSubdivision(double midRadius, double midLat, double midLong);

	bool outsideBounds(GridBounds b);

	void fillRenderable(Renderable& r, DisplayMode mode);
	void faceRenderable(Renderable& r);
	void lineRenderable(Renderable& r);
	glm::vec3 getDataColour();
	glm::vec3 getVolumeColour();
};
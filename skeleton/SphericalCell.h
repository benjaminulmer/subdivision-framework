#pragma once

#include <vector>

#include "Renderable.h"
#include "SphericalData.h"

#include "Frustum.h"

class Frustum;

enum class SubdivisionMode {
	FULL,
	SELECTION
};

enum class DisplayMode {
	DATA,
	LINES
};

enum class Scheme {
	SDOG,
	OPT_SDOG,
};

enum class CellType {
	NG,
	LG,
	SG
};

struct CellBounds {
	double maxRadius, minRadius;
	double maxLat, minLat;
	double maxLong, minLong;
};

struct GridInfo {
	Scheme scheme;
	SubdivisionMode mode;
	bool culling;
	double radius;
	CellBounds cull;
	CellBounds selection;
};

class SphericalCell {
	
public:
	SphericalCell(CellType type, const GridInfo& info, double maxRadius, double minRadius, 
	              double maxLat, double minLat, double maxLong, double minLong);
	virtual ~SphericalCell();

	void cornerPoints(glm::vec3& o1, glm::vec3& o2, glm::vec3& o3, glm::vec3& o4, 
					  glm::vec3& i1, glm::vec3& i2, glm::vec3& i3, glm::vec3& i4) const;

	bool contains(const SphericalDatum& d) const;
	void fillData(const SphericalDatum& d, int level, const DataSetInfo& info);
	void subdivideTo(int level);
	void createRenderable(Renderable& r, int level, DisplayMode mode);

	static Frustum frust;

private:
	CellType type;

	double maxRadius, minRadius;
	double maxLat, minLat;
	double maxLong, minLong;

	std::vector<SphericalCell*> children;
	const GridInfo& info;

	std::vector<DataPoints> dataSets;

	void addDataPoint(const SphericalDatum& d, const DataSetInfo& info);

	void subdivide();
	void subdivisionSplit(double midRadius, double midLat, double midLong);

	bool outsideBounds(CellBounds b);

	void fillRenderable(Renderable& r, DisplayMode mode);
	void faceRenderable(Renderable& r);
	void lineRenderable(Renderable& r);
	glm::vec3 getDataColour();
};
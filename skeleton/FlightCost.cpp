#include "FlightCost.h"
#include "SdogCell.h"

#include <tuple>

double FlightCost::GreatCircleArcDistance(std::string startCode, std::string endCode, double radius) {
	SdogCell s{startCode, radius};
	SdogCell e{endCode, radius};

	SphCoord s_coord;
	double s_midRad;
	std::tie(s_coord, s_midRad) = s.getMidPoint();
	SphCoord e_coord;
	double e_midRad;
	std::tie(e_coord, e_midRad) = e.getMidPoint();

	// TODO: consider how to do this when start/end aren't at the same radius
	return s_coord.arcLength(e_coord) * s_midRad;
}

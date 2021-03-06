#include "AirSigmet.h"

#include "Constants.h"
#include "SdogCell.h"

#include <map>

// Finds cells of a grid with given radius, up to the provided depth, that are inside and on the boudnary of the AirSigmet
//
// gridRadius - radius of the SDOG grid to insert into
// maxDepth - maximum depth (subdivision level) to find cells for
// interior - output list of all interior cells - treats as empty
// boundary - output list of all boundary cells - treats as empty
#include <iostream>
void AirSigmet::gridInsertion(double gridRadius, int maxDepth, std::vector<std::string>& interior, std::vector<std::string>& boundary) const {

	// Enum for identifying boundary, interior, and exterior cases
	enum {
		NONE = -1,
		INTER = 0,
		EXTER = 1,
		BOUND = 2
	};

	// Struct and map for caching horizontal results for cells with same spherical bounds (but different radius bounds)
	struct Bounds {
		Bounds() {}
		Bounds(double minLat, double minLong, double maxLat, double maxLong) :
			minLat(minLat), minLong(minLong), maxLat(maxLat), maxLong(maxLong) {}
		bool operator<(const Bounds& r) const {
			return std::tie(minLat, minLong, maxLat, maxLong) < std::tie(r.minLat, r.minLong, r.maxLat, r.maxLong);
		}
		double minLat, minLong, maxLat, maxLong;
	};
	std::map<Bounds, int> cache;

	// Create list of cells to process and populate with octants
	std::vector<SdogCell> toTest;
	toTest.push_back(SdogCell("0", gridRadius));
	toTest.push_back(SdogCell("1", gridRadius));
	toTest.push_back(SdogCell("2", gridRadius));
	toTest.push_back(SdogCell("3", gridRadius));
	toTest.push_back(SdogCell("4", gridRadius));
	toTest.push_back(SdogCell("5", gridRadius));
	toTest.push_back(SdogCell("6", gridRadius));
	toTest.push_back(SdogCell("7", gridRadius));

	// Loop until no more cells to process
	while (toTest.size() > 0) {

		SdogCell c = toTest[toTest.size() - 1];
		toTest.pop_back();

		if (c.getCode() == "32056445647456465646660") {
			std::cout << std::endl;
		}

		int horizontal = NONE;
		int vertical = NONE;

		// Check if horizontal results for this cell are already known
		Bounds b(c.getMinLat(), c.getMinLong(), c.getMaxLat(), c.getMaxLong());
		if (cache.count(b) == 1) {
			horizontal = cache[b];
		}
		else {

			// Test bounds of cells against all lines in polygon
			for (int i = 0; i < polygon.size(); i++) {

				SphCoord start = polygon[i];
				SphCoord end = polygon[(i + 1) % polygon.size()];
				SphCoord inter;

				if (SphCoord::greatCircleArc2Intersect(start, end, SphCoord(c.getMinLat(), c.getMinLong()), SphCoord(c.getMaxLat(), c.getMinLong()), inter)) {
					horizontal = BOUND;
					break;
				}
				if (SphCoord::greatCircleArc2Intersect(start, end, SphCoord(c.getMinLat(), c.getMaxLong()), SphCoord(c.getMaxLat(), c.getMaxLong()), inter)) {
					horizontal = BOUND;
					break;
				}
				if (SphCoord::greatCircleArcLatIntersect(start, end, c.getMaxLat(), c.getMinLong(), c.getMaxLong(), inter)) {
					horizontal = BOUND;
					break;
				}
				if (SphCoord::greatCircleArcLatIntersect(start, end, c.getMinLat(), c.getMinLong(), c.getMaxLong(), inter)) {
					horizontal = BOUND;
					break;
				}
			}
			// No intersections, test for disjoint and contained cases
			if (horizontal == NONE) {

				double tLat = polygon[0].latitude;
				double tLong = polygon[0].longitude;

				// Test for polygon inside of cell, treat as boundary case (need to subdivide)
				if (((c.getMinLat() < tLat && tLat < c.getMaxLat()) || (c.getMaxLat() < tLat && tLat < c.getMinLat())) &&
					((c.getMinLong() < tLong && tLong < c.getMaxLong()) || (c.getMaxLong() < tLong && tLong < c.getMinLong()))) {
					horizontal = BOUND;
				}
				else {
					SphCoord point1(c.getMinLat(), c.getMinLong());
					SphCoord point2(c.getMaxLat(), c.getMaxLong());

					double ang1 = acos(glm::dot(point1.toCartesian(1.0), polygon[0].toCartesian(1.0)));
					double ang2 = acos(glm::dot(point2.toCartesian(1.0), polygon[0].toCartesian(1.0)));

					glm::dvec3 testNorm = (ang1 < ang2) ? point1.toCartesian(1.0) : point2.toCartesian(1.0);

					// Calculate winding number of a point in the cell with respect to the polygon
					double sum = 0.0;
					for (int i = 0; i < polygon.size(); i++) {

						glm::dvec3 plane1 = glm::normalize(glm::cross(testNorm, polygon[i].toCartesian(1.0)));
						glm::dvec3 plane2 = glm::normalize(glm::cross(testNorm, polygon[(i + 1) % polygon.size()].toCartesian(1.0)));

						glm::dvec3 cross = glm::cross(plane1, plane2);
						double dot = glm::dot(plane1, plane2);

						if (dot > 1.0) dot = 1.0;
						if (dot < -1.0) dot = -1.0;

						double angle = (glm::dot(cross, testNorm) < 0.0) ? -acos(dot) : acos(dot);
						sum += angle;
					}
					horizontal = (abs(sum) < 1.0) ? EXTER : INTER;
				}
			}
			// Cache results
			cache[b] = horizontal;
		}

		// Veritcal test
		if (c.getMinRad() > altToAbs(minAltM) - 0.001 && c.getMaxRad() < altToAbs(maxAltM) + 0.001) {
			vertical = INTER;
		}
		else if (c.getMaxRad() < altToAbs(minAltM) + 0.001 || c.getMinRad() > altToAbs(maxAltM) - 0.001) {
			vertical = EXTER;
		}
		else {
			vertical = BOUND;
		}

		// If cell is interior or boundary act accordingly - do nothing with exterior cells
		if (horizontal == INTER && vertical == INTER) {

			//if (c.getCode().length() == 23 && c.getMinLong() > -1.99192244) {
			//	std::cout << std::endl;
			//}


			interior.push_back(c.getCode()); // update state in DB
		}
		else if (horizontal != EXTER && vertical != EXTER) {
			boundary.push_back(c.getCode()); // update state in DB

			// See if we have reached max depth yet
			if (c.getCode().length() < maxDepth + 1) {
				std::vector<std::string> children;
				c.children(children);

				for (std::string child : children) {
					toTest.push_back(SdogCell(child, gridRadius));
				}
			}
		}
	}
}


// Creats a list of AirSigmets as specified in the provided JSON file
//
// d - rapidjson document containing the information about the AirSigmet(s)
// out - output list of the AirSigmet(s) stored in d - treats as empty
void AirSigmet::readFromJson(const rapidjson::Document& d, std::vector<AirSigmet>& out) {

	const rapidjson::Value& airSigArray = d["response"]["data"]["AIRSIGMET"];

	for (rapidjson::SizeType i = 0; i < airSigArray.Size(); i++) {

		AirSigmet next;
		const rapidjson::Value& entry = airSigArray[i];

		next.validFrom = entry["valid_time_from"].GetString();
		next.validUntil = entry["valid_time_to"].GetString();
		next.type = stringToAirSigmetType(entry["airsigmet_type"].GetString());

		next.dirDeg = (entry.HasMember("movement_dir_degrees")) ? std::stoi(entry["movement_dir_degrees"].GetString()) : 0;
		next.speedKT = (entry.HasMember("movement_speed_kt")) ? std::stoi(entry["movement_speed_kt"].GetString()) : 0;

		if (entry.HasMember("hazard")) {

			std::string type = (entry["hazard"].HasMember("_type")) ? entry["hazard"]["_type"].GetString() : "";
			std::string sev = (entry["hazard"].HasMember("_severity")) ? entry["hazard"]["_severity"].GetString() : "";

			next.hazard = stringToHazardType(type);
			next.severity = stringToSeverity(sev);
		}

		if (entry.HasMember("altitude")) {

			int minFT = (entry["altitude"].HasMember("_min_ft_msl")) ? std::stoi(entry["altitude"]["_min_ft_msl"].GetString()) : 0;
			int maxFT = (entry["altitude"].HasMember("_max_ft_msl")) ? std::stoi(entry["altitude"]["_max_ft_msl"].GetString()) : 0;

			next.minAltM = minFT * 0.3048; // feet to M
			next.maxAltM = (maxFT > 0) ? maxFT * 0.3048 : 30000.0;
		}

		const rapidjson::Value& pointArray = entry["area"]["point"];

		for (rapidjson::SizeType j = 0; j < pointArray.Size() - 1; j++) {
			next.polygon.push_back(SphCoord(std::stod(pointArray[j]["latitude"].GetString()), std::stod(pointArray[j]["longitude"].GetString()), false));
		}

		out.push_back(next);
	}
}


// Convert string to AirSigmetType - NONE if invalid
AirSigmetType AirSigmet::stringToAirSigmetType(const std::string& s) {

	if (s == "OUTLOOK") {
		return AirSigmetType::OUTLOOK;
	}
	else if (s == "AIRMET") {
		return AirSigmetType::AIRMET;
	}
	else if (s == "SIGMET") {
		return AirSigmetType::SIGMET;
	}
	else {
		return AirSigmetType::NONE;
	}
}


// Convert string to HazardType - NONE if invalid
HazardType AirSigmet::stringToHazardType(const std::string& s) {

	if (s == "MTN") {
		return HazardType::MTN;
	}
	else if (s == "OBSCN") {
		return HazardType::OBSCN;
	}
	else if (s == "IFR") {
		return HazardType::IFR;
	}
	else if (s == "TURB") {
		return HazardType::TURB;
	}
	else if (s == "ICE") {
		return HazardType::ICE;
	}
	else if (s == "CONVECTIVE") {
		return HazardType::CONVECTIVE;
	}
	else if (s == "ASH") {
		return HazardType::ASH;
	}
	else {
		return HazardType::NONE;
	}
}


// Convert string to Severity - NONE if invalid
Severity AirSigmet::stringToSeverity(const std::string& s) {

	if (s == "LT-MOD") {
		return Severity::LT_MOD;
	}
	else if (s == "MOD") {
		return Severity::MOD;
	}
	else if (s == "MOD-SEV") {
		return Severity::MOD_SEV;
	}
	else if (s == "SEV") {
		return Severity::SEV;
	}
	else {
		return Severity::NONE;
	}
}
#include "AirSigmet.h"

#include "SdogCell.h"

void AirSigmet::gridInsertion(double gridRadius, int maxDepth, std::vector<std::string>& interior, std::vector<std::string>& boundary) {

	enum {
		NONE = -1,
		INTER = 0,
		EXTER = 1,
		BOUND = 2
	};

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

		int horizontal = NONE;
		int vertical = NONE;

		// Horizontal test
		// Test cells against all arcs of boudnary
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

			// Polygon inside of cell, treat as boundary case (need to subdivide)
			if (((c.getMinLat() < tLat && tLat < c.getMaxLat()) || (c.getMaxLat() < tLat && tLat < c.getMinLat())) &&
				((c.getMinLong() < tLong && tLong < c.getMaxLong()) || (c.getMaxLong() < tLong && tLong < c.getMinLong()))) {
				horizontal = BOUND;
			}
			else {
				SphCoord point1(c.getMinLat(), c.getMinLong());
				SphCoord point2(c.getMaxLat(), c.getMaxLong());

				float ang1 = acos(glm::dot(point1.toCartesian(1.0), polygon[0].toCartesian(1.0)));
				float ang2 = acos(glm::dot(point2.toCartesian(1.0), polygon[0].toCartesian(1.0)));

				glm::vec3 testNorm = (ang1 < ang2) ? point1.toCartesian(1.0) : point2.toCartesian(1.0);

				// Calculate winding number of a point in the cell with respect to the polygon
				float sum = 0.f;
				for (int i = 0; i < polygon.size(); i++) {

					glm::vec3 plane1 = glm::normalize(glm::cross(testNorm, polygon[i].toCartesian(1.0)));
					glm::vec3 plane2 = glm::normalize(glm::cross(testNorm, polygon[(i + 1) % polygon.size()].toCartesian(1.0)));

					glm::vec3 cross = glm::cross(plane1, plane2);
					float dot = glm::dot(plane1, plane2);

					if (dot > 1.f) dot = 1.f;
					if (dot < -1.f) dot = -1.f;

					float angle = (glm::dot(cross, testNorm) < 0) ? -acos(dot) : acos(dot);
					sum += angle;
				}

				horizontal = (abs(sum) < 1.f) ? EXTER : INTER;
			}
		}
		// Veritcal test
		if (c.getMinRad() > minAltKM && c.getMaxRad() < maxAltKM) {
			vertical = INTER;
		}
		else if (c.getMaxRad() < minAltKM || c.getMinRad() > maxAltKM) {
			vertical = EXTER;
		}
		else {
			vertical = BOUND;
		}

		// If cell is interior or boundary act accordingly - do nothing with exterior cells
		if (horizontal == INTER && vertical == INTER) {
			interior.push_back(c.getCode()); // update state in DB
		}
		else if (horizontal != EXTER && vertical != EXTER) {
			boundary.push_back(c.getCode()); // update state in DB

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
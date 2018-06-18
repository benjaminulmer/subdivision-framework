


#define _USE_MATH_DEFINES
#include "Geometry.h"

#include <cmath>
#include <list>

#include <cstdlib>
#include <iostream>



// Geometric slerp between two vectors
glm::vec3 Geometry::geomSlerp(const glm::vec3& v1, const glm::vec3& v2, float t) {
	float theta = acos(glm::dot(glm::normalize(v1), glm::normalize(v2)));

	glm::vec3 term1 = (sin((1.f - t) * theta) / sin(theta)) * v1;
	glm::vec3 term2 = (sin(t * theta) / sin(theta)) * v2;
	return term1 + term2;
}

// Creates a renderable for an arc defined by p1, p2, and centre points
void Geometry::createArcR(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& centre, Renderable& r) {
	glm::vec3 v1 = p1 - centre;
	glm::vec3 v2 = p2 - centre;

	// Points on centre, cannot create arc (and will crash)
	if (glm::length(v1) < 0.0001 || glm::length(v2) < 0.0001) {
		return;
	}

	// Compute angle between vectors
	float theta = acos(glm::dot(glm::normalize(v1), glm::normalize(v2)));
	int angleDeg = (int) (theta * 180.f / M_PI);

	// Points are very close on arc, just draw line between them
	if (angleDeg == 0) {
		r.verts.push_back(p1);
		r.verts.push_back(p2);
		r.colours.push_back(r.renderColour);
		r.colours.push_back(r.renderColour);
	}

	else {
		// #num line segments ~= angle of arc in degrees / 6
		angleDeg /= 6;
		angleDeg++;
		for (int i = 0; i <= angleDeg; i++) {
			float t = (float)i / angleDeg;

			glm::vec3 result = geomSlerp(v1, v2, t);

			r.verts.push_back(centre + result);
			r.colours.push_back(r.renderColour);
			if (i != 0 && i != angleDeg) {
				r.verts.push_back(centre + result);
				r.colours.push_back(r.renderColour);
			}
		}
	}
}

void Geometry::createWallR(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& maxp1, const glm::vec3& maxp2, const glm::vec3& centre, Renderable& r) {
	glm::vec3 v1 = p1 - centre;
	glm::vec3 v2 = p2 - centre;

	glm::vec3 maxv1 = maxp1 - centre;
	glm::vec3 maxv2 = maxp2 - centre;

	// Points on centre, cannot create arc (and will crash)
	if (glm::length(v1) < 0.0001 || glm::length(v2) < 0.0001) {
		return;
	}

	// Compute angle between vectors
	float theta = acos(glm::dot(glm::normalize(v1), glm::normalize(v2)));
	int angleDeg = (int)(theta * 180.f / M_PI);

	// Points are very close on arc, just draw line between them
	if (angleDeg == 0) {
		r.verts.push_back(p1);
		r.verts.push_back(p2);
		r.verts.push_back(maxp2);

		r.verts.push_back(p1);
		r.verts.push_back(maxp2);
		r.verts.push_back(maxp1);

		r.colours.push_back(r.renderColour);
		r.colours.push_back(r.renderColour);
		r.colours.push_back(r.renderColour);
		r.colours.push_back(r.renderColour);
		r.colours.push_back(r.renderColour);
		r.colours.push_back(r.renderColour);
	}

	else {
		// #num line segments ~= angle of arc in degrees / 6
		angleDeg /= 6;
		angleDeg++;

		glm::vec3 a = v1;
		glm::vec3 maxa = maxv1;

		for (int i = 0; i <= angleDeg; i++) {
			float t = (float)i / angleDeg;

			glm::vec3 result = geomSlerp(v1, v2, t);
			glm::vec3 mResult = geomSlerp(maxv1, maxv2, t);

			r.verts.push_back(a);
			r.verts.push_back(result);
			r.verts.push_back(mResult);

			r.verts.push_back(a);
			r.verts.push_back(mResult);
			r.verts.push_back(maxa);

			a = result;
			maxa = mResult;

			for (int j = 0; j < 6; j++) {
				r.colours.push_back(r.renderColour);
			}

			/*r.verts.push_back(centre + result);
			r.colours.push_back(r.renderColour);
			if (i != 0 && i != angleDeg) {
				r.verts.push_back(centre + result);
				r.colours.push_back(r.renderColour);
			}*/
		}
	}
	///////
	/*
	r.verts.push_back(p1);
	r.verts.push_back(p2);
	r.verts.push_back(maxp2);

	r.verts.push_back(p1);
	r.verts.push_back(maxp2);
	r.verts.push_back(maxp1);

	r.colours.push_back(r.renderColour);
	r.colours.push_back(r.renderColour);
	r.colours.push_back(r.renderColour);
	r.colours.push_back(r.renderColour);
	r.colours.push_back(r.renderColour);
	r.colours.push_back(r.renderColour);
	*/
}

// Creates a renderable for a line defined by p1 and p2
void Geometry::createLineR(const glm::vec3& p1, const glm::vec3& p2, Renderable & r) {
	r.verts.push_back(p1);
	r.verts.push_back(p2);
	r.colours.push_back(r.renderColour);
	r.colours.push_back(r.renderColour);
}

std::vector<glm::vec2> Geometry::triangulatePolygon(const std::vector<glm::vec2> coords) {

	std::vector<p2t::Triangle*> triangles;
	std::list<p2t::Triangle*> map;

	std::vector<std::vector<p2t::Point*>> polylines;
	std::vector<p2t::Point*> polyline;

	double prevX = -999999.0;
	double prevY = -999999.0;

	for (glm::vec2 p : coords) {
		if ((p.x != prevX) || (p.y != prevY)) {
			polyline.push_back(new p2t::Point(p.x, p.y));
			//std::cout << p.x << "  " << p.y << std::endl;

			prevX = p.x;
			prevY = p.y;
		}
	}

	std::cout << "DONE CREATING POLYLINE of size " << polyline.size() << std::endl;

	polylines.push_back(polyline);

	p2t::CDT* cdt = new p2t::CDT(polyline);
	std::cout << "triangulating..." << std::endl;
	cdt->Triangulate();
	std::cout << "finished triangulating" << std::endl;

	triangles = cdt->GetTriangles();
	map = cdt->GetMap();

	std::vector<glm::vec2> newCoords;

	//std::cout << "Created " << triangles.size() << " triangles" << std::endl;

	for (p2t::Triangle* tri : triangles) {
		glm::vec2 p;
		for (int i = 0; i < 3; i++) {
			p.x = tri->GetPoint(i)->x;
			p.y = tri->GetPoint(i)->y;
			newCoords.push_back(p);

			std::cout << "point " << p.x << "  " << p.y << std::endl;
		}
	}
	/*
	// An attempt at refinement
	polyline.clear();

	prevX = -999999.0;
	prevY = -999999.0;

	for (glm::vec2 p : newCoords) {
		if ((p.x != prevX) || (p.y != prevY)) {
			polyline.push_back(new p2t::Point(p.x, p.y));
			//std::cout << p.x << "  " << p.y << std::endl;

			prevX = p.x;
			prevY = p.y;
		}
	}
	cdt = new p2t::CDT(polyline);
	cdt->Triangulate();
	triangles = cdt->GetTriangles();
	map = cdt->GetMap();

	newCoords.clear();

	for (p2t::Triangle* tri : triangles) {
		glm::vec2 p;
		for (int i = 0; i < 3; i++) {
			p.x = tri->GetPoint(i)->x;
			p.y = tri->GetPoint(i)->y;
			newCoords.push_back(p);
		}
	}
/*
	std::cout << "Number of triangles = " << triangles.size() << std::endl;*/

	return newCoords;
}

std::vector<glm::vec2> Geometry::refinePolygon(std::vector<glm::vec2> in) {
	std::vector<glm::vec2> temp;

	double prevX = -999999.0;
	double prevY = -999999.0;

	for (glm::vec2 p : in) {
		if ((p.x != prevX) || (p.y != prevY)) {
			temp.push_back(p);
			std::cout <<"removed" <<std::endl;

			prevX = p.x;
			prevY = p.y;
		}
	}
	in = temp;

	std::vector<glm::vec2> out;

	for (int i = 0; i < in.size() - 1; i++) {
		// Compute angle between vectors
		float length = (in[i] - in[i + 1]).length();

		// Points are very close on arc, just draw line between them
		if (length <= 1) {
			out.push_back(in[i]);
			out.push_back(in[i+1]);


			std::cout << "regular: " << std::endl;
			std::cout << in[i].x << " " << in[i].y << std::endl;
			std::cout << in[i+1].x << " " << in[i+1].y << std::endl;
			//r.colours.push_back(r.renderColour);
		}

		else {
			// #num line segments ~= angle of arc in degrees / 6

			for (int i = 0; i < 6; i++) {
				float t = (float)i / 6.f;

				if (t == 0.f)
					out.push_back(in[i]);

				else {
					glm::vec2 result = (in[i] * (1.f / t)) + ((t)* in[i + 1]);

					std::cout << "interpolated at " << t << ": " << std::endl;
					std::cout << result.x << " " << result.y << std::endl;

					out.push_back(result);
				}

				/*r.colours.push_back(r.renderColour);
				if (i != 0 && i != angleDeg) {
					r.verts.push_back(centre + result);
					r.colours.push_back(r.renderColour);
				}
				*/
			}
		}
	}
	
	

	return out;
}

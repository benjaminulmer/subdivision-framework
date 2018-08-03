#include "Thompson.h"


Thompson::Thompson(int numPoints, int numIterations) : numPoints(numPoints), numIterations(numIterations) {
	/*for (int i = 0; i < numPoints; i++) {
		glm::vec3 p = glm::vec3(2 * frand(), 2 * frand(), 2 * frand());
		p = glm::normalize(p);
		points.push_back(p);
	}*/
	double C0 = 0.3249196962329063261558714122151;
	double C1 = 0.525731112119133606025669084848;
	double C2 = 0.541166900871121200823328817258;
	double C3 = 0.850650808352039932181540497063;
	double C4 = 0.875626439195919181581615733217;
	double C5 = 1.05146222423826721205133816970;

points.push_back(glm::vec3(0.0, 0.0,  C5));
points.push_back(glm::vec3(0.0, 0.0, -C5));
points.push_back(glm::vec3( C5, 0.0, 0.0));
points.push_back(glm::vec3(-C5, 0.0, 0.0));
points.push_back(glm::vec3(0.0,  C5, 0.0));
points.push_back(glm::vec3(0.0, -C5, 0.0));
points.push_back(glm::vec3( C2, 0.0,  C4));
points.push_back(glm::vec3( C2, 0.0, -C4));
points.push_back(glm::vec3(-C2, 0.0,  C4));
points.push_back(glm::vec3(-C2, 0.0, -C4));
points.push_back(glm::vec3( C4,  C2, 0.0));
points.push_back(glm::vec3( C4, -C2, 0.0));
points.push_back(glm::vec3(-C4,  C2, 0.0));
points.push_back(glm::vec3(-C4, -C2, 0.0));
points.push_back(glm::vec3(0.0,  C4,  C2));
points.push_back(glm::vec3(0.0,  C4, -C2));
points.push_back(glm::vec3(0.0, -C4,  C2));
points.push_back(glm::vec3(0.0, -C4, -C2));
points.push_back(glm::vec3( C0,  C1,  C3));
points.push_back(glm::vec3( C0,  C1, -C3));
points.push_back(glm::vec3( C0, -C1,  C3));
points.push_back(glm::vec3( C0, -C1, -C3));
points.push_back(glm::vec3(-C0,  C1,  C3));
points.push_back(glm::vec3(-C0,  C1, -C3));
points.push_back(glm::vec3(-C0, -C1,  C3));
points.push_back(glm::vec3(-C0, -C1, -C3));
points.push_back(glm::vec3( C3,  C0,  C1));
points.push_back(glm::vec3( C3,  C0, -C1));
points.push_back(glm::vec3( C3, -C0,  C1));
points.push_back(glm::vec3( C3, -C0, -C1));
points.push_back(glm::vec3(-C3,  C0,  C1));
points.push_back(glm::vec3(-C3,  C0, -C1));
points.push_back(glm::vec3(-C3, -C0,  C1));
points.push_back(glm::vec3(-C3, -C0, -C1));
points.push_back(glm::vec3( C1,  C3,  C0));
points.push_back(glm::vec3( C1,  C3, -C0));
points.push_back(glm::vec3( C1, -C3,  C0));
points.push_back(glm::vec3( C1, -C3, -C0));
points.push_back(glm::vec3(-C1,  C3,  C0));
points.push_back(glm::vec3(-C1,  C3, -C0));
points.push_back(glm::vec3(-C1, -C3,  C0));
points.push_back(glm::vec3(-C1, -C3, -C0));

}


Thompson::~Thompson() {

}

void Thompson::fillRenderable(Renderable & r) {
	for (int i = 0; i < points.size(); i++) {
		r.verts.push_back(points[i]);
		r.normals.push_back(glm::vec3(0.f, 1.f, 0.f));
	}
	r.drawMode = GL_POINTS;
	/*double minDistance = 0.45;
	for (int i = 0; i < points.size(); i++) {
		for (int j = i + 1; j < points.size(); j++) {
			if (glm::length(points[i] - points[j]) < minDistance) {
				r.verts.push_back(points[i]);
				r.normals.push_back(glm::vec3(0.f, 1.f, 0.f));
				r.verts.push_back(points[j]);
				r.normals.push_back(glm::vec3(0.f, 1.f, 0.f));
			}
		}
	}
	r.drawMode = GL_LINES;*/
}

void Thompson::outputCSV() {
	std::ofstream myfile;
	myfile.open("vertices.csv");
	for (int i = 0; i < points.size(); i++) {
		myfile << std::to_string(points[i].x) + "," + std::to_string(points[i].y) + "," + std::to_string(points[i].z) + "\n";
	}
	myfile.close();
}

double Thompson::getCoulombEnergy(std::vector<glm::vec3> pts) {
	double e = 0.0;
	for (int i = 0; i < pts.size(); i++) {
		for (int j = i + 1; j < pts.size(); j++) {
			e += 1 / glm::length(pts[i] - pts[j]);
		}
	}
	return e;
}

void Thompson::getForces() {
	forces.clear();
	for (int i = 0; i < points.size(); i++) {
		forces.push_back(glm::vec3(0.0, 0.0, 0.0));
	}
	for (int i = 0; i < points.size(); i++) {
		for (int j = i + 1; j < points.size(); j++) {
			glm::vec3 r = points[i] - points[j];
			double l = glm::length(r);
			l = 1 / (l * l * l);
			forces[i].x += l * r.x;
			forces[i].y += l * r.y;
			forces[i].z += l * r.z;
			forces[j].x -= l * r.x;
			forces[j].y -= l * r.y;
			forces[j].z -= l * r.z;
		}
	}
}

void Thompson::iterate() {
	double e0 = getCoulombEnergy(points);
	std::vector<glm::vec3> newPoints;
	for (int i = 0; i < points.size(); i++) {
		newPoints.push_back(glm::vec3(0.0, 0.0, 0.0));
	}
	for (int i = 0; i < numIterations; i++) {
		printf("%.100f\n", step);
		getForces();
		for (int j = 0; j < points.size(); j++) {
			double d = glm::dot(forces[j], points[j]);
			forces[j] -= points[j] * (float)d;
			newPoints [j] = points[j] + forces[j] * (float)step;
			newPoints[j] = glm::normalize(newPoints[j]);
		}
		double e = getCoulombEnergy(newPoints);
		//unsuccessful iteration
		if (e >= e0) {
			step /= 2.0;
			if (step < minimumStep) {
				break;
			}
		} else {
			for (int k = 0; k < points.size(); k++) {
				points[k] = newPoints[k];
			}
			e0 = e;
			step *= 2;
		}
	}
}

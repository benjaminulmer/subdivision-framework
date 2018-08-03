#include "Renderable.h"

Renderable::Renderable() : vao(0), vertexBuffer(0), normalBuffer(0), drawMode(0) {

}

Renderable::~Renderable() {
	
}

void Renderable::outputFaceAreas() {
	std::ofstream myfile;
	myfile.open("areas.csv");
	for (int i = 0; i < verts.size(); i += 3) {
		double alpha = std::asin(glm::length(glm::cross(glm::cross(verts[i], verts[i + 1]), glm::cross(verts[i + 1], verts[i + 2]) ) / (glm::length(glm::cross(verts[i], verts[i + 1])) * glm::length(glm::cross(verts[i + 1], verts[i + 2])))) );
		double beta = std::asin(glm::length(glm::cross(glm::cross(verts[i], verts[i + 1]), glm::cross(verts[i], verts[i + 2])) / (glm::length(glm::cross(verts[i], verts[i + 1])) * glm::length(glm::cross(verts[i], verts[i + 2])))));
		double gamma = std::asin(glm::length(glm::cross(glm::cross(verts[i], verts[i + 2]), glm::cross(verts[i + 1], verts[i + 2])) / (glm::length(glm::cross(verts[i], verts[i + 2])) * glm::length(glm::cross(verts[i + 1], verts[i + 2])))));
		//double area = std::abs(glm::length(glm::cross(verts[i] - verts[i+1], verts[i] - verts[i+2])) / 2.0 );
		double area = alpha + beta + gamma - M_PI;
		myfile << std::to_string(area) + "\n";
	}
	myfile.close();
}


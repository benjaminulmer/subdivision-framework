#pragma once

#include <glm/glm.hpp>

#define _USE_MATH_DEFINES
#include <math.h>

class Camera {

public:
	Camera();

	glm::mat4 getLookAt() const;
	glm::vec3 getPosition() const;
	glm::vec3 getUp() const;
	glm::vec3 getLookDir() const;

	void updateLongitudeRotation(float deg);
	void updateLatitudeRotation(float deg);
	void updateZoom(int value);
	void translate(const glm::vec3& planeTranslation);

	void reset();

private:
	const float zoomScale;
	const float rotScale;

	glm::vec3 eye;
	glm::vec3 up;
	glm::vec3 centre;

	float longitudeRotRad;
	float latitudeRotRad;

	glm::vec3 translation;
};

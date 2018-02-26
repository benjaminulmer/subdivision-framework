#pragma once

#include <glm/glm.hpp>

#define _USE_MATH_DEFINES
#include <math.h>

#include "SphericalCell.h"

class Camera {

public:
	Camera();

	glm::mat4 getLookAt() const;
	glm::vec3 getPosition() const;
	glm::vec3 getUp() const;
	glm::vec3 getLookDir() const;

	void setScale(float scale);

	void updateLongitudeRotation(float rad);
	void updateLatitudeRotation(float rad);
	void updateZoom(int value);
	void translate(const glm::vec3& planeTranslation);

	void reset();

private:
	const float zoomScale;
	const float rotScale;

	float curScale;

	glm::vec3 eye;
	glm::vec3 up;
	glm::vec3 centre;

	float longitudeRotRad;
	float latitudeRotRad;

	glm::vec3 translation;
};

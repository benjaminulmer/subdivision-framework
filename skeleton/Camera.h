#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

#define _USE_MATH_DEFINES
#include <math.h>

class Camera {

public:
	Camera();

	glm::mat4 getLookAt();
	glm::vec3 getPosition();
	void updateLongitudeRotation(float deg);
	void updateLatitudeRotation(float deg);
	void updateZoom(int value);
	void translate(glm::vec3 planeTranslation);

	void reset();

private:
	glm::vec3 eye;
	glm::vec3 up;
	glm::vec3 centre;

	float longitudeRotRad;
	float latitudeRotRad;

	glm::vec3 translation;
};

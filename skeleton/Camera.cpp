#include "Camera.h"

Camera::Camera() {
	eye = glm::vec3(0.0f, 0.0f, 10.0f);
	up = glm::vec3(0.0f, 1.0f, 0.0f);
	centre = glm::vec3(0.0f, 0.0f, 0.0f);

	longitudeRotRad = 0;
	latitudeRotRad = 0;
}

// Returns view matrix for the camera
glm::mat4 Camera::getLookAt() {

	// Rotate eye along longitude
	glm::vec3 eyeTemp = glm::rotateY(eye, -longitudeRotRad);

	// Find axis to rotate eye and up along latitude
	glm::vec3 axis = glm::cross(eyeTemp, glm::vec3(0.0, 1.0, 0.0));

	eyeTemp = glm::rotate(eyeTemp, latitudeRotRad, axis);
	glm::vec3 upTemp = glm::rotate(up, latitudeRotRad, axis);

	return glm::lookAt(eyeTemp, centre, upTemp);
}

// Returns position of the camera
glm::vec3 Camera::getPosition() {
	glm::vec3 eyeTemp = glm::rotateY(eye, -longitudeRotRad);
	eyeTemp = glm::rotate(eyeTemp, latitudeRotRad, glm::cross(eyeTemp, glm::vec3(0.0, 1.0, 0.0)));

	return eyeTemp;
}

// Rotates camera along longitudinal axis (spherical coords)
void Camera::updateLongitudeRotation(float deg) {
	
	// If camera is upside down reverse longitude rotations
	if (cos(latitudeRotRad) > 0) {
		longitudeRotRad += deg * M_PI / 180;
	}
	else {
		longitudeRotRad -= deg * M_PI / 180;
	}
}

// Rotates camera along latitudinal axis (spherical coords)
void Camera::updateLatitudeRotation(float deg) {
	latitudeRotRad += deg * M_PI/180;
}

// Zooms camera in or out (+1 or -1)
void Camera::updateZoom(float value) {
	float zoom = (eye.z / 10.f) * value;
	eye.z += zoom;
}

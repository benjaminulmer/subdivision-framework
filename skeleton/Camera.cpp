#include "Camera.h"

Camera::Camera() {
	eye = glm::vec3(0.f, 0.f, 10.f);
	up = glm::vec3(0.f, 1.f, 0.f);
	centre = glm::vec3(0.f, 0.f, 0.f);

	longitudeRotRad = 0;
	latitudeRotRad = 0;

	translation = glm::vec3(0.f, 0.f, 0.f);
}

// Returns view matrix for the camera
glm::mat4 Camera::getLookAt() {

	// Rotate eye along longitude
	glm::vec3 eyeTemp = glm::rotateY(eye, -longitudeRotRad);

	// Find axis then rotate eye and up along latitude
	glm::vec3 axis = glm::cross(eyeTemp, glm::vec3(0.f, 1.f, 0.f));

	eyeTemp = glm::rotate(eyeTemp, latitudeRotRad, axis);
	glm::vec3 upTemp = glm::rotate(up, latitudeRotRad, axis);

	return glm::lookAt(eyeTemp + translation, centre + translation, upTemp);
}

// Returns position of the camera
glm::vec3 Camera::getPosition() {
	glm::vec3 eyeTemp = glm::rotateY(eye, -longitudeRotRad);
	eyeTemp = glm::rotate(eyeTemp, latitudeRotRad, glm::cross(eyeTemp, glm::vec3(0.f, 1.f, 0.f)));

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

// Translates camera along x and y of view plane
void Camera::translate(glm::vec3 planeTranslation) {

	// Scale translation based on zoom level
	float scale = (eye.z / 500.f);
	planeTranslation *= scale;

	// Get rotation axis
	glm::vec3 eyeTemp = glm::rotateY(eye, -longitudeRotRad);
	glm::vec3 axis = glm::cross(eyeTemp, glm::vec3(0.f, 1.f, 0.f));

	// Convert screen space translation into world space translation
	glm::vec3 rotatedTranslation = glm::rotateY(planeTranslation, -longitudeRotRad);
	rotatedTranslation = glm::rotate(rotatedTranslation, latitudeRotRad, axis);

	translation += rotatedTranslation;
}

// Reset camera to starting position
void Camera::reset() {
	eye = glm::vec3(0.f, 0.f, 10.f);
	up = glm::vec3(0.f, 1.f, 0.f);
	centre = glm::vec3(0.f, 0.f, 0.f);

	longitudeRotRad = 0;
	latitudeRotRad = 0;

	translation = glm::vec3(0.f, 0.f, 0.f);
}
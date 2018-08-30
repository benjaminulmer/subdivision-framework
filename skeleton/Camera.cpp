#include "Camera.h"

Camera::Camera() {
	reset();
}
#include <iostream>
// Returns view matrix for the camera
glm::mat4 Camera::getLookAt() {

	std::cout << "Scale: " << "ee" << "\tLong: " << longitudeRotRad << "\tLat: " << latitudeRotRad << "\tTrans: " << translation.x << ", " << translation.y << ", " << translation.z << std::endl;

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

	return eyeTemp + translation;
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
	latitudeRotRad += deg * M_PI / 180;
}

// Zooms camera in or out (+1 or -1)
void Camera::updateZoom(int value) {
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

	longitudeRotRad = 0.0;
	latitudeRotRad = 0.0;
	translation = glm::dvec3(0.0);

	longitudeRotRad = 2.41728;
	latitudeRotRad = 0.575959;
	translation = glm::dvec3(-0.721749, 0.747265, 0.672448);

	longitudeRotRad = 9.40731;
	latitudeRotRad = -0.00872728;
	translation = glm::dvec3(-1.33246, 1.3438, 0.774795);

	longitudeRotRad = 0.785398;
	latitudeRotRad = 0.785398;
	translation = glm::dvec3(0.0);
}
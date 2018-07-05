#define _USE_MATH_DEFINES
#include "Camera.h"

#include <glm/gtx/rotate_vector.hpp>

#include <cmath>

#include "Constants.h"

#include <iostream>
Camera::Camera() : zoomScale(1.3f), rotScale(0.008f) {
	reset();
}

// Returns view matrix for the camera
glm::mat4 Camera::getLookAt() const {

	// Rotate eye along longitude
	glm::vec3 eyeTemp = glm::rotateY(eye, -longitudeRotRad);

	// Find axis then rotate eye and up along latitude
	glm::vec3 axis = glm::cross(eyeTemp, glm::vec3(0.f, 1.f, 0.f));

	eyeTemp = glm::rotate(eyeTemp, latitudeRotRad, axis);
	glm::vec3 upTemp = glm::rotate(up, latitudeRotRad, axis);

	return glm::lookAt(eyeTemp + translation, centre + translation, upTemp);
}

// Returns position of the camera
glm::vec3 Camera::getPosition() const {

	glm::vec3 eyeTemp = glm::rotateY(eye, -longitudeRotRad);
	eyeTemp = glm::rotate(eyeTemp, latitudeRotRad, glm::cross(eyeTemp, glm::vec3(0.f, 1.f, 0.f)));

	//std::cout << "Camera pos: " << (eyeTemp + translation).x << " " << (eyeTemp + translation).y << " " << (eyeTemp + translation).z << std::endl;

	return eyeTemp + translation;
}

// Returns up of the camera
glm::vec3 Camera::getUp() const {

	// Rotate eye along longitude
	glm::vec3 eyeTemp = glm::rotateY(eye, -longitudeRotRad);

	// Find axis then rotate eye and up along latitude
	glm::vec3 axis = glm::cross(eyeTemp, glm::vec3(0.f, 1.f, 0.f));

	eyeTemp = glm::rotate(eyeTemp, latitudeRotRad, axis);
	return glm::rotate(up, latitudeRotRad, axis);
}

// Returns looking direction of camera
glm::vec3 Camera::getLookDir() const {
	return glm::normalize(centre - getPosition());
}

// Sets current model scale
void Camera::setScale(float scale) {
	curScale = scale;

	eye = glm::vec3(0.f, 0.f, RADIUS_EARTH_VIEW * scale + 30.f);
	centre = glm::vec3(0.f, 0.f, RADIUS_EARTH_VIEW * scale);
	latitudeRotRad = 0.f;
}

// Rotates camera along longitudinal axis (spherical coords)
void Camera::updateLongitudeRotation(float rad) {
	longitudeRotRad += rad;
}

// Rotates camera along latitudinal axis (spherical coords)
void Camera::updateLatitudeRotation(float rad) {
	latitudeRotRad -= rad;
}

// Zooms camera in or out (+1 or -1)
void Camera::updateZoom(int sign) {

	if (sign < 0) {
		eye.z = (eye.z - RADIUS_EARTH_VIEW) / zoomScale + RADIUS_EARTH_VIEW;
	}
	else {
		eye.z = (eye.z - RADIUS_EARTH_VIEW) * zoomScale + RADIUS_EARTH_VIEW;;
	}
	if (eye.z > 4.f * RADIUS_EARTH_VIEW) eye.z = 4.f * RADIUS_EARTH_VIEW;
}

// Translates camera along x and y of view plane
void Camera::translate(const glm::vec3& planeTranslation) {

	glm::vec3 pTrans(planeTranslation);

	// Scale translation based on zoom level
	float scale = (eye.z / 500.f);
	pTrans *= scale;

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
	eye = glm::vec3(0.f, 0.f, RADIUS_EARTH_VIEW + 30.f);
	up = glm::vec3(0.f, 1.f, 0.f);
	centre = glm::vec3(0.f, 0.f, RADIUS_EARTH_VIEW);

	longitudeRotRad = 0;
	latitudeRotRad = 0;

	translation = glm::vec3(0.f, 0.f, 0.f);
}
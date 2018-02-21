#include "Camera.h"

#include "Constants.h"

Camera::Camera() : zoomScale(1.3f), rotScale(0.008f) {
	reset();
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

	return eyeTemp + translation;
}

// Rotates camera along longitudinal axis (spherical coords)
void Camera::updateLongitudeRotation(float pixelsMoved) {
	
	float scale = rotScale * (eye.z - MODEL_SCALE);
	// If camera is upside down reverse longitude rotations
	if (cos(latitudeRotRad) > 0) {
		longitudeRotRad += scale * pixelsMoved * M_PI / 180;
	}
	else {
		longitudeRotRad -= scale * pixelsMoved * M_PI / 180;
	}
}

// Rotates camera along latitudinal axis (spherical coords)
void Camera::updateLatitudeRotation(float pixelsMoved) {
	float scale = rotScale * (eye.z - MODEL_SCALE);
	latitudeRotRad += scale * pixelsMoved * M_PI / 180;
}

// Zooms camera in or out (+1 or -1)
void Camera::updateZoom(int sign) {

	if (sign < 0) {
		eye.z = (eye.z - MODEL_SCALE) / zoomScale + MODEL_SCALE;
	}
	else {
		eye.z = (eye.z - MODEL_SCALE) * zoomScale + MODEL_SCALE;;
	}
	if (eye.z > 4.f * MODEL_SCALE) eye.z = 4.f * MODEL_SCALE;
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
	eye = glm::vec3(0.f, 0.f, 4.f * MODEL_SCALE);
	up = glm::vec3(0.f, 1.f, 0.f);
	centre = glm::vec3(0.f, 0.f, 0.f);

	longitudeRotRad = 0;
	latitudeRotRad = 0;

	translation = glm::vec3(0.f, 0.f, 0.f);
}
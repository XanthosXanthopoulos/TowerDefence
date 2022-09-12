#include "Camera.h"

#include "glm/gtc/matrix_transform.hpp"

Camera::Camera() : position(0.0, 1.0, 1.0), target(0.0), up(0.0, 1.0, 0.0), moveX(0), moveY(0), lookAngleDestination(0) { }

Camera::Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up) : position(position), target(target), up(up), moveX(0), moveY(0), lookAngleDestination(0) { }

Camera::~Camera() { }

void Camera::Update(float delta)
{
	glm::vec3 direction = glm::normalize(target - position);
	position += moveX *  movementSpeed * direction * delta;
	target += moveY * movementSpeed * direction * delta;

	glm::vec3 right = glm::normalize(glm::cross(direction, up));
	position += moveY *  movementSpeed * right * delta;
	target += moveY * movementSpeed * right * delta;

	glm::mat4 rotation = glm::mat4(1.0f);

	rotation *= glm::rotate(glm::mat4(1.0), lookAngleDestination.y * angularSpeed, right);
	rotation *= glm::rotate(glm::mat4(1.0), -lookAngleDestination.x  * angularSpeed, up);
	lookAngleDestination = glm::vec2(0);

	direction = rotation * glm::vec4(direction, 0);
	float distance = glm::distance(position, target);
	target = position + direction * distance;

	viewMatrix = glm::lookAt(position, target, up);
}

glm::mat4 Camera::GetViewMatrix()
{
	return viewMatrix;
}

glm::vec3 Camera::GetPosition()
{
	return position;
}

glm::vec3 Camera::GetTarget()
{
	return target;
}

glm::mat4 Camera::GetReflectedCameraViewMatrix()
{
	glm::vec3 reflectedPosition;
	glm::vec3 reflectedTarget;

	reflectedPosition.x = position.x;
	reflectedPosition.y = -position.y;
	reflectedPosition.z = position.z;

	reflectedTarget.x = target.x;
	reflectedTarget.y = -target.y;
	reflectedTarget.z = target.z;

	return glm::lookAt(reflectedPosition, reflectedTarget, up);
}

void Camera::MoveForward(bool active)
{
	moveX = active ? 1 : 0;
}

void Camera::MoveBackward(bool active)
{
	moveX = active ? -1 : 0;
}

void Camera::MoveLeft(bool active)
{
	moveY = active ? -1 : 0;
}

void Camera::MoveRight(bool active)
{
	moveY = active ? 1 : 0;
}

void Camera::Look(glm::vec2 direction)
{
	lookAngleDestination = glm::vec2(1, -1) * direction;
}

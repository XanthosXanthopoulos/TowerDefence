#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"

class Camera
{
public:
	/// <summary>
	/// Default constructor.
	/// </summary>
	Camera();

	/// <summary>
	/// Constructo a camera with the given position, targe and up vector.
	/// </summary>
	/// <param name='position'> The position of the camera. </param>
	/// <param name='target'> The camera target. </param>
	/// <param name='up'> The camera up vector. </param>
	Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up);

	/// <summary>
	/// Virtual destructor.
	/// </summary>
	virtual ~Camera();

	/// <summary>
	/// Update the camera position and orientation.
	/// </summary>
	/// <param name='delta'> The time passed since the last draw call. </param>
	void Update(float delta);

	/// <summary>
	/// Get the camera view matrix.
	/// </summary>
	glm::mat4 GetViewMatrix();

	/// <summary>
	/// Get the camera position.
	/// </summary>
	glm::vec3 GetPosition();

	glm::vec3 GetTarget();

	/// <summary>
	/// Get the XZ-plane reflection camera view matrix.
	/// </summary>
	glm::mat4 GetReflectedCameraViewMatrix();

	/// <summary>
	/// Move camera forward.
	/// </summary>
	/// <param name='active'> Indicate whether the camera should move. </param>
	void MoveForward(bool active);

	/// <summary>
	/// Move camera backward.
	/// </summary>
	/// <param name='active'> Indicate whether the camera should move. </param>
	void MoveBackward(bool active);

	/// <summary>
	/// Move camera left.
	/// </summary>
	/// <param name='active'> Indicate whether the camera should move. </param>
	void MoveLeft(bool active);

	/// <summary>
	/// Move camera right.
	/// </summary>
	/// <param name='active'> Indicate whether the camera should move. </param>
	void MoveRight(bool active);

	/// <summary>
	/// Rotate the camera.
	/// </summary>
	/// <param name='direction'> Indicate the rotation direction of the camera. </param>
	void Look(glm::vec2 direction);

private:

	glm::vec3 position;
	glm::vec3 target;
	glm::vec3 up;

	float moveX;
	float moveY;
	glm::vec2 lookAngleDestination;

	glm::mat4 viewMatrix;

	const float movementSpeed = 3.0;
	const float angularSpeed = glm::pi<float>() * 0.0025;
};

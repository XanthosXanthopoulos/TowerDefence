#pragma once

#include <glm/glm.hpp>
#include <string>

#include "ShaderProgram.h"
#include "GLEW\glew.h"


class Light
{
public:
	/// <summary>
	/// Default constructor.
	/// </summary>
	Light();

	/// <summary>
	/// Construct a light with the given color and intensity
	/// </summary>
	/// <param name='color'> The color of the light. </param>
	/// <param name='intensity'> The intensity of the light </param>
	Light(glm::vec3 color, float intensity);

	/// <summary>
	/// Virtual destructor.
	/// </summary>
	virtual ~Light();

	/// <summary>
	/// Pass the light parameters to the shader program.
	/// </summary>
	virtual void SetupLight(ShaderProgram& shader) = 0;

	/// <summary>
	/// Set the shadow status and set up required buffers.
	/// </summary>
	/// <param name='enable'> The status of the shadows. </param>
	void CastShadow(bool enable);

//protected:
	std::string name;

	glm::vec3 color;
	float intensity;

	GLuint shadowMapDepthTexture;
	GLuint shadowMapFBO;

	unsigned int shadowMapResolution;
	float shadowMapBias;
	bool enableShadowMap;

	glm::mat4 projectionMatrix;
	glm::mat4 projectionInverseMatrix;
	glm::mat4 viewMatrix;
	glm::mat4 viewInverseMatrix;
};


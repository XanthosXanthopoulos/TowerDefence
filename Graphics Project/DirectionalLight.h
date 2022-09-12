#pragma once

#include "Light.h"
#include <glm/glm.hpp>
#include "ShaderProgram.h"

class DirectionalLight : public Light
{
public:
	/// <summary>
	/// Default constructor.
	/// </suumary>
	DirectionalLight();

	/// <summary>
	/// Default constructor.
	/// </suumary>
	DirectionalLight(glm::vec3 direction, glm::vec3 color, float intensity);

	/// <summary>
	/// Virtual destructor.
	/// </suumary>
	virtual ~DirectionalLight();

	/// <summary>
	/// Pass the light parameters to the shader program.
	/// </summary>
	void SetupLight(ShaderProgram& shader);

	/// <summary>
	/// Render the shadow map.
	/// </summary>
	void SetupShadowMap(ShaderProgram& shader);

protected:
	glm::vec3 direction;
};


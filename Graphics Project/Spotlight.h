#pragma once

#include "Light.h"
#include "glm/glm.hpp"
#include "GLEW\glew.h"
#include "ShaderProgram.h"

class Spotlight
{
public:
	Spotlight();
	virtual ~Spotlight();

	void SetupLight(ShaderProgram& shader);
	void CastShadow();

protected:
	float umbra;
	float penumbra;

	glm::vec3 direction;
	glm::vec3 position;
};


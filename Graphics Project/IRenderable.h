#pragma once

#include "ShaderProgram.h"

class IRenderable
{
public:

	virtual void Render(ShaderProgram& shader) const = 0;
};
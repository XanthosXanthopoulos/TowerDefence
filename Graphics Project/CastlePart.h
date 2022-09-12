#pragma once

#include "Object.h"

class CastlePart : public Object
{
public:

	CastlePart(std::shared_ptr<GeometryNode> model, glm::vec3 position, glm::vec3 rotationAxis, double angle, glm::vec3 scale);

	virtual ~CastlePart();

	virtual void Update(double delta);
};
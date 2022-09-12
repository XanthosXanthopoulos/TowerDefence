#pragma once

#include "Object.h"

class Tree : public Object
{
public:

	Tree(std::shared_ptr<GeometryNode> model, glm::vec3 position, glm::vec3 rotationAxis, double angle, glm::vec3 scale);

	virtual ~Tree();

	virtual void Update(double delta);
};
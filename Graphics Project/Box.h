#pragma once

#include "Object.h"
#include "ICollidable.h"

class Box : public Object, public ICollidable
{
public:

	Box(std::shared_ptr<GeometryNode> model, glm::vec3 position, glm::vec3 scale, glm::vec3 collisionTriggerPosition, float collisionTriggerRadius);

	virtual ~Box();

	virtual void Update(double delta);

	virtual bool Collide(const ICollidable& other);
};
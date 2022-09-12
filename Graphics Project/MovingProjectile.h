#pragma once

#include "ProjectileBase.h"
#include "ICollidable.h"

class MovingProjectile : public ProjectileBase, public ICollidable
{
public:

	static std::shared_ptr<MovingProjectile> CreateInstance(std::shared_ptr<GeometryNode> model, Tag tag, glm::vec3 position, glm::vec3 scale, glm::vec3 target, double movementSpeed, double damage, double collisionRadius);

	~MovingProjectile();

	virtual void Move(double delta);

	virtual bool Collide(const ICollidable& other);

	glm::vec3 GetPosition() const;

protected:

	MovingProjectile(std::shared_ptr<GeometryNode> model, Tag tag, glm::vec3 position, glm::vec3 scale, glm::vec3 target, double movementSpeed, double damage, double collisionRadius);

	double movementSpeed;

	glm::vec3 position;
	glm::vec3 direction;
};
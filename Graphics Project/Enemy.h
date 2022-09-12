#pragma once

#include "Object.h"
#include "ICollidable.h"

class Enemy : public Object, public ICollidable
{
public:

	static std::shared_ptr<Enemy> CreateInstance(std::shared_ptr<GeometryNode> body, std::shared_ptr<GeometryNode> rightHand, std::shared_ptr<GeometryNode> leftFoot, std::shared_ptr<GeometryNode> rightFoot, glm::vec3 position, glm::vec3 rotationAxis, double angle, glm::vec3 scale, double movementSpeed, float hitPoints, float value, float collisionRadius);
	
	virtual ~Enemy();

	virtual void Update(double delta);

	virtual bool Collide(const ICollidable& other);

	virtual void Animate(double delta);

	virtual void Move(double delta);

	void Damage(double damage);

	double GetRemainingHitPoints() const;

	float GetRemainingLifetime() const;

	float GetValue() const;

	glm::vec3 GetPosition() const;

protected:

	Enemy(std::shared_ptr<GeometryNode> body, std::shared_ptr<GeometryNode> rightHand, std::shared_ptr<GeometryNode> leftFoot, std::shared_ptr<GeometryNode> rightFoot, glm::vec3 position, glm::vec3 rotationAxis, double angle, glm::vec3 scale, double movementSpeed, float hitPoints, float value, float collisionRadius);

	glm::vec3 position;
	float angle;

	float value;
	double hitPoints;
	double remainingHitPoints;
	double movementSpeed;
	double AnimationSpeed;

	double currentAngle;
	double maxAngle;

	float lifetime = 0.6;
	float remainingLifetime = 0.6;

	std::pair<int, int> currentTile;
	std::pair<int, int> targetTile;
};
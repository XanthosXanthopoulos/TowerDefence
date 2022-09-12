#pragma once

#include "TowerBase.h"

class CannonsTower : public TowerBase
{
public:

	static std::shared_ptr<CannonsTower> CreateInstance(std::shared_ptr<GeometryNode> model, glm::vec3 position, glm::vec3 rotationAxis, double angle, glm::vec3 scale, double attackDamage, double attackRate, double attackRange, std::shared_ptr<GeometryNode> projectileModel, glm::vec3 projectilePosition);

	CannonsTower(std::shared_ptr<GeometryNode> model, glm::vec3 position, glm::vec3 rotationAxis, double angle, glm::vec3 scale, double attackDamage, double attackRate, double attackRange, std::shared_ptr<GeometryNode> projectileModel, glm::vec3 projectilePosition);

	virtual ~CannonsTower();

protected:

	virtual void Attack(double delta);

	float angle;
	float previousAngle;
	float rotationSpeed;

	std::shared_ptr<GeometryNode> projectileModel;
	glm::vec3 projectilePosition;
};
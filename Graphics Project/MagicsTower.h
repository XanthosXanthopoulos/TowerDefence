#pragma once

#include "TowerBase.h"

class MagicsTower : public TowerBase
{
public:

	static std::shared_ptr<MagicsTower> CreateInstance(std::shared_ptr<GeometryNode> model, glm::vec3 position, glm::vec3 rotationAxis, double angle, glm::vec3 scale, double attackDamage, double attackRate, double attackRange, std::shared_ptr<GeometryNode> projectileModel);

	MagicsTower(std::shared_ptr<GeometryNode> model, glm::vec3 position, glm::vec3 rotationAxis, double angle, glm::vec3 scale, double attackDamage, double attackRate, double attackRange, std::shared_ptr<GeometryNode> projectileModel);

	virtual ~MagicsTower();

protected:

	virtual void Attack(double delta);

	std::shared_ptr<GeometryNode> projectileModel;
};
#pragma once

#include "Object.h"
#include "IUpdateable.h"

class TowerBase : public Object
{
public:

	TowerBase(std::shared_ptr<GeometryNode> model, Tag tag, glm::vec3 position, glm::vec3 rotationAxis, double angle, glm::vec3 scale, double attackDamage, double attackRate, double attackRange);

	virtual ~TowerBase();

	virtual void Update(double delta);

	virtual void MoveAt(glm::vec3 position);

	glm::vec3 GetPosition();

	float GetAttackRange() const;

protected:

	virtual void Attack(double delta) = 0;

	glm::vec3 position;
	glm::vec3 scale;

	double attackDamage;
	double attackRate;
	double attackRange;
	double previousAttack;
};
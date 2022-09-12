#include "TowerBase.h"

#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/vector_angle.hpp"

TowerBase::TowerBase(std::shared_ptr<GeometryNode> model, Tag tag, glm::vec3 position, glm::vec3 rotationAxis, double angle, glm::vec3 scale, double attackDamage, double attackRate, double attackRange) : Object(tag, model), position(position), scale(scale), attackDamage(attackDamage), attackRate(attackRate), attackRange(attackRange)
{
	previousAttack = 0;

	modelTransformations.at(0).AddScale(scale);
	modelTransformations.at(0).AddRotation(rotationAxis, angle);
	modelTransformations.at(0).AddTranslation(position);
	modelTransformations.at(0).CombineTransformationMatrices();
}

TowerBase::~TowerBase() { }

void TowerBase::Update(double delta)
{
	previousAttack += delta;
	Attack(delta);
}

void TowerBase::MoveAt(glm::vec3 position)
{
	this->position = position;

	modelTransformations.at(0).UpdateTransformation(2, glm::translate(glm::mat4(1.0), position + glm::vec3(0.5, 0.0, 0.5)));
	modelTransformations.at(0).CombineTransformationMatrices();
}

glm::vec3 TowerBase::GetPosition()
{
	return position;
}

float TowerBase::GetAttackRange() const
{
	return attackRange;
}

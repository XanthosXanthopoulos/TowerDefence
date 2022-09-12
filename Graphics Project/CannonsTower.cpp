#include "CannonsTower.h"

#include "Enemy.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/vector_angle.hpp"
#include "MovingProjectile.h"

std::shared_ptr<CannonsTower> CannonsTower::CreateInstance(std::shared_ptr<GeometryNode> model, glm::vec3 position, glm::vec3 rotationAxis, double angle, glm::vec3 scale, double attackDamage, double attackRate, double attackRange, std::shared_ptr<GeometryNode> projectileModel, glm::vec3 projectilePosition)
{
	std::shared_ptr<CannonsTower> tower(new CannonsTower(model, position, rotationAxis, angle, scale, attackDamage, attackRate, attackRange, projectileModel, projectilePosition));
	//Object::gameObjects.at(Tag::CannonTower).push_back(tower);
	return tower;
}

CannonsTower::~CannonsTower()
{
}

CannonsTower::CannonsTower(std::shared_ptr<GeometryNode> model, glm::vec3 position, glm::vec3 rotationAxis, double angle, glm::vec3 scale, double attackDamage, double attackRate, double attackRange, std::shared_ptr<GeometryNode> projectileModel, glm::vec3 projectilePosition) : TowerBase(model, Tag::CannonTower, position, rotationAxis, angle, scale, attackDamage, attackRate, attackRange), projectileModel(projectileModel), projectilePosition(projectilePosition)
{
	previousAngle = angle;
	rotationSpeed = 2;
}

void CannonsTower::Attack(double delta)
{
	for (std::shared_ptr<Object>& object : Object::GetObjectsWithTag(Tag::Enemys))
	{
		std::shared_ptr<Enemy> enemy = std::static_pointer_cast<Enemy>(object);

		if (glm::distance(position, enemy->GetPosition()) <= attackRange && enemy->GetRemainingHitPoints() > 0)
		{
			glm::vec3 norm = enemy->GetPosition() - position;
			glm::vec2 dir(norm.x, norm.z);
			glm::vec2 normal = glm::normalize(dir);
			angle = atan2(normal.x, normal.y);

			if (glm::abs(angle - previousAngle) < rotationSpeed * delta || (glm::two_pi<float>() - glm::abs(angle - previousAngle)) < rotationSpeed * delta)
			{
				previousAngle = angle;
			}
			else
			{
				if (glm::abs(previousAngle - angle) > glm::pi<float>())
				{
					if (previousAngle > angle)
					{
						previousAngle += rotationSpeed * delta;

						if (glm::abs(previousAngle) > glm::pi<float>())
						{
							if (previousAngle > 0) previousAngle -= glm::two_pi<float>();
							else previousAngle += glm::two_pi<float>();
						}
					}
					else if (previousAngle < angle)
					{
						previousAngle -= rotationSpeed * delta;

						if (glm::abs(previousAngle) > glm::pi<float>())
						{
							if (previousAngle > 0) previousAngle -= glm::two_pi<float>();
							else previousAngle += glm::two_pi<float>();
						}

					}
				}
				else
				{
					if (previousAngle > angle)
					{
						previousAngle -= rotationSpeed * delta;
					}
					else if (previousAngle < angle)
					{
						previousAngle += rotationSpeed * delta;
					}
				}
			}

			modelTransformations.at(0).UpdateTransformation(1, glm::rotate(glm::mat4(1.0), previousAngle, glm::vec3(0.0, 1.0, 0.0)));
			modelTransformations.at(0).CombineTransformationMatrices();

			if (previousAttack < attackRate) break;
			if (glm::abs(angle - previousAngle) > 0.01) break;


			glm::vec3 initialPosition = glm::vec3(0.5, 0.0, 0.5) + position + glm::vec3(glm::rotate(glm::mat4(1.0), angle, glm::vec3(0.0, 1.0, 0.0)) * glm::vec4(projectilePosition, 1.0));

			MovingProjectile::CreateInstance(projectileModel, Tag::Cannonball, initialPosition, scale, enemy->GetPosition() + glm::vec3(0.5, 0.0, 0.5), 3, attackDamage, 0.6);

			previousAttack = 0;

			break;
		}
	}
}

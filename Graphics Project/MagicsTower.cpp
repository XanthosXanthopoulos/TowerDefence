#include "MagicsTower.h"

#include "Enemy.h"
#include "CastProjectile.h"

std::shared_ptr<MagicsTower> MagicsTower::CreateInstance(std::shared_ptr<GeometryNode> model, glm::vec3 position, glm::vec3 rotationAxis, double angle, glm::vec3 scale, double attackDamage, double attackRate, double attackRange, std::shared_ptr<GeometryNode> projectileModel)
{
	std::shared_ptr<MagicsTower> tower(new MagicsTower(model, position, rotationAxis, angle, scale, attackDamage, attackRate, attackRange, projectileModel));
	//Object::gameObjects.at(Tag::MagicTower).push_back(tower);
	return tower;
}

MagicsTower::MagicsTower(std::shared_ptr<GeometryNode> model, glm::vec3 position, glm::vec3 rotationAxis, double angle, glm::vec3 scale, double attackDamage, double attackRate, double attackRange, std::shared_ptr<GeometryNode> projectileModel) : TowerBase(model, Tag::MagicTower, position, rotationAxis, angle, scale, attackDamage, attackRate, attackRange), projectileModel(projectileModel)
{
}

MagicsTower::~MagicsTower()
{
}

void MagicsTower::Attack(double delta)
{
	for (std::shared_ptr<Object>& object : Object::GetObjectsWithTag(Tag::Enemys))
	{
		std::shared_ptr<Enemy> enemy = std::static_pointer_cast<Enemy>(object);

		if (glm::distance(position, enemy->GetPosition()) <= attackRange && enemy->GetRemainingHitPoints() > 0)
		{
			if (previousAttack < attackRate) break;

			CastProjectile::CreateInstance(projectileModel, position, enemy, attackDamage, 0.5);
			enemy->Damage(attackDamage);

			previousAttack = 0;

			break;
		}
	}
}

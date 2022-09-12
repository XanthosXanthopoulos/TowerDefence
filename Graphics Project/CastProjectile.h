#pragma once

#include "ProjectileBase.h"
#include "Enemy.h"

class CastProjectile : public ProjectileBase
{
public:

	static std::shared_ptr<CastProjectile> CreateInstance(std::shared_ptr<GeometryNode> model, glm::vec3 position, std::shared_ptr<Enemy> enemy, double damage, double lifetime);

	CastProjectile(std::shared_ptr<GeometryNode> model, glm::vec3 position, std::shared_ptr<Enemy> enemy, double damage, double lifetime);

	~CastProjectile();

	virtual void Move(double delta);

	float GetRemainingLifetime() const;

	virtual void Render(ShaderProgram& shader) const override;

protected:

	glm::vec3 position;

	double lifetime;
	double remainingLifetime;

	std::shared_ptr<Enemy> target;

};
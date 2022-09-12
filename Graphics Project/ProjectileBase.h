#pragma once

#include "Object.h"

class ProjectileBase : public Object
{
public:

	ProjectileBase(std::shared_ptr<GeometryNode> model, Tag tag, double damage);

	virtual void Update(double delta);

	virtual void Move(double delta) = 0;

	double GetDamage() const;

protected:

	double damage;
};
#include "ProjectileBase.h"

ProjectileBase::ProjectileBase(std::shared_ptr<GeometryNode> model, Tag tag, double damage) : Object(tag, model), damage(damage) { }

void ProjectileBase::Update(double delta)
{
	Move(delta);	
}

double ProjectileBase::GetDamage() const
{
	return damage;
}
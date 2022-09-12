#include "BoundingSphere.h"

#include <iostream>

BoundingSphere::BoundingSphere() : origin(0.0), radius(1) { }

BoundingSphere::BoundingSphere(glm::vec3 origin, float radius) : origin(origin), radius(radius) { }

BoundingSphere::~BoundingSphere() { }

bool BoundingSphere::Intersect(const BoundingSphere& boundingSphere) const
{
	float distance = glm::distance(origin, boundingSphere.origin);

	if (distance <= radius + boundingSphere.radius) return true;

	return false;
}

void BoundingSphere::SetOrigin(glm::vec3 origin)
{
	this->origin = origin;
}

void BoundingSphere::SetRadius(float radius)
{
	this->radius = radius;
}

void BoundingSphere::Move(glm::vec3 offest)
{
	origin += offest;
}
#pragma once

#include "BoundingSphere.h"

class ICollidable
{
public:

	virtual bool Collide(const ICollidable& other) = 0;

	BoundingSphere boundingSphere;
};
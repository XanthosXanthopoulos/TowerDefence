#include "ICollidable.h"

ICollidable::ICollidable(glm::vec3 center, double radius) : boundingSphere(center, radius)
{
}

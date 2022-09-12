#include "MovingProjectile.h"

#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

MovingProjectile::MovingProjectile(std::shared_ptr<GeometryNode> model, Tag tag, glm::vec3 position, glm::vec3 scale, glm::vec3 target, double movementSpeed, double damage, double collisionRadius) : ProjectileBase(model, tag, damage), position(position), movementSpeed(movementSpeed)
{
	modelTransformations.at(0).AddScale(glm::vec3(0.3885));
	modelTransformations.at(0).AddTranslation(position);
	modelTransformations.at(0).CombineTransformationMatrices();

	direction = target - position;
	direction.y = 0;

	boundingSphere.SetOrigin(this->position);
	boundingSphere.SetRadius(collisionRadius * scale.x);
}

std::shared_ptr<MovingProjectile> MovingProjectile::CreateInstance(std::shared_ptr<GeometryNode> model, Tag tag, glm::vec3 position, glm::vec3 scale, glm::vec3 target, double movementSpeed, double damage, double collisionRadius)
{
	std::shared_ptr<MovingProjectile> projectile(new MovingProjectile(model, tag, position, scale, target, movementSpeed, damage, collisionRadius));
	//Object::gameObjects.at(tag).push_back(projectile);
	return projectile;
}

MovingProjectile::~MovingProjectile() { }

void MovingProjectile::Move(double delta)
{
	position += direction * static_cast<float>(movementSpeed * delta);

	boundingSphere.Move(direction * static_cast<float>(movementSpeed * delta));

	modelTransformations.at(0).UpdateTransformation(1, glm::translate(glm::mat4(1.0), position));
	modelTransformations.at(0).CombineTransformationMatrices();
}

bool MovingProjectile::Collide(const ICollidable& other)
{
	return boundingSphere.Intersect(other.boundingSphere);
}

glm::vec3 MovingProjectile::GetPosition() const
{
	return position;
}

#include "Box.h"

Box::Box(std::shared_ptr<GeometryNode> model, glm::vec3 position, glm::vec3 scale, glm::vec3 collisionTriggerPosition, float collisionTriggerRadius) : Object(Tag::Arrow, model)
{
	modelTransformations.at(0).AddScale(scale);
	modelTransformations.at(0).AddTranslation(position);

	modelTransformations.at(0).CombineTransformationMatrices();

	boundingSphere.SetOrigin(collisionTriggerPosition);
	boundingSphere.SetRadius(collisionTriggerRadius);
}

Box::~Box() { }

void Box::Update(double delta) { }

bool Box::Collide(const ICollidable & other)
{
	return boundingSphere.Intersect(other.boundingSphere);
}

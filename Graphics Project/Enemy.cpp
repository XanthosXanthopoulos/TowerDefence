#include "Enemy.h"

#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/vector_angle.hpp"
#include "Landscape.h"

#include <iostream>

Enemy::Enemy(std::shared_ptr<GeometryNode> body, std::shared_ptr<GeometryNode> rightHand, std::shared_ptr<GeometryNode> leftFoot, std::shared_ptr<GeometryNode> rightFoot, glm::vec3 position, glm::vec3 rotationAxis, double angle, glm::vec3 scale, double movementSpeed, float hitPoints, float value, float collisionRadius) : Object(Tag::Enemys, body, rightHand, leftFoot, rightFoot), position(position), angle(angle), movementSpeed(movementSpeed), hitPoints(hitPoints), remainingHitPoints(hitPoints), value(value)
{
	modelTransformations.at(0).AddScale(glm::vec3(scale));
	modelTransformations.at(0).AddRotation(rotationAxis, angle);
	modelTransformations.at(0).AddTranslation(position);

	modelTransformations.at(1).AddTranslation(glm::vec3(0, -3, 0));
	modelTransformations.at(1).AddRotation(glm::vec3(0.0, 0.0, 1.0), 0);
	modelTransformations.at(1).AddTranslation(glm::vec3(4.5, 12, 0));
	modelTransformations.at(1).AddScale(glm::vec3(scale));
	modelTransformations.at(1).AddRotation(rotationAxis, angle);
	modelTransformations.at(1).AddTranslation(position);

	modelTransformations.at(2).AddTranslation(glm::vec3(-4, 0, -2));
	modelTransformations.at(2).AddRotation(glm::vec3(0.0, 0.0, 1.0), 0);
	modelTransformations.at(2).AddScale(glm::vec3(scale));
	modelTransformations.at(2).AddRotation(rotationAxis, angle);
	modelTransformations.at(2).AddTranslation(position);

	modelTransformations.at(3).AddTranslation(glm::vec3(4, 0, -2));
	modelTransformations.at(3).AddRotation(glm::vec3(0.0, 0.0, 1.0), 0);
	modelTransformations.at(3).AddScale(glm::vec3(scale));
	modelTransformations.at(3).AddRotation(rotationAxis, angle);
	modelTransformations.at(3).AddTranslation(position);

	modelTransformations.at(0).CombineTransformationMatrices();
	modelTransformations.at(1).CombineTransformationMatrices();
	modelTransformations.at(2).CombineTransformationMatrices();
	modelTransformations.at(3).CombineTransformationMatrices();

	currentAngle = 0;
	maxAngle = 1.0;
	AnimationSpeed = 3 ;
	currentTile = targetTile = std::pair<int, int>(int(floor(position.x + 20)), int(floor(position.z + 20)));

	boundingSphere.SetOrigin(glm::vec3(-0.5957, 11.683, -4.274) * scale);
	boundingSphere.Move(position + glm::vec3(0.5, 0.0, 0.5));
	boundingSphere.SetRadius(collisionRadius * scale.x);
}

std::shared_ptr<Enemy> Enemy::CreateInstance(std::shared_ptr<GeometryNode> body, std::shared_ptr<GeometryNode> rightHand, std::shared_ptr<GeometryNode> leftFoot, std::shared_ptr<GeometryNode> rightFoot, glm::vec3 position, glm::vec3 rotationAxis, double angle, glm::vec3 scale, double movementSpeed, float hitPoints, float value, float collisionRadius)
{
	std::shared_ptr<Enemy> enemy(new Enemy(body, rightHand, leftFoot, rightFoot, position, rotationAxis, angle, scale, movementSpeed, hitPoints, value, collisionRadius));
	//Object::gameObjects.at(Tag::Enemys).push_back(enemy);

	std::cout << enemy.use_count() << std::endl;
	return enemy;
}

Enemy::~Enemy() 
{
	std::cout << "DESTROY" << std::endl;
}

void Enemy::Animate(double delta)
{
	currentAngle += delta * AnimationSpeed;
	double feetAngle = currentAngle / 2;

	if (currentAngle > maxAngle && AnimationSpeed > 0) AnimationSpeed *= -1;
	if (currentAngle < -maxAngle && AnimationSpeed < 0) AnimationSpeed *= -1;

	modelTransformations.at(1).UpdateTransformation(1, glm::rotate(glm::mat4(1.0), float(currentAngle), glm::vec3(1.0, 0.0, 0.0)));
	modelTransformations.at(2).UpdateTransformation(1, glm::rotate(glm::mat4(1.0), float(feetAngle + maxAngle / 2), glm::vec3(1.0, 0.0, 0.0)));
	modelTransformations.at(3).UpdateTransformation(1, glm::rotate(glm::mat4(1.0), float(maxAngle / 2 - feetAngle), glm::vec3(1.0, 0.0, 0.0)));

	modelTransformations.at(1).CombineTransformationMatrices();
	modelTransformations.at(2).CombineTransformationMatrices();
	modelTransformations.at(3).CombineTransformationMatrices();
}

void Enemy::Move(double delta)
{
	std::shared_ptr<Landscape> map = std::static_pointer_cast<Landscape>(Object::GetObjectsWithTag(Tag::Map)[0]);

	if (currentTile.first == targetTile.first && currentTile.second == targetTile.second)
	{
		targetTile = map->GetNextPosition(currentTile);
	}

	glm::vec3 direction = glm::vec3(targetTile.first, position.y, targetTile.second) - glm::vec3(currentTile.first, position.y, currentTile.second);

	position += direction * glm::vec3(movementSpeed * delta);
	boundingSphere.Move(direction * glm::vec3(movementSpeed * delta));

	if (direction.x < 0 || direction.z < 0)
	{
		currentTile = std::pair<int, int>(int(ceil(position.x + 20)), int(ceil(position.z + 20)));
	}
	else
	{
		currentTile = std::pair<int, int>(int(floor(position.x + 20)), int(floor(position.z + 20)));
	}

	modelTransformations.at(0).UpdateTransformation(1, glm::rotate(glm::mat4(1.0), angle + atan2(direction.z, -direction.x), glm::vec3(0, 1, 0)));
	modelTransformations.at(0).UpdateTransformation(2, glm::translate(glm::mat4(1.0), position + glm::vec3(0.5, 0, 0.5)));
	
	modelTransformations.at(1).UpdateTransformation(4, glm::rotate(glm::mat4(1.0), angle + atan2(direction.z, -direction.x), glm::vec3(0, 1, 0)));
	modelTransformations.at(1).UpdateTransformation(5, glm::translate(glm::mat4(1.0), position + glm::vec3(0.5, 0, 0.5)));

	modelTransformations.at(2).UpdateTransformation(3, glm::rotate(glm::mat4(1.0), angle + atan2(direction.z, -direction.x), glm::vec3(0, 1, 0)));
	modelTransformations.at(2).UpdateTransformation(4, glm::translate(glm::mat4(1.0), position + glm::vec3(0.5, 0, 0.5)));

	modelTransformations.at(3).UpdateTransformation(3, glm::rotate(glm::mat4(1.0), angle + atan2(direction.z, -direction.x), glm::vec3(0, 1, 0)));
	modelTransformations.at(3).UpdateTransformation(4, glm::translate(glm::mat4(1.0), position + glm::vec3(0.5, 0, 0.5)));

	modelTransformations.at(0).CombineTransformationMatrices();
	modelTransformations.at(1).CombineTransformationMatrices();
	modelTransformations.at(2).CombineTransformationMatrices();
	modelTransformations.at(3).CombineTransformationMatrices();
}

void Enemy::Damage(double damage)
{
	remainingHitPoints -= damage;
}

double Enemy::GetRemainingHitPoints() const
{
	return remainingHitPoints;
}

float Enemy::GetRemainingLifetime() const
{
	return remainingLifetime;
}

float Enemy::GetValue() const
{
	return value;
}

glm::vec3 Enemy::GetPosition() const
{
	return position;
}

void Enemy::Update(double delta)
{
	if (remainingHitPoints <= 0)
	{
		opacity = remainingLifetime / lifetime;
		remainingLifetime -= delta;
		return;
	}

	Animate(delta);
	Move(delta);
}


bool Enemy::Collide(const ICollidable & other)
{
	return boundingSphere.Intersect(other.boundingSphere);
}

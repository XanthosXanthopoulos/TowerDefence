#include "Spawner.h"

#include <thread>
#include <iostream>

#include "Enemy.h"

Spawner::Spawner(ObjectLibrary& library, glm::vec3 spawnPosition, float rotation) : library(library), spawnPosition(spawnPosition), rotation(rotation) 
{
	waveActive = false;
	waveSize = 10;
	spawnRate = 2;
}

Spawner::~Spawner() { }

void Spawner::PlaceAt(glm::vec3 spawnPosition, float rotation)
{
	this->spawnPosition = spawnPosition;
	this->rotation = rotation;
}

void Spawner::StartWave()
{
	if (!waveActive)
	{
		waveActive = true;
		spawnedEnemies = 0;
		actualTime = 0;
	}
}

void Spawner::UpgradeWave(int waveNumber)
{
	pirateSpeed = 2 + waveNumber * 0.3;
	hitPoints = 10 + 3 * waveNumber;
	value = 5 + 2 * waveNumber;
}

void Spawner::Spawn(float delta)
{
	if (waveActive && actualTime > spawnRate)
	{
		actualTime = 0;
		Enemy::CreateInstance(library.GetObject("PirateBody"), library.GetObject("PirateRightHand"), library.GetObject("PirateLeftFoot"), library.GetObject("PirateRightFoot"), spawnPosition, glm::vec3(0.0, 1.0, 0.0), rotation, glm::vec3(0.045), pirateSpeed, hitPoints, value, 12);
		++spawnedEnemies;
		actualTime = 0;

		if (spawnedEnemies == waveSize)
		{
			waveActive = false;
		}
	}
	else
	{
		actualTime += delta;
	}
}

bool Spawner::IsActive()
{
	return waveActive;
}

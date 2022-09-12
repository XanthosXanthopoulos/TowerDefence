#pragma once

#include <vector>

#include "glm/glm.hpp"
#include "ObjectLibrary.h"

class Spawner
{
public:

	/// <summary>
	/// </summary>
	/// <param name='position'> The position of the spawner. </param>
	/// <param name='rotation'> The angle of rotation the spawner. </param>
	Spawner(ObjectLibrary& library, glm::vec3 spawnPosition, float rotation);

	/// <summary>
	/// Virtual destructor.
	/// </summary>
	virtual ~Spawner();

	void PlaceAt(glm::vec3 spawnPosition, float rotation);

	/// <summary>
	/// Improves the enemy stats from the next wave and on.
	/// </summary>
	void UpgradeWave(int waveNumber);

	/// <summary>
	/// Enable the spawner.
	/// </summary>
	void StartWave();

	/// <summary>
	/// Spawn an enemy.
	/// </summary>
	void Spawn(float delta);

	bool IsActive();

private:

	ObjectLibrary library;
	bool waveActive;

	glm::vec3 spawnPosition;
	float rotation;

	unsigned int waveSize;
	unsigned int spawnedEnemies;
	float spawnRate;
	float actualTime;

	float pirateSpeed = 0;
	float hitPoints = 0;
	float value = 0;
};


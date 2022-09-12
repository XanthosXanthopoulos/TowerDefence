#pragma once

#include "glm/glm.hpp"

class BoundingSphere
{
public:
	/// <summary>
	/// Default constructor.
	/// </summary>
	BoundingSphere();

	/// <summary>
	/// Construct a bounding sphere with he given origin annd redius.
	/// </summary>
	/// <param name='origin'> The center of the bounding sphere. </param>
	/// <param name='radius'> The radius of the bounding sphere. </param>
	BoundingSphere(glm::vec3 origin, float radius);

	/// <summary>
	/// Virtual destructor.
	/// </summary>
	~BoundingSphere();

	/// <summary>
	/// Check if the bounding sphere collides with another bounding sphere.
	/// </summary>
	/// <param name='boundingSphere'> The bounding sphere to check the collision against. </param>
	bool Intersect(const BoundingSphere& boundingSphere) const;

	/// <summary>
	/// Set the origin of the bounding sphere.
	/// </summary>
	/// <param name='origin'> The new origin of the bounding sphere. </param>
	void SetOrigin(glm::vec3 origin);

	/// <summary>
	/// Set the radius of the bounding sphere.
	/// </summary>
	/// <param name='radius'> The new radiusn of the bounding sphere. </param>
	void SetRadius(float radius);

	/// <summary>
	/// Move the bounding sphere the given amount.
	/// </summary>
	/// <param name='offset'> The amount to move the sphere. </param>
	void Move(glm::vec3 offset);

private:

	glm::vec3 origin;
	float radius;
};
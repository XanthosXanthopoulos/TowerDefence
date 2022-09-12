#pragma once

#include <vector>

#include "glm/glm.hpp"

class Transformation
{
public:

	Transformation();

	virtual ~Transformation();

	void AddTransformation(glm::mat4 transformationMatrix);

	void AddTranslation(glm::vec3 offset);

	void AddRotation(glm::vec3 axis, float angle);

	void AddScale(glm::vec3 ratio);

	void UpdateTransformation(unsigned int index, glm::mat4 trasformationMatrix);

	glm::mat4 RemoveTransformation(unsigned int index);

	void CombineTransformationMatrices();

	glm::mat4 GetTotalTranformationMatrix() const;

	glm::mat4 GetTotalNormalTranformationMatrix() const;

private:

	glm::mat4 totalTansformationMatrix;
	glm::mat4 totalNormalTansformationMatrix;

	std::vector<glm::mat4> transformationList;

};


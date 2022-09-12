#include "Transformation.h"

#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

Transformation::Transformation() : totalTansformationMatrix(1.0) { }

Transformation::~Transformation() { }

void Transformation::AddTransformation(glm::mat4 transformationMatrix)
{
	transformationList.push_back(transformationMatrix);
}

void Transformation::AddTranslation(glm::vec3 offset)
{
	transformationList.push_back(glm::translate(glm::mat4(1.0), offset));
}

void Transformation::AddRotation(glm::vec3 axis, float angle)
{
	transformationList.push_back(glm::rotate(glm::mat4(1.0), angle, axis));
}

void Transformation::AddScale(glm::vec3 ratio)
{
	transformationList.push_back(glm::scale(glm::mat4(1.0), ratio));
}

void Transformation::UpdateTransformation(unsigned int index, glm::mat4 transformationMatrix)
{
	transformationList[index] = transformationMatrix;
}

glm::mat4 Transformation::RemoveTransformation(unsigned int index)
{
	glm::mat4 element = transformationList[index];
	transformationList.erase(transformationList.begin() + index);

	return element;
}

void Transformation::CombineTransformationMatrices()
{
	totalTansformationMatrix = glm::mat4(1.0);

	for (int i = transformationList.size() - 1; i >= 0; --i)
	{
		totalTansformationMatrix = totalTansformationMatrix * transformationList[i];
	}

	totalNormalTansformationMatrix = glm::transpose(glm::inverse(totalTansformationMatrix));
}

glm::mat4 Transformation::GetTotalTranformationMatrix() const
{
	return totalTansformationMatrix;
}

glm::mat4 Transformation::GetTotalNormalTranformationMatrix() const
{
	return totalNormalTansformationMatrix;
}
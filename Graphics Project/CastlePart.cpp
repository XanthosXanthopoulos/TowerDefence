#include "CastlePart.h"

CastlePart::CastlePart(std::shared_ptr<GeometryNode> model, glm::vec3 position, glm::vec3 rotationAxis, double angle, glm::vec3 scale) : Object(Tag::Treee, model)
{
	modelTransformations.at(0).AddScale(scale);
	modelTransformations.at(0).AddRotation(rotationAxis, angle);
	modelTransformations.at(0).AddTranslation(position);
	modelTransformations.at(0).CombineTransformationMatrices();
}

CastlePart::~CastlePart() { }

void CastlePart::Update(double delta) { }

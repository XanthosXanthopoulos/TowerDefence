#include "Sea.h"

Sea::Sea(std::shared_ptr<GeometryNode> model, float waveSpeed) : Object(Tag::SeaWater, model), waveSpeed(waveSpeed), moveFactor(0) { }

Sea::~Sea() { }

void Sea::Update(double delta)
{
	moveFactor += waveSpeed * delta;

	if (moveFactor > 1.0) moveFactor = 0.0;

}

void Sea::Render(ShaderProgram & shader) const
{
	glUniform1f(shader["moveFactor"], moveFactor);

	Object::Render(shader);
}

float Sea::GetMoveFactor() const
{
	return moveFactor;
}

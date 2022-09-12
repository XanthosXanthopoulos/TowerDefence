#pragma once

#include "Object.h"

class Sea : public Object
{
public:

	Sea(std::shared_ptr<GeometryNode> model, float waveSpeed);

	~Sea();

	virtual void Update(double delta);

	virtual void Render(ShaderProgram& shader) const override;

	float GetMoveFactor() const;

protected:

	float waveSpeed;
	float moveFactor;
};
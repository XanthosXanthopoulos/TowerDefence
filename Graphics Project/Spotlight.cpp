#include "Spotlight.h"

Spotlight::Spotlight()
{
	umbra = 60;
	penumbra = 80;

	position = glm::vec3(0.0, 10.0, 0.0);
	direction = glm::vec3(0.0, -1.0, 0.0);
}

Spotlight::~Spotlight() { }

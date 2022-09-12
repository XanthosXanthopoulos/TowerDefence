#include "CastProjectile.h"

#include "glm/gtx/vector_angle.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

std::shared_ptr<CastProjectile> CastProjectile::CreateInstance(std::shared_ptr<GeometryNode> model, glm::vec3 position, std::shared_ptr<Enemy> enemy, double damage, double lifetime)
{
	std::shared_ptr<CastProjectile> projectile(new CastProjectile(model, position, enemy, damage, lifetime));
	//Object::gameObjects.at(Tag::Lightning).push_back(projectile);
	return projectile;
}

CastProjectile::CastProjectile(std::shared_ptr<GeometryNode> model, glm::vec3 position, std::shared_ptr<Enemy> enemy, double damage, double lifetime) : ProjectileBase(model, Tag::Lightning, damage) , position(position), target(enemy), lifetime(lifetime), remainingLifetime(lifetime)
{
	glm::vec3 direction = target->GetPosition() - position;

	modelTransformations.at(0).AddRotation(glm::vec3(0.0, 1.0, 0.0), -glm::quarter_pi<float>());
	modelTransformations.at(0).AddScale(glm::vec3(1.0, 1.0, 1.0 / glm::sqrt(800)));
	modelTransformations.at(0).AddScale(glm::vec3(1.0, 1.0, glm::length(glm::vec2(direction.x, direction.z))));
	modelTransformations.at(0).AddRotation(glm::vec3(0.0, 1.0, 0.0), atan2(direction.x, direction.z));
	modelTransformations.at(0).AddTranslation(position + glm::vec3(0.5, 1.0, 0.5));
	modelTransformations.at(0).CombineTransformationMatrices();
}

CastProjectile::~CastProjectile() { }

void CastProjectile::Move(double delta)
{
	glm::vec3 direction = target->GetPosition() - position;

	modelTransformations.at(0).UpdateTransformation(2, glm::scale(glm::mat4(1.0), glm::vec3(glm::length(glm::vec2(direction.x, direction.z)))));
	modelTransformations.at(0).UpdateTransformation(3, glm::rotate(glm::mat4(1.0), atan2(direction.x, direction.z), glm::vec3(0.0, 1.0, 0.0)));
	modelTransformations.at(0).CombineTransformationMatrices();

	remainingLifetime -= float(delta);
	opacity = float(remainingLifetime / lifetime);
}

float CastProjectile::GetRemainingLifetime() const
{
	return remainingLifetime;
}

void CastProjectile::Render(ShaderProgram & shader) const
{
	glBindVertexArray(models.at(0)->m_vao);
	glUniformMatrix4fv(shader["uniform_model_matrix"], 1, GL_FALSE, glm::value_ptr(modelTransformations.at(0).GetTotalTranformationMatrix()));
	//std::cout << remainingLifetime << " " << opacity << std::endl;
	glUniform1f(shader["opacity"], opacity);
	glUniform1f(shader["time"], float(remainingLifetime));
	glLineWidth(2);
	glDrawArrays(GL_LINE_STRIP, 0, 101);
	glUniform1f(shader["time"], 2 * remainingLifetime);
	glDrawArrays(GL_LINE_STRIP, 0, 101);
	glUniform1f(shader["time"], 3 * remainingLifetime);
	glDrawArrays(GL_LINE_STRIP, 0, 101);
	glBindVertexArray(0);
}

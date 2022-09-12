#include "DirectionalLight.h"

#include <iostream>

#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

DirectionalLight::DirectionalLight() : direction(-1.0, -1.0, 0)
{
	projectionMatrix = glm::ortho(-20.0, 20.0, -20.0, 20.0, 0.1, 10.0);
	viewMatrix = glm::lookAt(-direction * 20.0f, glm::vec3(0.0), glm::vec3(0.0, 1.0, 0.0));
}

DirectionalLight::DirectionalLight(glm::vec3 direction, glm::vec3 color, float intensity) : Light(color, intensity), direction(direction)
{
	projectionMatrix = glm::ortho(-25.0, 25.0, -18.0, 18.0, 0.1, 50.0);
	viewMatrix = glm::lookAt(-direction * 20.0f, glm::vec3(0.0), glm::vec3(0.0, 1.0, 0.0));
}

DirectionalLight::~DirectionalLight() { }

void DirectionalLight::SetupLight(ShaderProgram& shader)
{
	glUniformMatrix4fv(shader["uniform_light_projection_matrix"], 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(shader["uniform_light_view_matrix"], 1, GL_FALSE, glm::value_ptr(viewMatrix));
	
	glUniform3fv(shader["directionalLight.direction"], 1, glm::value_ptr(direction));
	glUniform3fv(shader["directionalLight.color"], 1, glm::value_ptr(color * intensity));

	glUniform1i(shader["uniform_cast_shadows"], enableShadowMap ? 1 : 0);
	glUniform1i(shader["shadowmap_texture"], 21);
	glActiveTexture(GL_TEXTURE21);
	glBindTexture(GL_TEXTURE_2D, enableShadowMap ? shadowMapDepthTexture : 0);
}

void DirectionalLight::SetupShadowMap(ShaderProgram& shader)
{
	if (!enableShadowMap) return;

	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glViewport(0, 0, shadowMapResolution, shadowMapResolution);
	GLenum drawbuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawbuffers);

	glUniformMatrix4fv(shader["uniform_projection_matrix"], 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(shader["uniform_view_matrix"], 1, GL_FALSE, glm::value_ptr(viewMatrix));
}
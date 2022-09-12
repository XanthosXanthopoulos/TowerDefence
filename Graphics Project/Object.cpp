#include "Object.h"

#include <iostream>

#include "TextureManager.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

std::map<Tag, std::vector<std::shared_ptr<Object>>> Object::gameObjects;

std::vector<std::shared_ptr<Object>>& Object::GetObjectsWithTag(Tag tag)
{
	std::vector<std::shared_ptr<Object>> empty;

	if (gameObjects.find(tag) == gameObjects.end())
	{
		return empty;
	}

	return gameObjects.at(tag);
}

Object::~Object() { }

void Object::Render(ShaderProgram& shader) const
{
	int textureNumber = 1;
	for (auto const&[key, value] : textures)
	{
		glUniform1i(shader[key], textureNumber);
		glActiveTexture(GL_TEXTURE0 + textureNumber);
		glBindTexture(GL_TEXTURE_2D, value);

		++textureNumber;
	}

	for (int i = 0; i < models.size(); ++i)
	{
		glBindVertexArray(models.at(i)->m_vao);

		glUniformMatrix4fv(shader["uniform_model_matrix"], 1, GL_FALSE, glm::value_ptr(modelTransformations.at(i).GetTotalTranformationMatrix()));
		glUniformMatrix4fv(shader["uniform_normal_matrix"], 1, GL_FALSE, glm::value_ptr(modelTransformations.at(i).GetTotalNormalTranformationMatrix()));
		glUniform1f(shader["opacity"], opacity);
		glUniform1i(shader["diffuse_texture"], 0);
		glActiveTexture(GL_TEXTURE0);

		for (int j = 0; j < models.at(i)->parts.size(); j++)
		{
			glm::vec3 diffuseColor = models.at(i)->parts[j].diffuseColor;
			glm::vec3 specularColor = models.at(i)->parts[j].specularColor;
			float shininess = models.at(i)->parts[j].shininess;
			glUniform3f(shader["uniform_diffuse"], diffuseColor.r, diffuseColor.g, diffuseColor.b);
			glUniform3f(shader["uniform_specular"], specularColor.r, specularColor.g, specularColor.b);
			glUniform1f(shader["uniform_shininess"], shininess);
			glUniform1f(shader["uniform_has_texture"], (models.at(i)->parts[j].textureID > 0) ? 1.0f : 0.0f);
			glBindTexture(GL_TEXTURE_2D, models.at(i)->parts[j].textureID);

			glDrawArrays(models.at(i)->renderMode, models.at(i)->parts[j].start_offset, models.at(i)->parts[j].count);
		}
	}
}

bool Object::AddTexture(std::string key, std::string filename, bool mipmap)
{
	if (textures.size() > 20)
	{
		std::cerr << "Maximum number of textures" << std::endl;
		return false;
	}

	textures.insert(std::pair<std::string, GLuint>(key, TextureManager::GetInstance().RequestTexture(filename.c_str(), mipmap)));

	return true;
}

bool Object::AddTexture(std::string key, GLuint texture)
{
	if (textures.size() > 20)
	{
		std::cerr << "Maximum number of textures" << std::endl;
		return false;
	}

	textures.insert(std::pair<std::string, GLuint>(key, texture));

	return true;
}

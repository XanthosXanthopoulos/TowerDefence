#pragma once

#include <memory>
#include <map>
#include <string>

#include "GeometryNode.h"
#include "IRenderable.h"
#include "IUpdateable.h"
#include "Transformation.h"
#include "Tag.h"
#include "glm/glm.hpp"
#include <iostream>

class Object : public IRenderable, public IUpdateable
{
public:

	template <class ... T> Object(Tag tag, T... model) : tag(tag)
	{
		for (std::shared_ptr<GeometryNode> x : { model... })
		{
 			models.push_back(x);
			modelTransformations.push_back(Transformation());
		}

		if (gameObjects.find(tag) == gameObjects.end())
		{
			std::cout << "Make" << std::endl;
			gameObjects.insert(std::pair<Tag, std::vector<std::shared_ptr<Object>>>(tag, std::vector<std::shared_ptr<Object>>()));
		}

		gameObjects.at(tag).push_back(std::shared_ptr<Object>(std::move(this)));
	}

	virtual ~Object();

	virtual void Render(ShaderProgram& shader) const;

	bool AddTexture(std::string key, std::string filename, bool mipmap);

	bool AddTexture(std::string key, GLuint texture);

	static std::vector<std::shared_ptr<Object>>& GetObjectsWithTag(Tag tag);

	static std::map<Tag, std::vector<std::shared_ptr<Object>>> gameObjects;

protected:

	Tag tag;

	std::map<std::string, GLuint> textures;

	std::vector<std::shared_ptr<GeometryNode>> models;

	std::vector<Transformation> modelTransformations;

	float opacity = 1;
};

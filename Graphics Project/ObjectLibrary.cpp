#include "ObjectLibrary.h"

#include "OBJLoader.h"
#include "GeometricMesh.h"

#include <iostream>

ObjectLibrary::ObjectLibrary() { }

ObjectLibrary::ObjectLibrary(std::vector<std::string> keys, std::vector<std::string> objectFilenames)
{
	if (keys.size() != objectFilenames.size())
	{
		std::cerr << "ERROR: Different number of keys and object filenames. No Object loaded." << std::endl;
		return;
	}

	OBJLoader loader;

	for (int i = 0; i < keys.size(); ++i)
	{
		GeometricMesh* mesh = loader.load(objectFilenames[i].c_str());

		if (mesh != nullptr)
		{
			GeometryNode* objectGeometry = new GeometryNode();
			objectGeometry->Init(mesh);
			objectMap.insert(std::pair<std::string, GeometryNode*>(keys[i], objectGeometry));
		}
		else
		{
			std::cerr << "ERROR: Could not load model. Filepath: " << objectFilenames[i] << std::endl;
		}

		delete mesh;
	}
}

ObjectLibrary::~ObjectLibrary() { }

void ObjectLibrary::AddObject(std::string key, std::string filename)
{
	OBJLoader loader;
	GeometricMesh* mesh = loader.load(filename.c_str());

	if (mesh != nullptr)
	{
		std::shared_ptr<GeometryNode> objectGeometry = std::make_shared<GeometryNode>(GeometryNode());
		objectGeometry->Init(mesh);
		objectMap.insert(std::pair<std::string, std::shared_ptr<GeometryNode>>(key, objectGeometry));
	}
	else
	{
		std::cerr << "ERROR: Could not load model. Filepath: " << filename << std::endl;
	}

	delete mesh;
}

std::shared_ptr<GeometryNode> ObjectLibrary::GetObject(std::string key)
{
	return objectMap.at(key);
}

void ObjectLibrary::RemoveObject(std::string key)
{
	objectMap.erase(key);
}

#pragma once

#include "GeometryNode.h"
#include "ShaderProgram.h"
#include <string>
#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>
#include <memory>

class ObjectLibrary
{
public:

	/// <summary>
	/// Default constructor.
	/// </summary>
	ObjectLibrary();

	/// <summary>
	/// Constructs an ObjectLibrary object and loads it with objects.
	/// </summary>
	/// <param name='keys'> The keys of the objects to be loaded. </param>
	/// <param name='objectFilenames'> The filenames of the objects to be loaded. </param>
	ObjectLibrary(std::vector<std::string> keys, std::vector<std::string> objectFilenames);

	/// <summary>
	/// Virtual destructor.
	/// </summary>
	virtual ~ObjectLibrary();

	/// <summary>
	/// Loads an object into the library and assigns a key to it.
	/// </summary>
	/// <param name='key'> The key of the object. </param>
	/// <param name='filename'> The filename of the object to be loaded. </param>
	void AddObject(std::string key, std::string filename);

	/// <summary>
	/// Gets the object with the specified key.
	/// </summary>
	/// <param name='key'> The key of the object. </param>
	std::shared_ptr<GeometryNode> GetObject(std::string key);

	/// <summary>
	/// Removes the object with the given ey from the library.
	/// </summary>
	/// <para name='key'> The key of the object to be removed. </param>
	void RemoveObject(std::string key);

private:

	std::unordered_map<std::string, std::shared_ptr<GeometryNode>> objectMap;
};


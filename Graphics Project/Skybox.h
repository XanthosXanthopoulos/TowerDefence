#pragma once

#include <string>
#include <vector>

#include "glm/glm.hpp"
#include "ShaderProgram.h"

class Skybox
{
public:
	/// <summary>
	/// Default constructor.
	/// </summary>
	Skybox();

	/// <summary>
	/// Construct a skybox with the given size.
	/// </summary>
	/// <param name='size'> The size of the skybox. </param>
	Skybox(float size);

	/// <summary>
	/// Virtual destructor.
	/// </summary>
	virtual ~Skybox();

	void AddTexture(std::vector<std::string> filenames);

	/// <summary>
	/// Render the model on the screen
	/// </summary>
	/// <param name='shader'> The shader program for rendering the model. </param>
	void Render(ShaderProgram& shader);

private:

	GLuint vao;
	GLuint vbo;

	GLuint cubeMapTexture;
};


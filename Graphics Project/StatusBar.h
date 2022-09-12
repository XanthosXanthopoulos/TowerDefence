#pragma once

#include <string>

#include "ShaderProgram.h"
#include "Text.h"

class StatusBar
{
public:
	/// <summary>
	/// Default constructor.
	/// </summary>
	StatusBar();

	/// <summary>
	/// Construct a status bar with certain height.
	/// </summary>
	/// <param name='height'> The height of the status bar in pixels. </param>
	/// <param name='screenWidth'> The width of the window in pixels. </param>
	/// <param name='screenHeight'> The height of the window in pixels. </param>
	StatusBar(double height);

	/// <summary>
	/// Destructor.
	/// </summary>
	~StatusBar();

	void AddBackgroundTexture(std::string filepath);

	void Resize(double screenWidth, double screenHeight);

	/// <summary>
	/// Render the model on the screen
	/// </summary>
	/// <param name='shader'> The shader program for rendering the model. </param>
	void Render(ShaderProgram& shader);

private:

	double height;

	GLuint vao;
	GLuint vbo;
	GLuint texture;
};


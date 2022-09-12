#include "StatusBar.h"

#include <iostream>

#include "TextureManager.h"

StatusBar::StatusBar() { }

StatusBar::StatusBar(double height) : height(height) { }

StatusBar::~StatusBar()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

void StatusBar::AddBackgroundTexture(std::string filepath)
{
	texture = TextureManager::GetInstance().RequestTexture(filepath.c_str(), false);
}

void StatusBar::Resize(double screenWidth, double screenHeight)
{
	if (vao)
	{
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
	}

	float ratio = 1 - 2 * height / screenHeight;

	float vertices[] = {
		-1, ratio,
		1, ratio,
		-1, 1,
		1, 1,
	};

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,				// attribute index
		2,              // number of elements per vertex, here (x,y,z)
		GL_FLOAT,       // the type of each element
		GL_FALSE,       // take our values as-is
		0,		         // no extra data between each position
		0				// pointer to the C array or an offset to our buffer
	);

	std::cout << vao << " " << vbo << std::endl;
}

void StatusBar::Render(ShaderProgram& shader)
{
	glBindVertexArray(vao);
	glUniform1i(shader["background"], 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

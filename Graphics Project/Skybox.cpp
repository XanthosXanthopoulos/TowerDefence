#include "Skybox.h"

#include <iostream>

#include "TextureManager.h"

Skybox::Skybox() { }

Skybox::Skybox(float size)
{
	float vertices[] = {
		-size,  size, -size,
		-size, -size, -size,
		size, -size, -size,
		 size, -size, -size,
		 size,  size, -size,
		-size,  size, -size,

		-size, -size,  size,
		-size, -size, -size,
		-size,  size, -size,
		-size,  size, -size,
		-size,  size,  size,
		-size, -size,  size,

		 size, -size, -size,
		 size, -size,  size,
		 size,  size,  size,
		 size,  size,  size,
		 size,  size, -size,
		 size, -size, -size,

		-size, -size,  size,
		-size,  size,  size,
		 size,  size,  size,
		 size,  size,  size,
		 size, -size,  size,
		-size, -size,  size,

		-size,  size, -size,
		 size,  size, -size,
		 size,  size,  size,
		 size,  size,  size,
		-size,  size,  size,
		-size,  size, -size,

		-size, -size, -size,
		-size, -size,  size,
		 size, -size, -size,
		 size, -size, -size,
		-size, -size,  size,
		 size, -size,  size
	};

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,				// attribute index
		3,              // number of elements per vertex, here (x,y,z)
		GL_FLOAT,       // the type of each element
		GL_FALSE,       // take our values as-is
		0,		         // no extra data between each position
		0				// pointer to the C array or an offset to our buffer
	);
}


Skybox::~Skybox()
{
}

void Skybox::AddTexture(std::vector<std::string> filenames)
{
	cubeMapTexture = TextureManager::GetInstance().RequestCubeMap(filenames);
}

void Skybox::Render(ShaderProgram& shader)
{
	glBindVertexArray(vao);
	glUniform1i(shader["cubeMap"], 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);

	glDrawArrays(GL_TRIANGLES, 0, 36);
}

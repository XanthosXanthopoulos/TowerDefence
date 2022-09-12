#pragma once

#include <string>
#include <vector>

#include "Font.h"
#include "ShaderProgram.h"
#include "glm/glm.hpp"

class Text
{
public:
	/// <summary>
	/// Default constructor.
	/// </summary>
	Text();

	Text(std::string text, Font font, double fontSize, glm::vec3 color, glm::vec2 position, double maxLineLength, bool centered, bool fixedHeight);

	/// <summary>
	/// Virtual destructor.
	/// </summary>
	virtual ~Text();

	void GenerateTextMesh();

	void Resize(double screenWidth, double screenHeight);

	void Render(ShaderProgram& shader);

	void SetText(std::string text);

private:

	Font font;

	std::string text;

	double fontSize;
	double maxLineLength;
	double lineHeight = 0.03;

	bool fixedHeight;

	glm::vec2 position;
	glm::vec3 color;

	bool centered;

	GLuint vao;
	GLuint vertexVBO;
	GLuint textureVBO;

	std::vector<float> vertexCoord;
	std::vector<float> textureCoord;

	glm::vec2 scale;

};


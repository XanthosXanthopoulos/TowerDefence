#include "Text.h"

#include <vector>

#include "Line.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

Text::Text() : vao(0), vertexVBO(0), textureVBO(0) { }

Text::Text(std::string text, Font font, double fontSize, glm::vec3 color, glm::vec2 position, double maxLineLength, bool centered, bool fixedHeight) : text(text), font(font), fontSize(fontSize), color(color), position(position), maxLineLength(maxLineLength), centered(centered), fixedHeight(fixedHeight), vao(0), vertexVBO(0), textureVBO(0) { }

Text::~Text() 
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vertexVBO);
	glDeleteBuffers(1, &textureVBO);
}

void Text::GenerateTextMesh()
{
	vertexCoord.clear();
	textureCoord.clear();

	std::vector<Line> lines;

	Line currentLine(font.spaceWidth, fontSize, maxLineLength);
	Word currentWord(fontSize);

	for (char character : text)
	{
		// If next character is space
		if (character == 32)
		{
			if (!currentLine.AddWord(currentWord))
			{
				lines.push_back(currentLine);
				currentLine = Line(font.spaceWidth, fontSize, maxLineLength);
				currentLine.AddWord(currentWord);
			}

			currentWord = Word(fontSize);
		}
		else
		{
			currentWord.AddCharacter(font.charset.at(character));
		}
	}

	if (!currentLine.AddWord(currentWord))
	{
		lines.push_back(currentLine);
		currentLine = Line(font.spaceWidth, fontSize, maxLineLength);
		currentLine.AddWord(currentWord);
	}

	lines.push_back(currentLine);

	glm::vec2 cursor(0.0);

	for (Line line : lines)
	{
		if (centered) 
		{
			cursor.x = (maxLineLength - line.GetLineLength() * scale.x) / 2.0;
		}

		for (Word word : line.GetWords())
		{
			for (Character character : word.GetCharacters())
			{
				double x = cursor.x + character.GetXOffset() * fontSize * scale.x;
				double y = cursor.y + character.GetYOffset() * fontSize * scale.y;
				double maxX = x + character.GetSizeX() * fontSize * scale.x;
				double maxY = y + character.GetSizeY() * fontSize * scale.y;
				double properX = 2 * x - 1;
				double properY = -2 * y + 1;
				double properMaxX = 2 * maxX - 1;
				double properMaxY = -2 * maxY + 1;

				vertexCoord.push_back(properX);
				vertexCoord.push_back(properY);
				vertexCoord.push_back(properX);
				vertexCoord.push_back(properMaxY);
				vertexCoord.push_back(properMaxX);
				vertexCoord.push_back(properMaxY);
				vertexCoord.push_back(properMaxX);
				vertexCoord.push_back(properMaxY);
				vertexCoord.push_back(properMaxX);
				vertexCoord.push_back(properY);
				vertexCoord.push_back(properX);
				vertexCoord.push_back(properY);

				textureCoord.push_back(character.GetXTextureCoord());
				textureCoord.push_back(character.GetYTextureCoord());
				textureCoord.push_back(character.GetXTextureCoord());
				textureCoord.push_back(character.GetYMaxTextureCoord());
				textureCoord.push_back(character.GetXMaxTextureCoord());
				textureCoord.push_back(character.GetYMaxTextureCoord());
				textureCoord.push_back(character.GetXMaxTextureCoord());
				textureCoord.push_back(character.GetYMaxTextureCoord());
				textureCoord.push_back(character.GetXMaxTextureCoord());
				textureCoord.push_back(character.GetYTextureCoord());
				textureCoord.push_back(character.GetXTextureCoord());
				textureCoord.push_back(character.GetYTextureCoord());

				cursor.x += character.GetAdvance() * fontSize * scale.x;
			}

			cursor.x += font.spaceWidth * fontSize * scale.x;
		}

		cursor.x = 0;
		cursor.y += lineHeight * fontSize * scale.y;
	}

	if (vao)
	{
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vertexVBO);
		glDeleteBuffers(1, &textureVBO);
	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vertexVBO);
	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
	glBufferData(GL_ARRAY_BUFFER, vertexCoord.size() * sizeof(float), vertexCoord.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,				// attribute index
		2,              // number of elements per vertex, here (x,y,z)
		GL_FLOAT,       // the type of each element
		GL_FALSE,       // take our values as-is
		0,		         // no extra data between each position
		0				// pointer to the C array or an offset to our buffer
	);

	glGenBuffers(1, &textureVBO);
	glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
	glBufferData(GL_ARRAY_BUFFER, textureCoord.size() * sizeof(float), textureCoord.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,				// attribute index
		2,              // number of elements per vertex, here (x,y,z)
		GL_FLOAT,       // the type of each element
		GL_FALSE,       // take our values as-is
		0,		         // no extra data between each position
		0				// pointer to the C array or an offset to our buffer
	);
}

void Text::Resize(double screenWidth, double screenHeight)
{
	font.GenerateCharset(screenWidth / screenHeight);

	scale.x = 800.0 / screenWidth;
	scale.y = 600.0 / screenHeight;

	GenerateTextMesh();
}

void Text::Render(ShaderProgram& shader)
{
	font.SetupFont(shader);

	glBindVertexArray(vao);
	glUniform3fv(shader["color"], 1, glm::value_ptr(color));
	glUniform2fv(shader["position"], 1, glm::value_ptr(position));
	glUniform2fv(shader["scale"], 1, glm::value_ptr(scale));
	glUniform1i(shader["fixHeight"], fixedHeight ? 1 : 0);
	glDrawArrays(GL_TRIANGLES, 0, vertexCoord.size() / 2);
}

void Text::SetText(std::string text)
{
	this->text = text;
}
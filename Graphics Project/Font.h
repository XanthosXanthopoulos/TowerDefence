#pragma once

#include <string>
#include <map>

#include "Character.h"
#include "glm/glm.hpp"
#include "ShaderProgram.h"

struct CharacterInfo
{
	CharacterInfo(int id, int x, int y, int width, int height, int xoffset, int yoffset, int advance) : id(id), x(x), y(y), width(width), height(height), xoffset(xoffset), yoffset(yoffset), advance(advance) { }

	int id;
	int x;
	int y;
	int width;
	int height;
	int xoffset;
	int yoffset;
	int advance;
};

class Font
{
public:
	/// <summary>
	/// Default constructor.
	/// </summary>
	Font();

	/// <summary>
	/// Construct a font from a file.
	/// </summary>
	/// <param name='filepath'> The file containing the font parameters </param> 
	Font(std::string filepath);

	/// <summary>
	/// Virtual destructor.
	/// </summary>
	virtual ~Font();

	/// <summary>
	/// Generate the character set for the specific aspect ratio and line height.
	/// </summary>
	/// <param name='aspectRatio'> The aspect ratio of the window. </param>
	void GenerateCharset(double aspectRatio);

	/// <summary>
	/// Add the texture atlas for the specific font.
	/// </summary>
	/// <param name='filepath'> The file path for the texture atlas. </param>
	void AddTextureAtlas(std::string filepath);

	/// <summary>
	/// Pass  the required values to the shader for rendering text.
	/// </summary>
	/// <param name='filepath'> The shader to set up for rendering text. </param>
	void SetupFont(ShaderProgram& shader);

	std::map<int, Character> charset;
	double spaceWidth;

private:

	GLuint fontAtlas;

	glm::ivec4 padding;
	std::map<int, CharacterInfo> charsetInfo;

	int lineHeight;
	int desiredPadding = 3;
};

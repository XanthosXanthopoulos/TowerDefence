#include "Font.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include "TextureManager.h"

Font::Font() : fontAtlas(0) { }

Font::Font(std::string filepath) : fontAtlas(0)
{
	std::ifstream file;
	file.open(filepath, std::ios::in);

	std::string line;

	//Read padding value
	std::getline(file, line);
	std::istringstream stream(line);
	while (stream)
	{
		std::string token;

		stream >> token;

		if (token.rfind("padding=", 0) == 0)
		{
			token.erase(0, 8);
			std::stringstream ss(token);

			for (int i = 0; i < 4; ++i) 
			{
				ss >> padding[i];
				std::cout << padding[i] << std::endl;
				if (ss.peek() == ',') ss.ignore();
			}
		}
	}

	//Read line height
	std::getline(file, line);
	stream = std::istringstream(line);
	while (stream)
	{
		std::string token;

		stream >> token;

		if (token.rfind("lineHeight=", 0) == 0)
		{
			token.erase(0, 11);
			lineHeight = std::stoi(token);
		}
	}

	//Skip next to lines
	std::getline(file, line);
	std::getline(file, line);

	//Read character data
	while (std::getline(file, line))
	{
		//std::cout << line << std::endl;
		std::istringstream ss(line);

		int id;
		int x;
		int y;
		int width;
		int height;
		int xoffset;
		int yoffset;
		int advance;

		std::string token;

		while (ss >> token)
		{
			if (token.rfind("id=", 0) == 0)
			{
				token.erase(0, 3);
				id = std::stoi(token);
			}
			else if (token.rfind("x=", 0) == 0)
			{
				token.erase(0, 2);
				x = std::stoi(token);
			}
			else if (token.rfind("y=", 0) == 0)
			{
				token.erase(0, 2);
				y = std::stoi(token);
			}
			else if (token.rfind("width=", 0) == 0)
			{
				token.erase(0, 6);
				width = std::stoi(token);
			}
			else if (token.rfind("height=", 0) == 0)
			{
				token.erase(0, 7);
				height = std::stoi(token);
			}
			else if (token.rfind("xoffset=", 0) == 0)
			{
				token.erase(0, 8);
				xoffset = std::stoi(token);
			}
			else if (token.rfind("yoffset=", 0) == 0)
			{
				token.erase(0, 8);
				yoffset = std::stoi(token);
			}
			else if (token.rfind("xadvance=", 0) == 0)
			{
				token.erase(0, 9);
				advance = std::stoi(token);
			}
		}

		charsetInfo.insert(std::pair<int, CharacterInfo>(id, CharacterInfo(id, x, y, width, height, xoffset, yoffset, advance)));
	}
}

Font::~Font() { }

void Font::GenerateCharset(double aspectRatio)
{
	int lineHeightPixels = lineHeight - padding[1] - padding[3];
	double verticalPerPixelSize = 0.03 / double(lineHeightPixels);
	double horizontalPerPixelSize = verticalPerPixelSize / aspectRatio;

	for (auto const&[key, value] : charsetInfo)
	{
		if (key == 32)
		{
			spaceWidth = (value.advance - padding[0] - padding[2]) * horizontalPerPixelSize;
			continue;
		}

		double xTex = (double(value.x) + (padding[0] - desiredPadding)) / 512;
		double yTex = (double(value.y) + (padding[1] - desiredPadding)) / 512;
		int width = value.width - padding[0] - padding[2] + 2 * desiredPadding;
		int height = value.height - padding[1] - padding[3] + 2 * desiredPadding;
		double quadWidth = width * horizontalPerPixelSize;
		double quadHeight = height * verticalPerPixelSize;
		double xTexSize = (double)width / 512;
		double yTexSize = (double)height / 512;
		double xOff = (value.xoffset + padding[0] - desiredPadding) * horizontalPerPixelSize;
		double yOff = (value.yoffset + (padding[1] - desiredPadding)) * verticalPerPixelSize;
		double xAdvance = (value.advance - padding[0] - padding[2]) * horizontalPerPixelSize;
		charset.insert(std::pair<int, Character>(key, Character(key, xTex, yTex, xTexSize, yTexSize, xOff, yOff, quadWidth, quadHeight, xAdvance)));
	}
}

void Font::AddTextureAtlas(std::string filepath)
{
	fontAtlas = TextureManager::GetInstance().RequestTexture(filepath.c_str());
}

void Font::SetupFont(ShaderProgram& shader)
{
	glUniform1i(shader["fontAtlas"], 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fontAtlas);
}
#include "Landscape.h"

#include <fstream>
#include <iostream>

Landscape::Landscape(std::shared_ptr<GeometryNode> model) : Object(Tag::Map, model)
{
	tiles = nullptr;
	width = 0;
	height = 0;
}

std::shared_ptr<Landscape> Landscape::CreateInstance(std::shared_ptr<GeometryNode> model)
{
	std::shared_ptr<Landscape> landscape = std::make_shared<Landscape>(Landscape(model));
	Object::gameObjects.at(Tag::Map).push_back(landscape);

	return landscape;
}

Landscape::~Landscape()
{
	if (tiles)
	{
		for (int i = 0; i < height; ++i)
		{
			delete[] tiles[i];
		}

		delete[] tiles;
	}
}

void Landscape::Update(double delta) { }

void Landscape::LoadLandscapeState(std::string filename)
{
	if (tiles)
	{
		for (int i = 0; i < height; ++i)
		{
			delete[] tiles[i];
		}

		delete[] tiles;
	}

	std::ifstream file;
	file.open(filename, std::ios::in);

	if (!file.is_open())
	{
		std::cerr << "Error opening file" << std::endl;
		return;
	}

	int lines;

	int tileWidth, tileHeight;

	file >> width;
	file >> height;
	file >> tileWidth;
	file >> tileHeight;
	file >> lines;

	if (file.fail())
	{
		std::cerr << "Error reading header of file" << std::endl;
		return;
	}

	tiles = new LandscapeState*[height];
	for (int i = 0; i < height; ++i)
	{
		tiles[i] = new LandscapeState[width];

		for (int j = 0; j < width; ++j)
		{
			tiles[i][j] = LandscapeState::Empty;
		}
	}

	for (int i = 0; i < lines; ++i)
	{
		std::string type;
		int x, y;

		file >> type;
		file >> x;
		file >> y;

		if (!type.compare("r"))
		{
			tiles[x][y] = LandscapeState::Path;
		}
		else if (!type.compare("m"))
		{
			tiles[x][y] = LandscapeState::Unable;
		}
	}

	file.close();
}

void Landscape::LoadPath(std::string filename)
{
	std::ifstream file;
	file.open(filename, std::ios::in);

	if (!file.is_open())
	{
		std::cerr << "Error opening file" << std::endl;
		return;
	}

	if (file.fail())
	{
		std::cerr << "Error reading header of file" << std::endl;
		return;
	}

	std::pair<int, int> previous(-1, -1);

	while (!file.eof())
	{
		int x = -1;
		int y = -1;

		file >> x;
		file >> y;

		std::pair<int, int> current(x, y);

		if (previous.first == -1)
		{
			previous = current;
		}
		else
		{
			path.insert(std::pair<std::pair<int, int>, std::pair<int, int>>(previous, current));
			previous = current;
		}
	}

	path.insert(std::pair<std::pair<int, int>, std::pair<int, int>>(previous, previous));

	if (file.fail())
	{
		std::cerr << "Error reading path file" << std::endl;
		return;
	}

	file.close();
}

std::pair<int, int> Landscape::GetNextPosition(std::pair<int, int> currentPosition)
{
	return path.at(currentPosition);
}

LandscapeState Landscape::GetTileState(std::pair<int, int> tile)
{
	if (tile.first < 0 || tile.first >= width || tile.second < 0 || tile.second > height) return LandscapeState::Illegal;

	return tiles[tile.first][tile.second];
}

void Landscape::SetTileState(std::pair<int, int> tile, LandscapeState state)
{
	if (tile.first < 0 || tile.first >= width || tile.second < 0 || tile.second > height) return;

	tiles[tile.first][tile.second] = state;
}

int Landscape::GetWidth() const
{
	return width;
}

int Landscape::GetHeight() const
{
	return height;
}

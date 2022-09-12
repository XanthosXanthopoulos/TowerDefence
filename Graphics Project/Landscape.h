#pragma once

#include "Object.h"
#include "LandscapeState.h"

#include <string>
#include <functional>
#include <utility>
#include <unordered_map>

struct pair_hash
{
	template <class T1, class T2> std::size_t operator () (const std::pair<T1, T2> &p) const
	{
		auto h1 = std::hash<T1>{}(p.first);
		auto h2 = std::hash<T2>{}(p.second);

		return h1 ^ h2;
	}
};

class Landscape : public Object
{
public:

	static std::shared_ptr<Landscape> CreateInstance(std::shared_ptr<GeometryNode> model);

	virtual ~Landscape();

	virtual void Update(double delta);

	virtual void LoadLandscapeState(std::string filename);

	virtual void LoadPath(std::string filename);

	virtual std::pair<int, int> GetNextPosition(std::pair<int, int> currentPosition);

	virtual LandscapeState GetTileState(std::pair<int, int> tile);

	virtual void SetTileState(std::pair<int, int> tile, LandscapeState state);

	int GetWidth() const;

	int GetHeight() const;

protected:

	Landscape(std::shared_ptr<GeometryNode> model);

	int width;
	int height;

	LandscapeState** tiles;

	std::unordered_map<std::pair<int, int>, std::pair<int, int>, pair_hash> path;
};
// --------------------------------------------
// Pathfinding.h
// Pathfinding.cpp
// --------------------------------------------
// This is the pathfinding test. The user can
// place bots on the modifiable map and select
// a target destination that the bots will
// travel to.
// --------------------------------------------

#ifndef PATHFINDING_H
#define PATHFINDING_H

#include <SFML/Graphics.hpp>

#include "imgui.h"
#include "imgui-SFML.h"
#include "ThreadPool.h"
#include "Vec3.h"
#include "Timer.h"
#include "AStar.h"
#include "TileMap.h"

class Pathfinding
{
public:
	Pathfinding();
	~Pathfinding();
	void handleUI();
	void render();

private:
	static const unsigned int SCREEN_WIDTH = 1280u;
	static const unsigned int SCREEN_HEIGHT = 720u;
	std::unique_ptr<AStar> aStar;
	std::unique_ptr<TileMap> layer_0;
	std::unique_ptr<TileMap> layer_1;
	std::unique_ptr<sf::Texture> tileSet;
	sf::RenderTexture tileMap_RT;
	sf::RenderTexture debugMap_RT;
	int mapWidth = 256;
	int mapHeight = 256;
	int tileWidth = 16;
	int tileHeight = 16;
	int borderSize = 8;
	bool showTileMap = true;
	bool showDebugMap = true;
	std::vector<sf::RectangleShape> nodes;
	Vec3f nodeColour{ 0, 0, 1 };
	Vec3f connectionColour{ 0, 1, 0 };
	Vec3f pathColour{ 1, 1, 1 };
	Vec3f botColour{ 1, 0, 0 };
	std::vector<int> mapData;
	sf::View windowView;
};

#endif // !PATHFINDING_H
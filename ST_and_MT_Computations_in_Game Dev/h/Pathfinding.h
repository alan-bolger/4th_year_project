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
	sf::RenderWindow window;
	AStar *aStar;
	int mapWidth = 32;
	int mapHeight = 32;
	int nodeWidth = 16;
	int nodeHeight = 16;
	int borderSize = 8;
	std::vector<sf::RectangleShape> nodes;
	Vec3f nodeColour{ 0, 0, 1 };
	Vec3f connectionColour{ 0, 1, 0 };
	Vec3f pathColour{ 1, 1, 1 };
	Vec3f botColour{ 1, 0, 0 };
	std::vector<int> mapData;
	sf::View windowView;
};

#endif // !PATHFINDING_H
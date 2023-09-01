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
#include <sstream>

#include "imgui.h"
#include "imgui-SFML.h"
#include "ThreadPool.h"
#include "Vec3.h"
#include "Timer.h"
#include "TileMap.h"
#include "Bot.h"

class Pathfinding
{
public:
	Pathfinding();
	~Pathfinding();
	void update(const sf::Time &dt);
	void handleUI();
	void render();

private:
	static const unsigned int SCREEN_WIDTH = 1280u;
	static const unsigned int SCREEN_HEIGHT = 720u;
	std::unique_ptr<TileMap> layer_0;
	std::unique_ptr<TileMap> layer_1;
	std::unique_ptr<sf::Texture> tileSet;
	sf::RenderTexture tileMap_RT;
	std::unique_ptr<sf::RenderTexture> main_RT;
	std::unique_ptr<ThreadPool> threadPool;
	float zoom = 1.0f;
	int mapWidth = 256;
	int mapHeight = 256;
	int tileWidth = 16;
	int tileHeight = 16;
	bool showTileMap = true;
	bool showDebugMap = false;
	bool showBots = true;
	bool showPaths = true;
	float botSpeed = 0.5f;
	bool multiThreaded = false;
	bool placeBotsMode = true;
	sf::Vector2i destinationNode{ 4, 4 };
	sf::Vector2f renderWindowMousePos;
	ImVec2 renderWindowSize{ SCREEN_WIDTH, SCREEN_HEIGHT };
	ImVec2 mousePos;
	std::vector<sf::RectangleShape> nodes;
	Vec3f nodeColour{ 0, 0, 1 };
	Vec3f obstacleColour{ 1, 0, 0 };
	Vec3f passableColour{ 0, 1, 0 };
	sf::View windowView;
	std::vector<Bot*> bots;
	bool mouseLeftButtonClicked = false;
	double ms;

	void loadDemoBots();
	void writeBotPositionsToFile();
	void startPathfinding(bool multiThreaded);
};

#endif // !PATHFINDING_H
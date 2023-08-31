// --------------------------------------------
// TerrainGenerator.h
// TerrainGenerator.cpp
// --------------------------------------------
// This test generates a massive terrain map
// using Simplex Noise.
// --------------------------------------------

#ifndef TERRAINGENERATOR_H
#define TERRAINGENERATOR_H

#include "imgui.h"
#include "imgui-SFML.h"
#include "ThreadPool.h"
#include "Noise.h"

#include <SFML/Graphics.hpp>

static const sf::Color MAP_PALETTE[10] =
{
	sf::Color(0, 102, 255, 255),
	sf::Color(55, 125, 235, 255),
	sf::Color(0, 150, 0, 255),
	sf::Color(25, 150, 25, 255),
	sf::Color(50, 175, 50, 255),
	sf::Color(75, 175, 75, 255),
	sf::Color(100, 200, 100, 255),
	sf::Color(125, 200, 125, 255),
	sf::Color(150, 225, 150, 255),
	sf::Color(200, 255, 200, 255)
};

class TerrainGenerator
{
public:
	TerrainGenerator();
	~TerrainGenerator();
	void generate(int width, int height, int seed, std::vector<int> &array);
	void update(const sf::Time &dt);
	void handleUI();
	void render();

private:
	std::unique_ptr<Noise> noise;
	std::vector<int> heightMap;
	int mapWidth = 128;
	int mapHeight = 128;
	int tileW = 16;
	int tileH = 16;
	bool multiThreaded = false;
	std::unique_ptr<sf::RenderTexture> tileMap_RT;
	std::unique_ptr<sf::RenderTexture> main_RT;
	sf::View windowView;
	float zoom = 1.0f;
	ImVec2 renderWindowSize{ 1280, 720 };
};

#endif // !TERRAINGENERATOR_H
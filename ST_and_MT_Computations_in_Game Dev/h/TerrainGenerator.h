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
#include "Timer.h"

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
	void generate(int width, int height, int seed, std::vector<float> &array);
	void generateSection(sf::Vector2i pixTL, sf::Vector2i pixBR);
	void update(const sf::Time &dt);
	void handleUI();
	void render();

private:
	std::unique_ptr<Noise> noise;
	std::vector<float> heightMap;
	std::unique_ptr<ThreadPool> threadPool;
	int mapWidth = 960;
	int mapHeight = 960;
	int seed = 0;
	bool multiThreaded = false;
	std::unique_ptr<sf::Texture> texture;
	int waterHeight = 0;
	std::random_device rd;
	std::mt19937 mt;
	std::vector<uint8_t> pixelArray;
	ImVec2 renderWindowSize{ 1280, 720 };
	int availableThreads = 0;
};

#endif // !TERRAINGENERATOR_H
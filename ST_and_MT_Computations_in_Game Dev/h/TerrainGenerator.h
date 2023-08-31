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

static const sf::Color EARTH[10] =
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

static const sf::Color MARS[10] =
{
	sf::Color(212, 50, 0, 255),
	sf::Color(212, 70, 15, 255),
	sf::Color(235, 90, 30, 255),
	sf::Color(235, 110, 50, 255),
	sf::Color(235, 130, 70, 255),
	sf::Color(235, 150, 90, 255),
	sf::Color(235, 170, 110, 255),
	sf::Color(235, 190, 130, 255),
	sf::Color(235, 210, 150, 255),
	sf::Color(235, 230, 170, 255)
};

static const sf::Color MOON[10] =
{
	sf::Color(50, 50, 50, 255),
	sf::Color(60, 60, 60, 255),
	sf::Color(70, 70, 70, 255),
	sf::Color(80, 80, 80, 255),
	sf::Color(90, 90, 90, 255),
	sf::Color(100, 100, 100, 255),
	sf::Color(110, 110, 110, 255),
	sf::Color(120, 120, 120, 255),
	sf::Color(130, 130, 130, 255),
	sf::Color(140, 140, 140, 255)
};

class TerrainGenerator
{
public:
	TerrainGenerator();
	~TerrainGenerator();
	void generate(int width, int height, int seed, std::vector<float> &array);
	void generateSection(sf::Vector2i pixTL, sf::Vector2i pixBR);
	void handleUI();
	void render();

private:
	std::unique_ptr<Noise> noise;
	std::vector<float> heightMap;
	std::unique_ptr<ThreadPool> threadPool;
	int mapWidth = 960;
	int mapHeight = 960;
	int seed = 0;
	int enteredSeed = 0;
	bool multiThreaded = false;
	std::unique_ptr<sf::Texture> texture;
	std::random_device rd;
	std::mt19937 mt;
	bool randomiseMap = false;
	std::vector<uint8_t> pixelArray;
	ImVec2 renderWindowSize{ 1280, 720 };
	int availableThreads = 0;
	double ms;
	int currentTerrainID = 0;
};

#endif // !TERRAINGENERATOR_H
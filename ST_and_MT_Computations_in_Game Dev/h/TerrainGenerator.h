// --------------------------------------------
// TerrainGenerator.h
// TerrainGenerator.cpp
// --------------------------------------------
// This test generates a massive terrain map
// using Simplex Noise.
// --------------------------------------------

#ifndef TERRAINGENERATOR_H
#define TERRAINGENERATOR_H

#include "Noise.h"

#include <SFML/Graphics.hpp>

class TerrainGenerator
{
public:
	TerrainGenerator();
	~TerrainGenerator();
	void generate(int width, int height, int seed, std::vector<float> &array);
	void update(const sf::Time &dt);
	void handleUI();
	void render();

private:
	Noise noise;
	std::vector<float> heightMap;
};

#endif // !TERRAINGENERATOR_H
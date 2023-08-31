#include "TerrainGenerator.h"

/// <summary>
/// TerrainGenerator constructor.
/// </summary>
TerrainGenerator::TerrainGenerator()
{
	// Test map
	generate(64, 64, 500, heightMap);
}

/// <summary>
/// TerrainGenerator destructor.
/// </summary>
TerrainGenerator::~TerrainGenerator()
{
	
}

/// <summary>
/// Generate a terrain/height map.
/// </summary>
/// <param name="width">The width of the map.</param>
/// <param name="height">The height of the map.</param>
/// <param name="seed">A seed value for map randomization. Any given seed will always produce the same result.</param>
/// <param name="array">The height map is stored here.</param>
void TerrainGenerator::generate(int width, int height, int seed, std::vector<float> &array)
{
	// Clear the height map if not empty
	if (array.size() > 0)
	{
		array.clear();
	}

	array.resize(width * height);

	// Generate terrain
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			float freqX = x / static_cast<float>(width) - 0.5f;
			float freqY = y / static_cast<float>(height) - 0.5f;

			//double e = (1.00f * noise.generate(1.0 * (double)freqX, 1.0 * (double)freqY)
			//	+ 0.50f * noise.generate(2.0 * (double)freqX + seed, 2.0 * (double)freqY + seed)
			//	+ 0.25f * noise.generate(4.0 * (double)freqX + seed, 4.0 * (double)freqY + seed)
			//	+ 0.13f * noise.generate(8.0 * (double)freqX + seed, 8.0 * (double)freqY + seed)
			//	+ 0.06f * noise.generate(16.0 * (double)freqX + seed, 16.0 * (double)freqY + seed)
			//	+ 0.03f * noise.generate(32.0 * (double)freqX + seed, 32.0 * (double)freqY + seed));

			//e /= (3.00 + 0.50 + 0.25 + 0.13 + 0.06 + 0.03);
			//e = pow(e, 4.0f);
			//array[y * width + x] = e;

			array[y * width + x] = noise.generate(1.0 * (double)freqX, 1.0 * (double)freqY);
		}
	}
}

/// <summary>
/// Update.
/// </summary>
/// <param name="dt">Delta time.</param>
void TerrainGenerator::update(const sf::Time &dt)
{

}

/// <summary>
/// Update and draw all ImGui menus.
/// </summary>
void TerrainGenerator::handleUI()
{

}

/// <summary>
/// Draw the terrain map.
/// </summary>
void TerrainGenerator::render()
{

}

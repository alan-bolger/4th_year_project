#include "TerrainGenerator.h"

/// <summary>
/// TerrainGenerator constructor.
/// </summary>
TerrainGenerator::TerrainGenerator() : mt(rd())
{
	noise = std::make_unique<Noise>();

	// Default map
	generate(mapWidth, mapHeight, 500, heightMap);

	pixelArray.resize(mapWidth * mapHeight * 4); // RGBA values stored here

	texture = std::make_unique<sf::Texture>();
	texture->create(mapWidth, mapHeight);

	render(); // Update map so it's visible on startup

	availableThreads = std::thread::hardware_concurrency();

	threadPool = std::make_unique<ThreadPool>(availableThreads);
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

	// Store futures in here
	std::vector<std::future<void>> futures;

	Timer timer("Terrain Generator");

	if (multiThreaded)
	{
		int sectionW = mapWidth / 16;
		int sectionH = mapHeight / 16;

		// Add the jobs to the thread pool
		for (int y = 0; y < 16; ++y)
		{
			for (int x = 0; x < 16; ++x)
			{
				// This lambda adds a block of code to the thread pool as a job
				auto f = threadPool->addJob([=]
					{
						int startX = x * sectionW;
						int startY = y * sectionH;

						int endX = startX + sectionW;
						int endY = startY + sectionH;

						generateSection(sf::Vector2i(startX, startY), sf::Vector2i(endX, endY));
					});

				futures.push_back(std::move(f));
			}
		}
	}
	else
	{
		generateSection({ 0, 0 }, { mapWidth, mapHeight });

		timer.stop();
	}

	if (multiThreaded)
	{
		// Wait for all threads to finish
		for (auto &future : futures)
		{
			future.wait();
		}

		timer.stop();
	}

	float min = 0;
	float max = 0;

	// Initialise min and max from first two elevation map elements
	if (heightMap[0] > heightMap[1])
	{
		max = heightMap[0];
		min = heightMap[1];
	}
	else
	{
		max = heightMap[1];
		min = heightMap[0];
	}

	// First pass of array to find MIN and MAX values
	for (int y = 0; y < mapHeight; ++y)
	{
		for (int x = 0; x < mapWidth; ++x)
		{
			float yValue = heightMap[y * mapWidth + x];

			// Set min and max values from array
			if (yValue > max)
			{
				max = yValue;
			}
			else if (yValue < min)
			{
				min = yValue;
			}
		}
	}

	float yValue = 0.0f;

	// Second pass of array to normalise values using MIN and MAX
	for (int y = 0; y < mapHeight; ++y)
	{
		for (int x = 0; x < mapWidth; ++x)
		{
			float arrVal = heightMap[y * mapWidth + x];
			heightMap[y * mapWidth + x] = noise->normaliseToRange(arrVal, min, max) * 10; // Between 0 and 9;

			if (heightMap[y * mapWidth + x] > 9)
			{
				heightMap[y * mapWidth + x] = 9;
			}

			if (heightMap[y * mapWidth + x] < 0)
			{
				heightMap[y * mapWidth + x] = 0;
			}
		}
	}
}

/// <summary>
/// Generates a block of terrain to the given dimensions.
/// </summary>
/// <param name="pixTL">The coordinate of the upper-left corner of the section.</param>
/// <param name="pixBR">The coordinate of the lower-right corner of the section.</param>
void TerrainGenerator::generateSection(sf::Vector2i pixTL, sf::Vector2i pixBR)
{
	// Generate terrain
	for (unsigned int y = pixTL.y; y < pixBR.y; ++y)
	{
		for (unsigned int x = pixTL.x; x < pixBR.x; ++x)
		{
			int index = (y * mapWidth + x);

			// Out of bounds check
			if (index < 0 || (index * 4) >(pixelArray.size() - 1))
			{
				continue;
			}

			float freqX = x / static_cast<float>(mapWidth) - 0.5f;
			float freqY = y / static_cast<float>(mapHeight) - 0.5f;

			double e = (1.00f * noise->generate(1.0 * (double)freqX, 1.0 * (double)freqY)
				+ 0.50f * noise->generate(2.0 * (double)freqX + seed, 2.0 * (double)freqY + seed)
				+ 0.25f * noise->generate(4.0 * (double)freqX + seed, 4.0 * (double)freqY + seed)
				+ 0.13f * noise->generate(8.0 * (double)freqX + seed, 8.0 * (double)freqY + seed)
				+ 0.06f * noise->generate(16.0 * (double)freqX + seed, 16.0 * (double)freqY + seed)
				+ 0.03f * noise->generate(32.0 * (double)freqX + seed, 32.0 * (double)freqY + seed));

			e /= (3.00 + 0.50 + 0.25 + 0.13 + 0.06 + 0.03);
			e = pow(e, 4.0f);
			heightMap[y * mapWidth + x] = e;
		}
	}
}

/// <summary>
/// Update and draw all ImGui menus.
/// </summary>
void TerrainGenerator::handleUI()
{
	ImGui::PushItemWidth(0);

	if (ImGui::CollapsingHeader("Terrain Generator"))
	{
		// Particle properties
		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		ImGui::SeparatorText("Map Size");

		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		ImGui::InputInt("Width", &mapWidth);
		ImGui::InputInt("Height", &mapHeight);

		mapWidth = std::clamp(mapWidth, 32, 16384);
		mapHeight = std::clamp(mapHeight, 32, 16384);

		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		std::string strX = "Seed Used: " + std::to_string(seed);

		ImGui::Text(strX.c_str());

		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		ImGui::SliderInt("Water Height", &waterHeight, 0, 9);

		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		if (ImGui::Button("Generate Map"))
		{
			texture = std::make_unique<sf::Texture>();
			texture->create(mapWidth, mapHeight);

			std::uniform_int_distribution<int> dist(0, 65535);
			seed = dist(mt);
			generate(mapWidth, mapHeight, seed, heightMap);
			render();
		}

		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		ImGui::SeparatorText("Thread Usage");

		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		static int sel_6gtr = 0;
		ImGui::RadioButton("Single-threaded", &sel_6gtr, 0);
		ImGui::RadioButton("Multi-threaded", &sel_6gtr, 1);
		sel_6gtr == 0 ? multiThreaded = false : multiThreaded = true;

		ImGui::Dummy(ImVec2(0.0f, 8.0f));
	}

	// Render output window
	ImGui::Begin("Terrain Generator");

	ImGui::Image(*texture);

	ImGui::End();
}

/// <summary>
/// Draw the terrain map.
/// This does NOT get done on every frame, instead the map is only
/// rendered when the terrain data is updated.
/// </summary>
void TerrainGenerator::render()
{
	pixelArray.resize(mapWidth * mapHeight * 4);

	// Set every value to 0
	std::fill(pixelArray.begin(), pixelArray.end(), 0);

	for (int y = 0; y < mapHeight; ++y)
	{
		for (int x = 0; x < mapWidth; ++x)
		{
			// Out of bounds check
			if (y < 0 || y >= mapHeight || x < 0 || x >= mapWidth)
			{
				continue;
			}

			// Get the height from the height map
			int index = y * mapWidth + x;
			int height = static_cast<int>(heightMap[index]);

			sf::Color tileColour;

			if (height >= waterHeight)
			{
				tileColour = MAP_PALETTE[height];
			}
			else
			{
				tileColour = MAP_PALETTE[0];
			}

			// Update pixel array - RGBA
			pixelArray[index * 4] = tileColour.r;
			pixelArray[(index * 4) + 1] = tileColour.g;
			pixelArray[(index * 4) + 2] = tileColour.b;
			pixelArray[(index * 4) + 3] = 255;
		}
	}

	texture->update(pixelArray.data());
}

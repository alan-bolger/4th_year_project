#include "TerrainGenerator.h"

/// <summary>
/// TerrainGenerator constructor.
/// </summary>
TerrainGenerator::TerrainGenerator() : mt(rd())
{
	noise = std::make_unique<Noise>();

	// Default map
	generate(mapWidth, mapHeight, 500, heightMap);

	main_RT = std::make_unique<sf::RenderTexture>();
	main_RT->create(mapWidth, mapHeight);
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

			double e = (1.00f * noise->generate(1.0 * (double)freqX, 1.0 * (double)freqY)
				+ 0.50f * noise->generate(2.0 * (double)freqX + seed, 2.0 * (double)freqY + seed)
				+ 0.25f * noise->generate(4.0 * (double)freqX + seed, 4.0 * (double)freqY + seed)
				+ 0.13f * noise->generate(8.0 * (double)freqX + seed, 8.0 * (double)freqY + seed)
				+ 0.06f * noise->generate(16.0 * (double)freqX + seed, 16.0 * (double)freqY + seed)
				+ 0.03f * noise->generate(32.0 * (double)freqX + seed, 32.0 * (double)freqY + seed));

			e /= (3.00 + 0.50 + 0.25 + 0.13 + 0.06 + 0.03);
			e = pow(e, 4.0f);
			array[y * width + x] = e;
		}
	}

	float min = 0;
	float max = 0;

	// Initialise min and max from first two elevation map elements
	if (array[0] > array[1])
	{
		max = array[0];
		min = array[1];
	}
	else
	{
		max = array[1];
		min = array[0];
	}

	// First pass of array to find MIN and MAX values
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			float yValue = array[y * width + x];

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
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			float arrVal = array[y * width + x];
			array[y * width + x] = noise->normaliseToRange(arrVal, min, max) * 10; // Between 0 and 9;

			if (array[y * width + x] > 9)
			{
				array[y * width + x] = 9;
			}

			if (array[y * width + x] < 0)
			{
				array[y * width + x] = 0;
			}
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
			main_RT = std::make_unique<sf::RenderTexture>();
			main_RT->create(mapWidth, mapHeight);

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

	// Draw content region for debug purposes
	// The content region position is also used to map the
	// mouse position correctly
	ImVec2 vMin = ImGui::GetWindowContentRegionMin();
	ImVec2 vMax = ImGui::GetWindowContentRegionMax();

	vMin.x += ImGui::GetWindowPos().x;
	vMin.y += ImGui::GetWindowPos().y;
	vMax.x += ImGui::GetWindowPos().x;
	vMax.y += ImGui::GetWindowPos().y;

	// ImGui::GetForegroundDrawList()->AddRect(vMin, vMax, IM_COL32(255, 255, 0, 255));

	// Get current mouse coordinates from ImGui window
	ImVec2 mousePos = ImGui::GetMousePos();

	// As the mouse position is relative to the entire window,
	// the region content position must be taken into account
	mousePos.x -= vMin.x;
	mousePos.y -= vMin.y;

	ImGui::Image(*main_RT);

	ImGui::End();
}

/// <summary>
/// Draw the terrain map.
/// This does NOT get done on every frame, instead the map is only
/// rendered when the terrain data is updated.
/// </summary>
void TerrainGenerator::render()
{
	main_RT->clear(sf::Color(0, 0, 0, 0));

	for (int y = 0; y < mapHeight; ++y)
	{
		for (int x = 0; x < mapWidth; ++x)
		{
			// Just in case because you never know.....
			if (y < 0 || y >= mapHeight || x < 0 || x >= mapWidth)
			{
				continue;
			}

			int height = static_cast<int>(heightMap[y * mapWidth + x]);

			sf::Color tileColour;

			if (height >= waterHeight)
			{
				tileColour = MAP_PALETTE[height];
			}
			else
			{
				tileColour = MAP_PALETTE[0];
			}

			sf::Vector2i f_topLeft = sf::Vector2i(x, y);

			sf::Vertex vertices[] =
			{
				sf::Vertex(sf::Vector2f(f_topLeft), tileColour),
				sf::Vertex(sf::Vector2f(f_topLeft += { 1, 0 }), tileColour),
				sf::Vertex(sf::Vector2f(f_topLeft += { 1, 1 }), tileColour),
				sf::Vertex(sf::Vector2f(f_topLeft += { 0, 1 }), tileColour),
			};

			main_RT->draw(vertices, 4, sf::Quads);
		}
	}

	main_RT->display();
}

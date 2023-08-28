#include "Pathfinding.h"

/// <summary>
/// Pathfinding constructor.
/// </summary>
Pathfinding::Pathfinding()
{
 	// Configure tile map
	tileSet = std::make_unique<sf::Texture>();
	tileSet->loadFromFile("assets/dungeon_tileset.png");

	layer_0 = std::make_unique<TileMap>();
	layer_1 = std::make_unique<TileMap>();

	layer_0->loadTileMap("assets/pathfinding_map_layer_0.txt", tileSet.get(), tileWidth, tileHeight, mapWidth, mapHeight);
	layer_1->loadTileMap("assets/pathfinding_map_layer_1.txt", tileSet.get(), tileWidth, tileHeight, mapWidth, mapHeight);

	tileMap_RT.create(tileWidth * mapWidth, tileHeight * mapHeight);

	windowView.setSize(tileWidth * mapWidth, tileHeight * mapHeight);
	windowView.setCenter((tileWidth * mapWidth) / 2, (tileHeight * mapHeight) / 2);

	main_RT = std::make_unique<sf::RenderTexture>();
	main_RT->create(SCREEN_WIDTH, SCREEN_HEIGHT);

	threadPool = std::make_unique<ThreadPool>(std::thread::hardware_concurrency());
}

/// <summary>
/// Pathfinding constructor.
/// </summary>
Pathfinding::~Pathfinding()
{

}

/// <summary>
/// Update.
/// </summary>
/// <param name="dt">Delta time.</param>
void Pathfinding::update(const sf::Time &dt)
{
	for (int i = 0; i < bots.size(); ++i)
	{
		bots.at(i)->update(botSpeed);
	}
}

/// <summary>
/// Update and draw all ImGui menus.
/// </summary>
void Pathfinding::handleUI()
{
    ImGui::PushItemWidth(0);

    if (ImGui::CollapsingHeader("Map"))
    {
        // Cosmetic options
        ImGui::Dummy(ImVec2(0.0f, 8.0f));

        ImGui::SeparatorText("Colour Scheme");

        ImGui::Dummy(ImVec2(0.0f, 8.0f));

        ImGui::ColorEdit3("Nodes", nodeColour.get());

        ImGui::Dummy(ImVec2(0.0f, 8.0f));

        ImGui::ColorEdit3("Obstacles", obstacleColour.get());

        ImGui::Dummy(ImVec2(0.0f, 8.0f));

		ImGui::ColorEdit3("Passable", passableColour.get());

		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		ImGui::Checkbox("Show Tile Map", &showTileMap);
		ImGui::Checkbox("Show Debug Map", &showDebugMap);

		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		if (ImGui::Button("Zoom -"))
		{
			zoom *= 2.0f;
			windowView.zoom(2.0f);
		}

		if (ImGui::Button("Zoom +"))
		{
			zoom *= 0.5f;
			windowView.zoom(0.5f);
			windowView.setCenter(windowView.getCenter());
		}

		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		ImGui::SeparatorText("Mouse Positon (Map Coordinates)");

		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		std::string strX = "X: " + std::to_string(renderWindowMousePos.x);
		std::string strY = "Y: " + std::to_string(renderWindowMousePos.y);

		ImGui::Text(strX.c_str());
		ImGui::Text(strY.c_str());

		ImGui::Dummy(ImVec2(0.0f, 8.0f));
    }

	if (ImGui::CollapsingHeader("Bots"))
	{
		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		ImGui::Text("This is the time taken between\nbot movements (in seconds)");

		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		ImGui::InputFloat("Speed", &botSpeed);

		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		ImGui::SeparatorText("Edit Mode");

		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		ImGui::Text("Place bots or the destination\nnode using the mouse and left-button");

		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		static int selA = 0;
		ImGui::RadioButton("Place Bots", &selA, 0);
		ImGui::RadioButton("Select Destination", &selA, 1);
		selA == 0 ? placeBotsMode = true : placeBotsMode = false;

		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		ImGui::SeparatorText("Destination Node (Tile Coords)");

		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		std::string strX = "X: " + std::to_string(destinationNode.x);
		std::string strY = "Y: " + std::to_string(destinationNode.y);

		ImGui::Text(strX.c_str());
		ImGui::Text(strY.c_str());

		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		ImGui::Checkbox("Show Bots", &showBots);
		ImGui::Checkbox("Show Paths", &showPaths);

		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		ImGui::SeparatorText("Thread Usage");

		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		static int sel = 0;
		ImGui::RadioButton("Single-threaded", &sel, 0);
		ImGui::RadioButton("Multi-threaded", &sel, 1);
		sel == 0 ? multiThreaded = false : multiThreaded = true;

		ImGui::Dummy(ImVec2(0.0f, 8.0f));

		if (ImGui::Button("Start Pathfinding"))
		{
			startPathfinding(multiThreaded);
		}

		ImGui::Dummy(ImVec2(0.0f, 8.0f));
	}

	// Render output window
	ImGui::Begin("Render");

	// Draw content region for debug purposes
	// The content region position is also used to map the
	// mouse position correctly
	ImVec2 vMin = ImGui::GetWindowContentRegionMin();
	ImVec2 vMax = ImGui::GetWindowContentRegionMax();

	vMin.x += ImGui::GetWindowPos().x;
	vMin.y += ImGui::GetWindowPos().y;
	vMax.x += ImGui::GetWindowPos().x;
	vMax.y += ImGui::GetWindowPos().y;

	ImGui::GetForegroundDrawList()->AddRect(vMin, vMax, IM_COL32(255, 255, 0, 255));

	// Handle ImGui window resize
	ImVec2 currentWindowSize = ImGui::GetWindowSize();

	if (currentWindowSize.x != renderWindowSize.x || currentWindowSize.y != renderWindowSize.y)
	{
		renderWindowSize = currentWindowSize;

		ImVec2 vMin = ImGui::GetWindowContentRegionMin();
		ImVec2 vMax = ImGui::GetWindowContentRegionMax();

		float sizeX = vMax.x - vMin.x;
		float sizeY = vMax.y - vMin.y;

		main_RT = std::make_unique<sf::RenderTexture>();
		main_RT->create(sizeX, sizeY);
	}

	main_RT->clear(sf::Color::Transparent);

	// Get current mouse coordinates from ImGui window
	ImVec2 mousePos = ImGui::GetMousePos();

	// As the mouse position is relative to the entire window,
	// the region content position must be taken into account
	mousePos.x -= vMin.x;
	mousePos.y -= vMin.y;

	// Use the render texture view to convert the mouse position to world coordinates
	renderWindowMousePos = tileMap_RT.mapPixelToCoords(sf::Vector2i(mousePos.x, mousePos.y));
	sf::Vector2i tileCoords = sf::Vector2i(renderWindowMousePos.x / tileWidth, renderWindowMousePos.y / tileHeight);

	// Draw cursor
	sf::RectangleShape rect(sf::Vector2f(tileWidth, tileHeight));
	rect.setOutlineThickness(-1.0f);
	rect.setFillColor(sf::Color::Transparent);
	rect.setPosition(tileCoords.x * tileWidth, tileCoords.y * tileHeight);
	rect.setOutlineColor(placeBotsMode ? sf::Color::Red : sf::Color::Green);

	tileMap_RT.draw(rect);

	sf::Sprite renderSprite(tileMap_RT.getTexture());
	main_RT->draw(renderSprite);

	main_RT->display();

	ImGui::Image(*main_RT);

	ImGui::End();

	// Controls for moving map
	float jumpRange = 5.0f;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		windowView.move({ 0.0f, -jumpRange });
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		windowView.move({ 0.0f, jumpRange });
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		windowView.move({ -jumpRange, 0.0f });
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		windowView.move({ jumpRange, 0.0f });
	}

	if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !mouseLeftButtonClicked)
	{
		mouseLeftButtonClicked = true;
	}

	if (!sf::Mouse::isButtonPressed(sf::Mouse::Left) && mouseLeftButtonClicked)
	{
		// Simply to array index value
		int index = tileCoords.y * mapWidth + tileCoords.x;

		// Prevent out-of-bounds access
		if ((index) >= 0 && (index) < (mapWidth * mapHeight))
		{
			// Bots can only be placed on empty floors
			// This also works when setting the destination node
			if (layer_0->getTileArray()->at(index) != 0)
			{
				if (layer_1->getTileArray()->at(index) == 0)
				{
					bool canPlace = true;

					// This is kind of inefficient, but it works - it makes sure you can't
					// place a bot over an existing bot
					// Also, how cool is it to see 'auto bot' in a range-based for loop
					for (const auto &bot : bots)
					{
						if (bot->getPosition() == tileCoords)
						{
							canPlace = false;
						}
					}

					if (canPlace)
					{
						if (placeBotsMode)
						{
							bots.push_back(new Bot(tileCoords.x, tileCoords.y, *layer_1->getTileArray()));
						}
						else
						{
							// This is to prevent the destination node being updated with incorrect values when
							// the mouse button is clicked outside of the render window
							if (tileCoords.x >= 0 && tileCoords.x < mapWidth && tileCoords.y >= 0 && tileCoords.y < mapHeight)
							{
								destinationNode = tileCoords;
							}							
						}
					}
				}
			}
		}		

		mouseLeftButtonClicked = false;
	}
}

/// <summary>
/// Draws the map and shows bots, paths, etc.
/// </summary>
void Pathfinding::render()
{
	tileMap_RT.clear(sf::Color(0, 0, 0, 0));
	tileMap_RT.setView(windowView);

	if (showTileMap)
	{
		// Draw map
		layer_0->draw(tileMap_RT, zoom);
		layer_1->draw(tileMap_RT, zoom);
	}

	if (showDebugMap)
	{
		// Draw nodes
		sf::RectangleShape nodeSquare;
		nodeSquare.setSize(sf::Vector2f(tileWidth / 2, tileHeight / 2));

		for (int x = 0; x < mapWidth; ++x)
		{
			for (int y = 0; y < mapHeight; ++y)
			{
				nodeSquare.setFillColor(sf::Color(nodeColour.x * 255, nodeColour.y * 255, nodeColour.z * 255));

				if (layer_0->getTileArray()->at(y * mapWidth + x) != 0) // Node is passable
				{
					nodeSquare.setFillColor(sf::Color(passableColour.x * 255, passableColour.y * 255, passableColour.z * 255));
				}

				if (layer_1->getTileArray()->at(y * mapWidth + x) != 0) // Node is an obstacle
				{
					nodeSquare.setFillColor(sf::Color(obstacleColour.x * 255, obstacleColour.y * 255, obstacleColour.z * 255));
				}

				nodeSquare.setPosition((x * tileWidth) + (tileWidth / 4), (y * tileHeight) + (tileWidth / 4));
				tileMap_RT.draw(nodeSquare);
			}
		}
	}

	if (showBots)
	{
		for (int i = 0; i < bots.size(); ++i)
		{
			bots.at(i)->draw(tileMap_RT, showPaths);
		}
	}

	tileMap_RT.display();
}

/// <summary>
/// Begin the pathfinding algorithm.
/// </summary>
/// <param name="multiThreaded">False for single-threaded and true for multi-threaded.</param>
void Pathfinding::startPathfinding(bool multiThreaded)
{
	if (multiThreaded)
	{
		// TODO: Add timer here
		for (auto &bot : bots)
		{
			// This lambda adds a block of code to the thread pool as a job
			threadPool->addJob([=]
				{
					bot->startPathfinding(destinationNode.x, destinationNode.y);
				});
		}
	}
	else
	{
		// TODO: Add timer here
		for (auto &bot : bots)
		{
			bot->startPathfinding(destinationNode.x, destinationNode.y);
		}
	}
}
